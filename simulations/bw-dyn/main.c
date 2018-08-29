//
//  main.c
//  bw-dyn
//
//  Created by jroberts on 02/08/2018.
//  Copyright © 2018 jroberts. All rights reserved.
//


#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define N 2000    // number of flows, size of tables

// Parameters


double theta=1;  // theta (<=1) is virtual queue service rate when real serive rate is 1

// Variables

double t, t0=1e20;    // time
double t_last=0;    // time of last arrival rounded up for complete virtual scheduler cycle
double tstart;  // start time for metrics
int r_qtot, p_qtot;       // total queue sizes
int r_q[N], p_q[N];       // queue sizes
double v_q[N];   // real (fluid) virtual queue sizes
double VQthresh,VPthresh;
double r_qt,v_qt,p_qt;   // cumulative queue sizes
double bt[N],st[N];   // number of blocked arrivals
double at[N];      // arrival time

double rate[N], maxrate[N];
double flowstart[N];
int class[N]; // class of flow
int full, spare, flowend[N];
double r_dt;      // departure time
double flowtime[6]={0,0,0,0,0,0}; // arrival time of next flow for each of 6 classes
double clrate[6];  // Poisson rate of each class flow
double poissontime=0;  // time of next single packet flow
double poissonrate;
double flowrate[6]; // flow arrival rate
double flowsize; // mean flow size in packets
double sumtime=0, vsumtime=0, sumsize=0, sumdrop=0;
double sumclsize[6]={0,0,0,0,0,0}, sumcltime[6]={0,0,0,0,0,0};
double sumcldrop[6],vsumcltime[6]={0,0,0,0,0,0};
int sumflows=0, sumpoisson=0, sumpackets=0, sumsingle=0, sumpriority=0;
int vector,burst=0,flowid[100],burstpackets=0;
double vectortime;
int pending[N];
int delayhist[2000], flowshist[1000], priohist[100], sampcount=0, nblsum=0;;
int nbl=0;  // number of backlogged flows


struct r_Queue  // real fifo queue
{
    int r_packet;
    struct r_Queue* r_next;
}  *r_head, *r_tail;

struct p_Queue  // real fifo queue
{
    int p_packet;
    struct p_Queue* p_next;
}  *p_head, *p_tail;

struct activelist  // list of active flows (have packet in virtual queue)
{
    int activeflow;
    struct activelist* nextflow;
}  *headflow, *tailflow;

struct fullflows  // list of in-progress flow emitting at up to full rate, other flows are single packet
{
    int fullflow;
    struct fullflows* nextfull;
}  *headfull, *tailfull;

struct freeflows  // list of inactive flow IDs, available for new flows
{
    int freeflow;
    struct freeflows* nextfree;
}  *headfree, *tailfree;


// Random variables

double uniform()
{
    return((double) random()/RAND_MAX);
}

double exponential()
{
    return((double) -log(1.-uniform()));
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

int p_dequeue()
{
    struct p_Queue *temp;
    int i;
    temp=p_head;
    i=p_head->p_packet;
    p_head=p_head->p_next;
    free(temp);
    return i;
}

void p_enqueue(int i)
{
    struct p_Queue *temp;
    temp=(struct p_Queue *)malloc(sizeof(struct p_Queue));
    temp->p_packet=i;
    temp->p_next=NULL;
    if (p_head == NULL)
    {
        p_tail=temp;
        p_head=temp;
    }
    else
    {
        p_tail->p_next=temp;
        p_tail=temp;
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

int fullout()
{
    struct fullflows *temp;
    int i;
    temp=headfull;
    i=headfull->fullflow;
    headfull=headfull->nextfull;
    free(temp);
    return i;
}

void fullin(int i)
{
    struct fullflows *temp;
    temp=(struct fullflows *)malloc(sizeof(struct fullflows));
    temp->fullflow=i;
    temp->nextfull=NULL;
    if (headfull == NULL)
    {
        tailfull=temp;
        headfull=temp;
    }
    else
    {
        tailfull->nextfull=temp;
        tailfull=temp;
    }
}

int freeout()
{
    int i;
    struct freeflows *temp;
    temp=headfree;
    i=headfree->freeflow;
    headfree=headfree->nextfree;
    free(temp);
    return i;
}

void freein(int i)
{
    struct freeflows *temp;
    temp=(struct freeflows *)malloc(sizeof(struct freeflows));
    temp->freeflow=i;
    temp->nextfree=NULL;
    if (headfree == NULL)
    {
        tailfree=temp;
        headfree=temp;
    }
    else
    {
        tailfree->nextfree=temp;
        tailfree=temp;
    }
}

void r_printqueue()
{
    struct r_Queue *temp;
    int count=0;
    temp=r_head;
    printf("r_Queue state: ");
    while (temp!=NULL)
    {
        printf("%d ",temp->r_packet);
        temp=temp->r_next;
        count++;
    }
    printf("\n");
    printf("Queue length = %d %d \n",r_qtot, count);
}


// Initialization

void initqueue()
{
    int i;
    r_qtot=0;
    p_qtot=0;
    r_qt=0;
    p_qt=0;
    for (i=0;i<N;i++)
    {
        v_q[i]=0;
        r_q[i]=0;
        p_q[i]=0;
        pending[i]=0;
    }
    for (i=0;i<N;i++) {
        at[i]=1e20;
        flowend[i]=0;
    }
    r_dt=1;
    r_head=NULL;
    p_head=NULL;
    headflow=NULL;
    headfull=NULL;
    nbl=0;
    //    swg=0;
    //    for (i=0;i<Ng;i++) sw[i]=0;
    headfree=NULL;
    for (i=0;i<N;i++) freein(i); // place all flow IDs in freeflows
    spare=N;
    full=0;
}


void printqueue()
{
    int i;
    for (i=0;i<N;i++) printf(" %d: %e",i+1,v_q[i]);
    printf("\n");
}


void vstate7()   // update state for unweighted fairness, using activelist
{
    int i,j,k;
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
                nbl--;  // this flow is no longer in activelist;
                
                if (flowend[j]>0)  {  // following code to compute statistics and replace flow id in full list
                    if (flowend[j]==1) {
                        vsumtime+=t-flowstart[j];  // record virtual queue response time
                        vsumcltime[class[j]]+=t-flowstart[j];
                    }
                    if (r_q[j]+p_q[j]==0 && pending[j]==0) { // remove flow
                        if (flowend[j]==1) {  // end of full rate flow
                            sumsize+=st[j];
                            sumclsize[class[j]]+=st[j];
                            sumdrop+=bt[j];
                            sumcldrop[class[j]]+=bt[j];
                            sumflows++;
                            //                            printf("v-size  %f %f %d\n",  st[j], bt[j],j);
                        }
                        else sumpoisson++;
                        
                        //                        if (flowend[j]==1) printf("virt flow %d %6.1f %6.1f %d %d \n",j,st[j],bt[j], r_qtot,full);
                        //                        if (flowend[j]==2) printf("virt poi  %d %d %d \n",j,r_qtot,full);
                        
                        st[j]=0;
                        bt[j]=0;
                        for (i=0;i<full;i++) { // remove j from fullflow list
                            k=fullout();
                            if (k!=j) fullin(k);
                            else full--;  // this flow exits fullflow list
                        }
                        freein(j); // add j to pool of free IDs
                        flowend[j]=0;
                        spare++;
                    }
                }
            }
        }
    }
    t_last=t;
}


void arrival(int i)  // packet arrival from full rate flow i
{
    int j,k;
    
    // packets admitted to batch or dropped; drop when VQ>threshold
    
    burstpackets++;  // call vstate after vector arrivals, dropped or not
    
    if (v_q[i]>VQthresh)  {  // packet dropped, source notified after 1 rtt for first packet in rtt
        bt[i]++;
        at[i]=t+exponential()/rate[i]; // paced arrivals
    }
    else {  // packet not dropped; add to pending batch
        flowid[burst]=i;  //  flow i packet pending for virtual queue
        pending[i]=1;  // means flow i has packet pending before being added to virtual queue
        burst++;
        st[i]+=1.;
        //        rate[i]+=ai[i]; // update arrival rate (additive increase, per packet)
        //        rtt[i]=rtt_cl[class[i]]+r_qtot+p_qtot;
        if (st[i]==flowsize) { // this flow stops generating new packets (deterministic size)
            flowend[i]=1;  // flag, used when last packet departs real queue
            at[i]=1e20;  // next arrival 'never'
        }
        else {  // generate next packet; rate decreases if mark observed
            //        printf("rate %d %f %f \n", markflag[i],rate[i], t-tmark[i]);
            at[i]=t+exponential()/rate[i]; // paced arrivals
        }
    }
    if (burstpackets==vector || t-t_last > vectortime) {   //  run vstate to update virtual queues and add new packets
        vstate7();
        for (j=0; j<burst; j++) {
            k=flowid[j];
            if (v_q[k]==0) {
                nbl++;
                flowin(k);
            }
            v_q[k]+=1.;  // increment by one packet
            pending[k]=0;
            sumpackets++;
            if (v_q[k]<=VPthresh) {  // this packet goes to priority queue
                p_enqueue(k);
                sumpriority++;
                p_q[k]++;
                if (p_qtot+r_qtot==0) r_dt=t+1; // this was an arrival to empty queues, need to programme departure
                p_qtot++;
            }
            else {  // packet goes to normal queue
                r_enqueue(k);
                r_q[k]++;
                if (p_qtot+r_qtot==0) r_dt=t+1; // this was an arrival to empty queues, need to programme departure
                r_qtot++;
            }
        }
        
        //        printf("%d %d \n",burst, full);
        
        burst=0;
        burstpackets=0;
    }
}


void r_departure()  // departure from real queue (FIFO)
{
    int i,j,k;
    
    if (p_qtot+r_qtot==0) {
        printf("anomaly: p_qtot+r_qtot=%d)\n",p_qtot+r_qtot);
        exit(0);
    }
    else if (p_qtot>0) {  // dequeue from priority queue
        j=p_dequeue();
        if (p_q[j]<=0) {
            printf("anomaly p_q: %d %d \n",j, p_q[j]);
            exit(0);
        }
        p_q[j]--;
        p_qtot--;
    }
    else if (r_qtot>0) {  // dequeue from normal queue
        j=r_dequeue();
        if (r_q[j]<=0) {
            printf("anomaly r_q: %d %d \n",j, r_q[j]);
            exit(0);
        }
        r_q[j]--;
        r_qtot--;
    }
    
    if (r_q[j]+p_q[j]==0 && flowend[j]>0) { // flow has ended
        if (flowend[j]==1) {
            sumtime+=t-flowstart[j];  // sojourn time of full flows
            sumcltime[class[j]]+=t-flowstart[j];  // sojourn time of full flows
            //            printf("          time %d %f \n",j,t-flowstart[j]);
        }
        //            printf(" %d %d   %d   %f   %f  %f \n",full,nbl,j, st[j], t-flowstart[j], st[j]/(t-flowstart[j]));
        if (v_q[j]==0 && pending[j]==0) {  // flow ended in both queues
            if (flowend[j]==1) {  // end of full rate flow
                sumsize+=st[j];
                sumclsize[class[j]]+=st[j];
                sumdrop+=bt[j];
                sumcldrop[class[j]]+=bt[j];
                sumflows++;
                //                printf("r-size %d %f %f \n", j, st[j], bt[j]);
                
            }
            else sumpoisson++;
            st[j]=0;
            bt[j]=0;
            
            for (i=0;i<full;i++) { // remove j from fullflow list
                k=fullout();
                if (k!=j) fullin(k);
                else full--;
            }
            
            freein(j); // add j to pool of free IDs
            flowend[j]=0;
            spare++;
        }
    }
}

void poissonarrival()
{
    int i,j,k;
    
    flowshist[nbl]++;
    nblsum+=nbl;
    sampcount++;
    
    if (spare==0) {
        printf("anomaly: not enough flow IDs (single)\n");
        exit(0);
    }
    i=freeout();  // identity of new single packet flow
    spare--;
    
    sumsingle++;
    
    // sample queue lengths
    if (p_qtot<100) priohist[p_qtot]++;  // histogram of priority queue length on single packet flow arrival
    if (p_qtot+r_qtot<2000) delayhist[p_qtot+r_qtot]++;  // histogram of total queue length on single packet flow arrival
    
    // sample activelist
    
    // flow ends immediately
    flowend[i]=2;  // flag, used when last packet departs real queue, =2 for single packet flow
    at[i]=1e20;  // next arrival 'never'
    
    flowid[burst]=i;
    pending[i]=1;  // means flow i has packet pending before being added to virtual queue
    burst++;
    burstpackets++;
    
    if (burstpackets==vector || t-t_last > vectortime) {   //  run vstate to update virtual queues and add new packets
        vstate7();
        for (j=0; j<burst; j++) {
            k=flowid[j];
            if (v_q[k]==0) {
                nbl++;
                flowin(k);
            }
            sumpackets++;
            if (v_q[k]<=VPthresh) {  // this packet goes to priority queue
                p_enqueue(k);
                sumpriority++;
                p_q[k]++;
                if (p_qtot+r_qtot==0) r_dt=t+1; // this was an arrival to empty queues, need to programme departure
                p_qtot++;
            }
            else {  // packet goes to normal queue
                r_enqueue(k);
                r_q[k]++;
                if (p_qtot+r_qtot==0) r_dt=t+1; // this was an arrival to empty queues, need to programme departure
                r_qtot++;
            }
            v_q[k]+=1.;  // increment by one packet
            pending[k]=0;
        }
        burst=0;
        burstpackets=0;
    }
}

void flowarriv(int i)  // i is class of new flow
{
    int newflow;
    
    flowshist[nbl]++;
    nblsum+=nbl;
    sampcount++;
    
    newflow=freeout();
    spare--;
    
    //    printf("flow out %d %d \n", newflow, spare);
    
    fullin(newflow);
    full++;
    flowstart[newflow]=flowtime[i];
    flowend[newflow]=0; // flag
    class[newflow]=i;
    st[newflow]=0;
    bt[newflow]=0;
    v_q[newflow]=0;
    r_q[newflow]=0;
    maxrate[newflow]=1;
    rate[newflow]=clrate[i];
    arrival(newflow);  // first packet arrival
    flowtime[i]+=exponential()/flowrate[i];
}

void update(double delta)
{
    r_qt+=delta*r_qtot;
    p_qt+=delta*p_qtot;
}

void step()
{
    int i, flow, f, cl;
    double arrevent, flowevent;
    
    //    printf("%e %6.1f \n",t, r_qt);
    
    // find flow with next packet arrival
    arrevent=1e20; flow=-1;
    for (i=0;i<full;i++) {
        f=fullout();
        if (arrevent>at[f]) {
            arrevent=at[f];
            flow=f;
        }
        fullin(f);
    }
    
    // find class with next flow arrival
    flowevent=1e20; cl=-1;
    for (i=0;i<6;i++) {
        if (flowevent>flowtime[i]) {
            flowevent=flowtime[i];
            cl=i;
        }
    }
    
    if (flowevent<arrevent && (flowevent<r_dt || r_qtot+p_qtot==0) && flowevent<poissontime) { //  new full rate flow arrival
        update(flowevent-t);
        if (spare==0) {
            printf("anomaly: not enough flow IDs (full)\n");
            exit(0);
        }
        t=flowevent;
        flowarriv(cl);
    }
    else if ((arrevent<r_dt || r_qtot+p_qtot==0) && arrevent<poissontime) { // new packet from full rate flow
        update(arrevent-t);
        t=arrevent;
        arrival(flow);
    }
    else if (r_dt<poissontime && r_qtot+p_qtot>0) { // end of real packet transmission
        update(r_dt-t);
        t=r_dt;
        r_departure();
        r_dt=t+1;
    }
    else {  // new single packet flow
        update(poissontime-t);
        t=poissontime;
        poissonarrival();
        poissontime+=exponential()/poissonrate;
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


void performance()
{
    int warmup=0;           // warm-up time (number of jumps)
    int i,k,sum,lastflows,lastsize,lasttime,lastpoisson,lastdrop,lastpackets,lastsingle,lastpriority;
    int cl;
    double quantile, lastepoch, lasttqt, lastpqt;
    double flowshare[6]={.4,.4,0,0,0,0};  // traffic share of each full rate class
    float load=0;              // load

    flowsize=3000;
    VQthresh=10;
    VPthresh=-1;
    
    clrate[0]=1.5;   // unresponsive rates of flows of class; needs to be aggressive (eg, 1.5) for low loads to realize expected throughput (1-rho); same rate at high load leads to spurious phenomena (eg, needs rate <= 1)
    clrate[1]=1.5;
    clrate[2]=0;
    clrate[3]=0;
    clrate[4]=0;
    clrate[5]=0;
    
    
    
    vector=1;  // max number of packets in batch
    vectortime=10;  // max time before activating pending batch (packet times)
    
    printf("flowsize %4.2f, theta %4.2f, VQthresh %4.2f VPthresh %4.2f \n", flowsize, theta, VQthresh, VPthresh);
    printf("shares: class1 %4.2f, class2 %4.2f, class3 %4.2f, class4 %4.2f, class5 %4.2f, class6 %4.2f, single packets %4.2f, total \n",flowshare[0], flowshare[1], flowshare[2], flowshare[3], flowshare[4], flowshare[5],
           1-flowshare[0]-flowshare[1]-flowshare[2]-flowshare[3]-flowshare[4]-flowshare[5]);
    printf("rate: class1 %4.2f, class2 %4.2f, class3 %4.2f, class4 %4.2f, class5 %4.2f, class6 %4.2f \n",clrate[0], clrate[1], clrate[2], clrate[3], clrate[4], clrate[5]);
    printf("batching: max number %d, max time %6.1f \n\n", vector, vectortime);
    
    
    for (load=.55;load<.95;load+=.5) {
        for (cl=0;cl<6;cl++)
            flowrate[cl]=load*flowshare[cl]/(flowsize);   // arrival rate of full rate flows
        poissonrate=load*(1-flowshare[0]-flowshare[1]-flowshare[2]-flowshare[3]-flowshare[4]-flowshare[5]);  // arrival rate of single packet flows
        if (poissonrate<1e-5) poissonrate=0;
        t=0;
        sumflows=0;
        for (cl=0;cl<6;cl++) flowtime[cl]=exponential()/flowrate[cl];
        poissontime=exponential()/poissonrate;
        
        initqueue();
        
        warmup=0;
        
        while (warmup==0) {
            step();
            if (sumflows>1000) warmup=1;
        }
        
        sumtime=0;
        vsumtime=0;
        sumflows=0;
        sumpoisson=0;
        sumsize=0;
        sumdrop=0;
        sumpackets=0;
        sumsingle=0;
        sumpriority=0;
        for (i=0;i<6;i++) {
            sumclsize[i]=0;
            sumcltime[i]=0;
            vsumcltime[i]=0;
            sumcldrop[i]=0;
        }
        r_qt=0.;
        p_qt=0;
        v_qt=0.;
        t0=t;
        for (k=0;k<2000;k++) delayhist[k]=0;
        for (k=0;k<100;k++) priohist[k]=0;
        for (k=0;k<500;k++) flowshist[k]=0;
        sampcount=0;
        nblsum=0;
        
        lastflows=0;
        lastsize=0;
        lasttime=0;
        lastpoisson=0;
        lastdrop=0;
        lastpackets=0;
        lastsingle=0;
        lastepoch=t;
        lasttqt=0;
        lastpqt=0;
        lastpriority=0;
        
        while (sumflows <= 20000) {
            step();
            if (sumflows>lastflows && sumflows==(sumflows/1000)*1000) {
                printf("%d  \t%6.3f \t%6.3f \t%6.2f \t%6.2f \t%6.2f\n", sumflows, (sumsize-lastsize)/(sumtime-lasttime),
                       1-(sumsize-lastsize+sumpoisson-lastpoisson)/(t-lastepoch)/theta,
                       (sumdrop-lastdrop)/(sumsize-lastsize+sumdrop-lastdrop), (r_qt+p_qt-lasttqt)/(sumpackets-lastpackets),
                       (p_qt-lastpqt)/(sumpriority-lastpriority));
                lastflows=sumflows;
                lastsize=sumsize;
                lasttime=sumtime;
                lastpoisson=sumpoisson;
                lastdrop=sumdrop;
                lastpackets=sumpackets;
                lastsingle=sumsingle;
                lastepoch=t;
                lasttqt=r_qt+p_qt;
                lastpqt=p_qt;
                lastpriority=sumpriority;
            }
        }
        printf("\nload, per class throughput and drop rate; mean, 90, 99 and 99.9 percentiles of queue length, priority queue, activelist \n");
        printf("%4.2f ",load);
        for (i=0;i<6;i++)
            if (sumcltime[i]>0) printf(" cl%d th'put %6.3f %6.3f ", i,
                                       sumclsize[i]/sumcltime[i], sumcldrop[i]/(sumclsize[i]+sumcldrop[i]));
        // overall delay quantiles
        printf("   %6.3f",(r_qt+p_qt)/sumpackets);
        sum=0;
        k=0;
        quantile=.1;
        while (sum<sumsingle && k<2000) {
            sum+=delayhist[k];
            if (1-1.*sum/sumsingle < quantile) {
                if (quantile>=.001) printf("   %d ", k);
                quantile=quantile/10;
            }
            k++;
        }
        if (quantile>=.1) printf("  >2000");
        if (quantile>=.01) printf("  >2000");
        if (quantile>=.001) printf("  >2000");
        
        // priority queue quantiles
        printf("  %6.3f ", p_qt/sumpriority);
        sum=0;
        k=0;
        quantile=.1;
        while (sum<sumsingle && k<100) {
            sum+=priohist[k];
            if (1-1.*sum/sumsingle < quantile) {
                if (quantile>=.001) printf("   %d   ", k);
                quantile=quantile/10;
            }
            k++;
        }
        if (quantile>=.1) printf("  >100");
        if (quantile>=.01) printf("  >100");
        if (quantile>=.001) printf("  >100");
        
        // activelist quantiles
        printf("    %4.2f ", 1.*nblsum/sampcount);
        sum=0;
        k=0;
        quantile=.1;
        while (sum<sampcount && k<1000) {
            sum+=flowshist[k];
            if (1-1.*sum/sampcount < quantile) {
                if (quantile>=.001) printf("    %d ", k);
                quantile=quantile/10;
            }
            k++;
        }
        if (quantile>=.1) printf("  >1000");
        if (quantile>=.01) printf("  >1000");
        if (quantile>=.001) printf("  >1000");
        
        printf("\nclass   thruput   drop rate \n");
        
        for (i=0;i<6;i++)
            if (sumcltime[i]>0) printf("  %d    %6.3f    %6.3f\n", i, sumclsize[i]/sumcltime[i],
                                       sumcldrop[i]/(sumclsize[i]+sumcldrop[i]));
    }
}


int main()
{
   // srandom(time(NULL));  // Init the random generator on local time
    srandom(1);  // for reproducible results
    // test();
    performance();
}
