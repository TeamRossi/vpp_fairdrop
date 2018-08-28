//
//  main.c
//  bw-static
//
//  Created by jroberts on 03/08/2018.
//  Copyright © 2018 jroberts. All rights reserved.
//


#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define N 3    // number of flows
#define Ng 3    // number of groups

// Parameters

float load;              // load
float dist[N];    // load distribution (sums to 1)
int buffer, VQthresh;
double fload[N];
double theta;  // theta (<=1) is virtual queue service rate when real service time is 1
int hierarchical;  // indicator for sharing mode
// Variables

double t;    // time
double t_last=0;    // time of last arrival rounded up for complete virtual scheduler cycle
double tstart;  // start time for metrics
int r_qtot;       // total queue sizes
int r_q[N];       // queue sizes
double v_q[N];   // real (fluid) virtual queue sizes
double r_qt[N],v_qt[N];   // cumulative queue sizes
double bt[N],st[N];   // number of blocked arrivals
double at;      // arrival time
double r_dt;      // departure time
double w[N];
int gp[N];  // group to which each flow belongs
double wg[Ng];  // weights of groups

int nbl=0;  // number of backlogged flows
double sw[Ng], swg;  // sw[g] is sum of weights w[j] of backlogged flows j in group g, swg is sum of wg[g] for backlogged g

struct r_Queue  // real fifo queue
{
    int r_packet;
    struct r_Queue* r_next;
}  *r_head, *r_tail;

struct activelist  // list of active flows
{
    int activeflow;
    struct activelist* nextflow;
}  *headflow, *tailflow;


// Random variables

double uniform()
{
    return((double) random()/RAND_MAX);
}

double exponential()
{
    return((double) -log(1.-uniform()));
}

int packetarrival()
{
    int i=0;
    double u=uniform();
    while (u>dist[i])
    {
        u-=dist[i];
        i++;
    }
    if (i>=N) {
        printf("error in packetarrival; i=%d\n",i);
        exit(0);
    }
    else return i;
}

int r_dequeue()
{
    struct r_Queue *temp;
    int i;
    temp=r_head;
    i=r_head->r_packet;
    r_head=r_head->r_next;
    free(temp);
    return i;
}


void r_enqueue(int i)
{
    struct r_Queue *temp;
    temp=(struct r_Queue *)malloc(sizeof(struct r_Queue));
    temp->r_packet=i;
    temp->r_next=NULL;
    if (r_head == NULL)
    {
        r_tail=temp;
        r_head=temp;
    }
    else
    {
        r_tail->r_next=temp;
        r_tail=temp;
    }
}

int flowout()
{
    struct activelist *temp;
    int i;
    temp=headflow;
    i=headflow->activeflow;
    headflow=headflow->nextflow;
    free(temp);
    return i;
}

void flowin(int i)
{
    struct activelist *temp;
    temp=(struct activelist *)malloc(sizeof(struct activelist));
    temp->activeflow=i;
    temp->nextflow=NULL;
    if (headflow == NULL)
    {
        tailflow=temp;
        headflow=temp;
    }
    else
    {
        tailflow->nextflow=temp;
        tailflow=temp;
    }
}

void r_printqueue()
{
    struct r_Queue *temp;
    temp=r_head;
    printf("r_Queue state: ");
    while (temp!=NULL)
    {
        printf("%d ",temp->r_packet+1);
        temp=temp->r_next;
    }
    printf("\n");
    printf("Queue length = %d\n",r_qtot);
}


// Initialization

void initqueue()
{
    int i;
    r_qtot=0;
    for (i=0;i<N;i++)
    {
        v_q[i]=0;
        r_q[i]=0;
    }
    at=0.;
    r_dt=1;
    r_head=NULL;
    headflow=NULL;
    nbl=0;
    swg=0;
    for (i=0;i<Ng;i++) sw[i]=0;
}

void initmetric()
{
    int i;
    tstart=t;
    for (i=0;i<N;i++)
    {
        r_qt[i]=0.;
        v_qt[i]=0.;
        bt[i]=0;
        st[i]=0;
    }
}


void printqueue()
{
    int i;
    for (i=0;i<N;i++) printf(" %d: %e",i+1,v_q[i]);
    printf("\n");
}


// Simulation




void vstate6(int i)   // update state for weighted fairness, with groups, using activelist
{
    int j, g, flows;
    double credit, newcredit, alloc;   // amount of per flow backlog served, credit to be redistributed from flows that empty prematurely
    double newswg,newsw[Ng];   // sums of weights updated, like credit, after each cycle
    int oldnbl=nbl+1;  // nbl is number of backlogged flows
    
    credit=(t-t_last)*theta;  // service capacity to be shared
    while (credit>0 && nbl>0) {
        oldnbl=nbl;
        newswg=swg;
        for (g=0;g<Ng;g++) newsw[g]=sw[g];
        newcredit=0;
        for (flows=0; flows<oldnbl; flows++) {  // do following for oldnbl backlogged flows
            j=flowout();  // extract flow from active list
            if (v_q[j]>0) {
                g=gp[j];
                alloc=credit*wg[g]/swg*w[j]/sw[g];
                if (v_q[j]>alloc) {
                    v_q[j]-=alloc;
                    flowin(j);
                }
                else {   // flow ceases to be backlogged, adjust weight factors
                    newcredit+=alloc-v_q[j];
                    v_q[j]=0;
                    newsw[g]-=w[j];
                    if (newsw[g]==0) newswg-=wg[g];
                    nbl--;
                }
            }
        }
        swg=newswg;
        for (g=0;g<Ng;g++) sw[g]=newsw[g];
        credit=newcredit;
    }
    if (i>=0) {    //  add new arrival, update sums of weights, i==-1 implies arrival was dropped
        g=gp[i];
        if (sw[g]==0) swg+=wg[g];
        if (v_q[i]==0) {
            sw[g]+=w[i];
            nbl++;
            flowin(i);
        }
        v_q[i]+=1.;  // increment by packet length (=1 in this case)
    }
}

void vstate7(int i)   // update state for unweighted fairness, using activelist
{
    int j;
    double served, credit=0;   // amount of per flow backlog served, credit to be redistributed from flows that empty prematurely
    int oldnbl=nbl+1, flows;
    
    credit=(t-t_last)*theta;  // service capacity to be shared
    while (oldnbl>nbl && nbl>0) {
        oldnbl=nbl;
        served=credit/nbl;   // amount of per-flow service if all stay backlogged
        credit=0;
        for (flows=0; flows<oldnbl; flows++) {  // do following for all backlogged flows
            j=flowout();  // extract flow from active list
            if (v_q[j]>served) {
                v_q[j]-=served;
                flowin(j);  // put back at end of activelist
            }
            else {
                credit+=served-v_q[j];
                v_q[j]=0;
                nbl--;  // this flow is no longer in activelist; ID can be used later by new flow
            }
        }
    }
    if (i>=0) {
        if (v_q[i]==0) {
            nbl++;
            flowin(i);
        }
        v_q[i]+=1.;  // increment by one packet
    }
}


void arrival()
{
    int i=packetarrival();
    //    printqueue();
    if (v_q[i]<=VQthresh && r_qtot<buffer) // packet non-drop condition, uses queue sizes after last arrival to avoid delay to execute vstate()
    {
        if (hierarchical==0) vstate7(i);  // update virtual state prior to this arrival, then add 1 to v_q[i] (FQ)
        if (hierarchical==1) vstate6(i);  // update virtual state prior to this arrival, then add 1 to v_q[i] (HWFQ)
        r_enqueue(i);
        r_q[i]++;
        r_qtot++;
        st[i]+=1.;
    }
    else
    {
        if (hierarchical==0) vstate7(-1);  // update virtual state, negative argument means no packet arrival
        if (hierarchical==1) vstate6(-1);  // update virtual state, negative argument means no packet arrival
        bt[i]+=1.;
        //        if (r_qtot>=buffer) printf("buffer overflow %f %d\n",v_q[i], r_qtot);
        //        else printf("fair drop  %f %d\n",v_q[i], r_qtot);
    }
    t_last=t;
}


void r_departure()  // departure from real queue (FIFO)
{
    int j;
    if (r_qtot>0) {
        j=r_dequeue();
        if (r_q[j]<=0) {
            printf("anomaly: %d %d \n",j, r_q[j]);
            r_printqueue();
        }
        r_q[j]--;
        r_qtot--;
    }
}

void update(double delta)
{
    int i;
    for (i=0;i<N;i++) {
        v_qt[i]+=delta*v_q[i];
        r_qt[i]+=delta*r_q[i];
    }
}

void step()
{
    if (r_qtot==0)   // arrival to empty real queues
    {
        t=at;
        arrival();
        at=t+exponential()/load;
        r_dt=t+1;
    }
    else {
        if (at<r_dt)  // arrival
        {
            update(at-t);
            t=at;
            arrival();
            at=t+exponential()/load;
        }
        else //  real departure
        {
            update(r_dt-t);
            t=r_dt;
            r_departure();
            r_dt=t+1;
        }
    }
}

// Output

void test()
{
    int warmup=10000;
    int k;
    
    initqueue();
    for (k=0;k<warmup;k++)
        step();
    printqueue();
}

void printmetric()
{
    int i;
    double sum=0;
    
    for (i=0;i<N;i++) {
        printf(" %5.2f %6.3f %6.2f %5.2f %5.2f %5.2f ",fload[i],st[i]/(t-tstart),r_qt[i]/(t-tstart),r_qt[i]/st[i],v_qt[i]/(t-tstart),v_qt[i]/st[i]);
        sum+=st[i]/(t-tstart);
    }
    printf(" %5.2f\n",sum);
}


void performance()
{
    int warmup=100000;           // warm-up time (number of jumps)
    int steady=10000000;         // simulation time (number of jumps)
    int i,k;
    
    buffer=30;
    VQthresh=30;
    theta=1;  // virtual service rate <=1
    
    printf("buffer %d, VQthresh %d, theta %4.2f \n",buffer,VQthresh,theta);
    hierarchical=0;
    
    if (hierarchical==0) printf("Fair sharing\n");
    if (hierarchical==1) {  // use hierarchical fair sharing
        printf("Hierarchical weigthed fair sharing\n");
        w[0]=1;
        w[1]=1;
        w[2]=.2;
        gp[0]=0;
        gp[1]=1;
        gp[2]=1;
        wg[0]=2;
        wg[1]=1;
        wg[2]=1;
        
        for (i=0;i<N;i++) {
            printf("flow %d: weight %4.1f, group %d \n ",i,w[i],gp[i]);
        }
        for (i=0;i<Ng;i++) {
            printf("group %d: weight %4.1f \n",i,wg[i]);
        }
        
    }
    
    fload[0]=1.0;
    fload[1]=0.6;
    fload[2]=.1;
    load=0;
    for (i=0;i<N;i++) load+=fload[i];
    dist[N-1]=1;
    for (i=0;i<N-1;i++) {  // ensures sum dist = 1 for new packet arrival
        dist[i]=fload[i]/load;
        dist[N-1]-=dist[i];
    }
    
    printf("                flow 1                                 flow 2                                 flow 3\n");
    printf("  load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    util \n");
    
    initqueue();
    for (k=0;k<warmup;k++)
        step();
    initmetric();
    for (k=0;k<steady;k++) {
        step();
    }
    printmetric();
    
}


int main()
{
    //    srandom(time(NULL));  // Init the random generator on local time
    srandom(1);
    // test();
    performance();
}
