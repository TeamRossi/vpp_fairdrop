//
//  main.c
//  cpu-stat
//
//  Created by jroberts on 03/08/2018.
//  Copyright © 2018 jroberts. All rights reserved.
//
//


#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define N 3    // number of flows
#define Ng 1    // number of groups

// Parameters

float load=0;              // load
float dist[N];    // load distribution (sums to 1)
int buffer;
float VQthresh, min_thresh, max_thresh;
float alpha,beta;  // MIMD parameters
double fload[N];
int hierarchical;  // indicator for sharing mode
// Variables

double t;    // time
double t_last=0;    // time of last arrival rounded up for complete virtual scheduler cycle
double tstart;  // start time for metrics
int r_qtot;       // total queue sizes
int r_q[N];       // queue sizes
double v_q[N], deficit[N];   // real (fluid) virtual queue sizes
double r_qt[N],v_qt[N];   // cumulative queue sizes
double btt[N],btv[N],st[N];   // number of blocked arrivals
double at;      // arrival time
double pollevent, packettime;      //
double packetsize[N], sumrate;
double increment[N];
double overhead;
double cost=0;
int nbl=0;  // number of backlogged flows
int vector,burst=0,numserved=0;
int flowshist[200], nblsum, burstcount;
int bursthist[400], servedhist[400], bufferhist[400], numpolls;


struct r_Queue  // real fifo queue
{
    int r_packet;
    double timestamp;
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
    while (u>dist[i] && i<N-1)
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
    packettime=r_head->timestamp;
    r_head=r_head->r_next;
    free(temp);
    return i;
}


void r_enqueue(int i)
{
    struct r_Queue *temp;
    temp=(struct r_Queue *)malloc(sizeof(struct r_Queue));
    temp->r_packet=i;
    temp->timestamp=t;
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
        deficit[i]=0;
        r_q[i]=0;
        increment[i]=packetsize[i];
    }
    at=0.;
    r_head=NULL;
    headflow=NULL;
    nbl=0;
    cost=0;
}

void initmetric()
{
    int i;
    tstart=t;
    for (i=0;i<N;i++)
    {
        r_qt[i]=0.;
        v_qt[i]=0.;
        btt[i]=0;
        btv[i]=0;
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




void vstate7()   // update state for unweighted fairness, using activelist
{
    int j;
    double served, credit=0;   // amount of per flow backlog served, credit to be redistributed from flows that empty prematurely
    int oldnbl=nbl+1, flows;
    
    credit=(packettime-t_last);  // service capacity to be shared
    if (credit<0) credit=0;
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
    t_last=packettime;
}


void arrival()
{
    int i=packetarrival();
    if (r_qtot<buffer) // packet non-drop condition, uses queue sizes after last arrival to avoid delay to execute vstate()
    {
        r_enqueue(i);
        r_q[i]++;
        r_qtot++;
    }
    else
    {
        btt[i]+=1.;
    }
    at=t+exponential()/sumrate;
}

void polling2()  // vpp polling: take vector of packets from r_queue, update virtual queues, drop, add packets to vq's
// version with one vstate update per vector, using timestamp of first packet
{
    int i, j, flows;
    double tempvq[N];
    double flowwght[N],sumwght;
    pollevent=t;
    burst=0;
    numserved=0;
    
    
    
    while (r_qtot>0 && burst<vector) {
        burst=1;
        i=r_dequeue();  // sets 'packettime' to time stamp of this packet - used in vstate to update vq's
        
        
        vstate7();
        
        for (j=0;j<N;j++) tempvq[j]=v_q[j];
        for (j=0;j<N;j++) flowwght[j]=0;
        sumwght=0;
        
        //  printf(" - %6.3f \n",v_q[i]);
        r_qtot--;
        r_q[i]--;
        // drop or increment vq for head packet
        if (tempvq[i]>VQthresh)  {  // packet dropped, increment polling interval by overhead
            btv[i]+=1;
            pollevent+=overhead;
            cost+=overhead;
        }
        else {  // add packet to VQ and increment polling interval by packet size
            numserved++;
            st[i]+=1;
            if (tempvq[i]==0) {  // new flow for active list
                nbl++;
                flowin(i);
            }
            tempvq[i]+=increment[i];  // uses  increment to accept or drop later packets, vq will exceed threshold since this is underestimate
            cost+=packetsize[i];
            flowwght[i]+=packetsize[i];
            sumwght+=packetsize[i];
            pollevent+=packetsize[i];
        }
        
        // now drop or add packets to VQs for remaining packets
        
        while (r_qtot>0 && burst<vector) {
            
            burst++;
            i=r_dequeue();  // next packet in vector
            r_qtot--;
            r_q[i]--;
            
            if (tempvq[i]>VQthresh)  {  // packet dropped
                btv[i]+=1;
                pollevent+=overhead;
                cost+=overhead;
            }
            else {  // add packet to VQ and increment polling interval by packet size
                numserved++;
                st[i]+=1;
                if (tempvq[i]==0) {  // vq was and still empty
                    nbl++;
                    flowin(i);
                }
                tempvq[i]+=increment[i];
                cost+=packetsize[i];
                pollevent+=packetsize[i];
                flowwght[i]+=packetsize[i];
                sumwght+=packetsize[i];
            }
        }
        
        // adjust vq sizes sharing overhead in proportion to packetsize
        
        if (sumwght>0) { // at least one packet non-dropped, share cost over flows in proportion to packetsize
            for (flows=0; flows<nbl; flows++) {  // do following for all backlogged flows
                j=flowout();  // extract flow from active list
                if (flowwght[j]>0) increment[j]=packetsize[j]*cost/sumwght;  // increment to be used in drop decision
                v_q[j]+=flowwght[j]*cost/sumwght;
                //         printf("%d %4.2f %4.2f %4.2f     ",j,flowwght[j],flowwght[j]*cost/sumwght, increment[j]);
                flowin(j);
            }
            cost=0;
        }
        flowshist[nbl]++;  // sample nbl
        nblsum+=nbl;
        burstcount++;
    }
    
    // adaptive VQthresh
    
    if (burst==vector && VQthresh>min_thresh) VQthresh=VQthresh*alpha; if (VQthresh<min_thresh) VQthresh=min_thresh;
    else {if (burst<vector && VQthresh<max_thresh) VQthresh=VQthresh*beta; if (VQthresh>max_thresh) VQthresh=max_thresh;}
    
    bufferhist[r_qtot]++;
    bursthist[burst]++;
    servedhist[numserved]++;
    numpolls++;
    
    if (r_qtot==0 && pollevent==t) pollevent+=1;  // if no packets served and none in queue, try 1 time unit later
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
    
    // printf("%e %d %e %e \n",t,r_qtot,at,pollevent);
    
    if (at<pollevent)  // arrival
    {
        update(at-t);
        t=at;
        arrival();
        //            printf("arrival %e %d \n",t, r_qtot);
    }
    else //  poll
    {
        if (pollevent<t) printf("***** %e %e \n",t,pollevent);
        update(pollevent-t);
        t=pollevent;
        polling2();
        //      printf("poll %e %d \n",t, r_qtot);
    }
}


void performance()
{
    int warmup=100000;           // warm-up time (number of steps)
    int steady=10000000;         // simulation time (number of steps)
    int i,k;
    float load,share[20],rate[20],sum,sumvq;
    
    alpha=.9;
    beta=1.1;
    
    printf("N %d\n",N);
    packetsize[0]=5;
    packetsize[1]=3;
    packetsize[2]=1;
    share[0]=10./18;  // share of server load (sum=1)
    share[1]=6./18;
    share[2]=2./18;
    
    overhead=.5; // cost of dropped packet
    
    sumvq=0;
    for (i=0;i<N;i++) {
        sumvq+=1/packetsize[i];
    }
    buffer=640;
    
    min_thresh=1;
    
    max_thresh=5*buffer/sumvq;
    
    printf("VQthresh min %4.2f, max %4.2f\n", min_thresh, max_thresh);
    
    
    printf("overhead %4.2f, packetsize[0] %4.2f, packetsize[1] %4.2f, packetsize[2] %4.2f \nshare[0] %4.3f, share[1] %4.3f, share[2] %4.3f \n",
           overhead,packetsize[0], packetsize[1], packetsize[2], share[0], share[1], share[2]);
    //    printf("load %4.2f \n",load);
    printf("                        flow 1                  flow 2                 flow 3       \n");
    printf(" buffer  load   load th'pt  pt   pv    load th'pt  pt   pv    load th'pt  pt   pv   \n");
    
    
    
    for (load=0.1;load<6;load+=.1) {
        VQthresh=.5*buffer/sumvq;  // initial value
        vector=256;
        // compute packet rates
        sumrate=0;
        for (i=0;i<N;i++) {
            rate[i]=load/packetsize[i]*share[i];   // packet arrival rates
            sumrate+=rate[i];
        }
        for (i=0;i<N;i++) {
            dist[i]=rate[i]/sumrate;
        }
        
        for (k=0;k<400;k++) bufferhist[k]=0;
        for (k=0;k<vector+1;k++) bursthist[k]=0;
        for (k=0;k<vector+1;k++) servedhist[k]=0;
        numpolls=0;
        
        
        pollevent=1;
        at=0;
        initqueue();
        
        for (k=0;k<warmup;k++)
            step();
        
        initmetric();
        for (k=0;k<steady;k++) {
            step();
        }
        printf("%6d   %4.2f   ",buffer,load);
        sum=0;
        for (i=0;i<3;i++) {
            printf("%4.2f %4.3f %4.2f %4.2f   ",load*share[i],st[i]*packetsize[i]/(t-tstart),btt[i]/(st[i]+btt[i]+btv[i]), btv[i]/(st[i]+btv[i]));
        }
        printf("\n");
    }
}



int main()
{
    // srandom(time(NULL));  // Init the random generator on local time
    srandom(1);
    // test();
    performance();
}
