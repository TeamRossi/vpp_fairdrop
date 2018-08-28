//
//  main.c
//  cpudyn
//
//  Created by jroberts on 02/08/2018.
//  Copyright © 2018 jroberts. All rights reserved.
//
// edited from project cpu-dyn-ohead2
//


#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define N 2000    // tables sized for max number of flow IDs, N

// Global variables
double t, t0=0;    // time
double t_last=0;    // time of last arrival rounded up for complete virtual scheduler cycle
double tstart;  // start time for metrics
int r_qtot, buffer;       // total queue sizes, real queue capacity
int r_q[N];       // queue sizes
double v_q[N];   // real (fluid) virtual queue sizes
double packettime;  // used for timestamp of last dequeued packet
double pollevent=1;  // epoch of next polling instant
double packetsize[N];
double pktsz[6], singlesize;  // packet size for flow class
double clrate[6];  // Poisson rate of each class flow
float VQthresh, min_thresh, max_thresh;
float alpha=0.5, beta=1.2; // MIMD parameters for adaptive threshold
int taildrop;
double r_qt,v_qt;   // cumulative queue sizes
double btt[N],btv[N],st[N];   // number of blocked arrivals
double at[N];      // arrival time
//double ai[N], md[N], ai_cl[6], md_cl[6], max_cl[6];  // additive increase and multiplicative decrease
double rate[N], maxrate[N];
double flowstart[N];
int class[N]; // class of flow
int full, spare, flowend[N], actflows, actcost;
double r_dt;      // departure time
double flowtime[6]={0,0,0,0,0,0}; // arrival time of next flow for each of 6 classes
double poissontime=0;  // time of next single packet flow
double poissonrate;
double pktload; // desired overall packet rate realized by transport; assume same rate for all active flows
double flowrate[6]; // flow arrival rate per class
double flowsize; // mean flow size in packets
//double tmark[N],rtt[N],rtt_cl[6];
//int markflag[N];
double sumtime=0, vsumtime=0, sumsize=0, sumlost=0;
double sumcltd[6],sumclfd[6];
double sumclsize[6]={0,0,0,0,0,0}, sumcltime[6]={0,0,0,0,0,0}, sumclcycles[6]={0,0,0,0,0,0};
double sumcllost[6]={0,0,0,0,0,0};
int sumflows=0, sumpoisson=0, sumpackets=0, sumsingle=0, lostsingle=0;
int vector,burst=0,flowid[100];
int pending[N];
int delayhist[2000], flowshist[1000], nblsum=0, sampcount=0;
int bursthist[500], numpolls=0;
int servedhist[500], numserved=0;
int nbl=0;  // number of backlogged flows
double increment[N];
double overhead;


struct r_Queue  // real fifo queue
{
    int r_packet;
    double timestamp;
    struct r_Queue* r_next;
}  *r_head, *r_tail;

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
    for (i=0;i<N;i++)
    {
        v_q[i]=0;
        r_q[i]=0;
        pending[i]=0;
        increment[i]=packetsize[i];
    }
    for (i=0;i<N;i++) {
        at[i]=1e20;
        flowend[i]=0;
    }
    //    r_dt=1;
    r_head=NULL;
    headflow=NULL;
    headfull=NULL;
    nbl=0;
    //    swg=0;
    //    for (i=0;i<Ng;i++) sw[i]=0;
    headfree=NULL;
    for (i=0;i<N;i++) freein(i); // place all flow IDs in freeflows
    spare=N;
    full=0;
    actflows=0;
    actcost=0;
}


void printqueue()
{
    int i;
    for (i=0;i<N;i++) printf(" %d: %e",i+1,v_q[i]);
    printf("\n");
}

void vstate8()   // update state for unweighted fairness, using activelist
{
    int i,j,k;
    double served, credit=0;   // amount of per flow backlog served, credit to be redistributed from flows that empty prematurely
    int oldnbl=nbl+1, flows;

    credit=(packettime-t_last);  // service capacity to be shared
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
                
                if (flowend[j]>0 && r_q[j]==0)  {  // following code to compute statistics and replace flow id in full list
                    
                    //            printf("%d %d %d \n",j,flowend[j],spare);
                    if (flowend[j]==1) {  // end of full rate flow
                        sumsize+=st[j]*packetsize[j];
                        sumclsize[class[j]]+=st[j]*packetsize[j];
                        sumcltd[class[j]]+=btt[j]*packetsize[j];
                        sumclfd[class[j]]+=btv[j]*packetsize[j];
                        sumlost+=(btt[j]+btv[j])*packetsize[j];
                        sumcllost[class[j]]+=(btt[j]+btv[j])*packetsize[j];
                        sumflows++;
                    }
                    else sumpoisson+=singlesize;
                    st[j]=0;
                    btt[j]=0;
                    btv[j]=0;
                    
                    if (flowend[j]==1) {
                        for (i=0;i<full;i++) { // remove j from fullflow list
                            k=fullout();
                            if (k==j) full--;
                            else {
                                if (actflows>0) {
                                    if (taildrop==1) rate[k]=pktload*(1-poissonrate*singlesize)/actcost; //  equal packet rates
                                    else rate[k]=pktload*(1-poissonrate*singlesize)/actflows/packetsize[k]; // fiar drop, equal cycle rates
                                }
                                fullin(k);
                            }
                        }
                    }
                    
                    freein(j); // add j to pool of free IDs
                    flowend[j]=0;
                    spare++;
                    // ongoing flow composition has changed, recompute rates so that each flow has rate = pktload * fair cpu rate / packetsize
                    // following code to adjust rates based on set of active flows
                    
                    // if (full!=actflows) printf("full %d actflows %d \n",full,actflows);
                    
                    for (j=0;j<full;j++) {
                        k=fullout();
                        if (actflows>0) {
                            if (taildrop==1) rate[k]=pktload*(1-poissonrate*singlesize)/actcost; //  equal packet rates
                            else rate[k]=pktload*(1-poissonrate*singlesize)/actflows/packetsize[k]; // fiar drop, equal cycle rates
                        }
                        fullin(k);
                    }
                    
                }
                
            }
        }
    }
    t_last=packettime;
}



void arrival(int i)  // packet arrival from full rate flow i ; version with fixed rate or using pktload
{
    if (r_qtot<buffer) {  // else packet is tail dropped
        
        r_enqueue(i);
        sumpackets++;
        r_qtot++;
        r_q[i]++;
    }
    else {  // packet is dropped
        btt[i]+=1;
    }
    at[i]=t+exponential()/rate[i];
}


void polling2()  // vpp polling: take vector of packets from r_queue, update virtual queues, drop, add packets to vq's
// version with one vstate update per vector, using timestamp of first packet
{
    int i,k,endflows=0,ended[256];
    pollevent=t;
    burst=0;
    numserved=0;
    
    if (r_qtot>0) {  // minimum polled vector - to avoid quasi-stationary state where vector is very small while VQs are close to threshold - threshold attains max but high drop rate due to overload,
        burst=1;
        i=r_dequeue();  // sets 'packettime' to time stamp of this packet - used in vstate to update vq's
        
        vstate8();
        
        r_qtot--;
        r_q[i]--;
        // drop or increment vq for head packet
        if (v_q[i]>VQthresh)  {  // packet dropped, increment polling interval by overhead
            //   printf("virtual drop %d %f %d \n",i,v_q[i],r_qtot);
            btv[i]+=1;
            pollevent+=overhead;
        }
        else {  // add packet to VQ and increment polling interval by packet size
            numserved++;
            st[i]+=1;
            if (v_q[i]==0) {
                nbl++;
                flowin(i);
            }
            
            v_q[i]+=packetsize[i];  // uses  increment to accept or drop later packets,
            pollevent+=packetsize[i];
            
            //         printf("%d %4.1f %4.1f \n",i,tempvq[i],flowwght[i]);
            if (st[i]>=flowsize) {  // this flow has ended, st[i] may later exceed flow size due to waiting packets
                flowend[i]=1;  // flag, used when last packet departs real queue
                at[i]=1e20;  // next arrival 'never'
            }
        }
        if (flowend[i]==1 && r_q[i]==0) {  // last packet polled or dropped, remember this flow and count time at end of vector processing
            ended[endflows]=i;
            endflows++;
        }
        
        
        // now drop or add packets to VQs for remaining packets
        while (r_qtot>0 && burst<vector) {
            burst++;
            i=r_dequeue();  // next packet in vector
            r_qtot--;
            r_q[i]--;
            
            if (v_q[i]>VQthresh)  {  // packet dropped
                btv[i]+=1;
                pollevent+=overhead;
            }
            else {  // add packet to VQ and increment polling interval by packet size
                numserved++;
                st[i]+=1;
                if (v_q[i]==0) {  // vq was and still is empty
                    nbl++;
                    flowin(i);
                }
                v_q[i]+=packetsize[i];  // uses  increment to accept or drop later packets,
                pollevent+=packetsize[i];
                
                if (st[i]>=flowsize) {  // this flow has ended, st[i] may later exceed flow size due to waiting packets
                    flowend[i]=1;  // flag, used when last packet departs real queue
                    at[i]=1e20;  // next arrival 'never'
                }
            }
            if (flowend[i]==1 && r_q[i]==0) {  // last packet polled or dropped, remember this flow and count time at end of vector processing
                ended[endflows]=i;
                endflows++;
            }
        }
    }
    
    
    for (k=0;k<endflows;k++)  {
        i=ended[k];
        sumtime+=pollevent-flowstart[i];  // sojourn time of full flows, pollevent is end of processing time for current vector
        sumcltime[class[i]]+=pollevent-flowstart[i];
        actflows--;   // reduce count of active full flows
        actcost-=packetsize[i];
    }
    
    if (burst==vector && VQthresh>min_thresh) VQthresh=VQthresh*alpha; if (VQthresh<min_thresh) VQthresh=min_thresh;
    else {if (burst<vector && VQthresh<max_thresh) VQthresh=VQthresh*beta; if (VQthresh>max_thresh) VQthresh=max_thresh;}
    
    bursthist[burst]++;
    servedhist[numserved]++;
    numpolls++;
    
    if (pollevent==t) pollevent+=1;  // if no packets served or none in queue, try 1 time unit later
    
}

void poissonarrival()
{
    int i;
    
    flowshist[nbl]++;  // sample number of backlogged flows before update (ie, after last update)
    nblsum+=nbl;
    sampcount++;
    
    if (r_qtot<buffer) {
        if (spare==0) {
            printf("anomaly: not enough flow IDs (single)\n");
            exit(0);
        }
        i=freeout();  // identity of new single packet flow
        spare--;
        
        //printf("poi out %d %d \n", i, spare);
        
        r_enqueue(i);
        packetsize[i]=singlesize;  // processing requirement for this packet
        sumpackets++;
        sumsingle++;
        if (r_qtot<1000) delayhist[r_qtot]++;  // histogram of queue length on single packet flow arrival
        r_q[i]=1;
        r_qtot++;
        
        // flow ends immediately
        flowend[i]=2;  // flag, used when last packet departs real queue, =2 for single packet flow
        at[i]=1e20;  // next arrival 'never'
    }
    else {
        lostsingle++;
    }
}

void flowarriv(int cl)  // cl is class of new flow
{
    int newflow,j,k;
    
    flowshist[nbl]++;  // sample number of backlogged flows before update (ie, after last update)
    nblsum+=nbl;
    sampcount++;
    
    
    newflow=freeout();
    spare--;
    
    //    printf("flow arrives %d %d \n", newflow, spare);
    
    fullin(newflow);
    full++;
    actflows++;
    actcost+=pktsz[cl];
    flowstart[newflow]=t;
    flowend[newflow]=0; // flag
    class[newflow]=cl;
    st[newflow]=0;
    btt[newflow]=0;
    btv[newflow]=0;
    v_q[newflow]=0;
    r_q[newflow]=0;
    packetsize[newflow]=pktsz[cl];  // processing requirement per packet of class i
    increment[newflow]=pktsz[cl];
    
    // following code to adjust rates based on set of active flows
    for (j=0;j<full;j++) {
        k=fullout();
        if (actflows>0) {
            if (taildrop==1) rate[k]=pktload*(1-poissonrate*singlesize)/actcost; //  equal packet rates
            else rate[k]=pktload*(1-poissonrate*singlesize)/actflows/packetsize[k]; // fair drop, equal cycle rates
        }
        fullin(k);
    }
    
    rate[newflow]=clrate[cl];
    arrival(newflow);  // first packet arrival
    flowtime[cl]+=exponential()/flowrate[cl];
}

void update(double delta)
{
    r_qt+=delta*r_qtot;
}

void step()
{
    int i, flow, f, cl;
    double arrevent, flowevent;
    
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
    for (i=0;i<2;i++) {
        if (flowevent>flowtime[i]) {
            flowevent=flowtime[i];
            cl=i;
        }
    }
    
    
    if (flowevent<arrevent && (flowevent<pollevent) && flowevent<poissontime) { //  new full rate flow arrival
        update(flowevent-t);
        if (spare==0) {
            printf("anomaly: not enough flow IDs (full)\n");
            exit(0);
        }
        t=flowevent;
        flowarriv(cl);
    }
    else if ((arrevent<pollevent) && arrevent<poissontime) { // new packet from full rate flow
        update(arrevent-t);
        t=arrevent;
        arrival(flow);
    }
    else if (pollevent<poissontime) { // vpp poll
        update(pollevent-t);
        t=pollevent;
        polling2();
    }
    else {  // new single packet flow
        update(poissontime-t);
        t=poissontime;
        poissonarrival();
        poissontime+=exponential()/poissonrate;
    }
}


void performance()
{
    int warmup=0;           // warm-up time (number of jumps)
    int i,k,sum,lastflows,lastpoisson,lastlost,lastpackets,lastsingle;
    int cl;
    double quantile, lastepoch,lastrqt,lastsize,lasttime;
    double flowshare[6]={.4,.4,0,0,0,0};  // traffic share of each full rate class (up to 6 classes possible
    float load;
    flowsize=30000;
    buffer=512;
    
    taildrop=0;
    if (taildrop==1) {
        min_thresh=100*buffer;
        max_thresh=100*buffer;
        printf("TAIL DROP: VQthresh min %4.2f, max %4.2f, \n", min_thresh, max_thresh);
    }
    else { // fair drop
        min_thresh=.02*buffer;
        max_thresh=100*buffer;
        printf("FAIR DROP: VQthresh min %4.2f, max %4.2f, \n", min_thresh, max_thresh);
        printf("MIMD adaptive rate parameters, alpha=%4.2f, beta=%4.2f \n",alpha,beta);
    }
    
    pktsz[0]=1;
    pktsz[1]=10;
    pktsz[2]=0;
    pktsz[3]=0;
    pktsz[4]=0;
    pktsz[5]=0;
    
    overhead=.5;
    
    
    VQthresh=buffer;
    
    
    clrate[0]=1.5;
    clrate[1]=1.5;
    clrate[2]=0;
    clrate[3]=0;
    clrate[4]=0;
    clrate[5]=0;
    
    
    pktload=1.1;
    
    
    singlesize=1;
    
    printf("Responsive flow rates set to %4.2f times fair rate\n",pktload);
    
    for (cl=0;cl<2;cl++) if (flowrate[cl]==0) flowtime[cl]=1e20;
    
    vector=256;  // max number of packets in batch
    
    printf("flowsize %4.2f, buffer %d, max batch %d \n", flowsize,  buffer, vector);
    printf("load shares: class1 %4.2f, class2 %4.2f, class3 %4.2f, class4 %4.2f, class5 %4.2f, class6 %4.2f, single packets %4.2f \n",flowshare[0], flowshare[1], flowshare[2], flowshare[3], flowshare[4], flowshare[5], (1-flowshare[0]-flowshare[1]-flowshare[2]-flowshare[3]-flowshare[4]-flowshare[5]));
    printf("pktsize: class1 %4.2f, class2 %4.2f, class3 %4.2f, class4 %4.2f, class5 %4.2f, class6 %4.2f, single packets %4.2f o'head %4.2f\n",pktsz[0], pktsz[1], pktsz[2], pktsz[3], pktsz[4], pktsz[5], singlesize,overhead);
    printf("initial rate: class1 %4.2f, class2 %4.2f, class3 %4.2f, class4 %4.2f, class5 %4.2f, class6 %4.2f \n",clrate[0], clrate[1], clrate[2], clrate[3], clrate[4], clrate[5]);
    printf("\n");
    
    for (load=.55;load<.99;load+=.5) {
        for (cl=0;cl<2;cl++)
            flowrate[cl]=load*flowshare[cl]/(flowsize*pktsz[cl]);   // arrival rate of full rate flows
        poissonrate=load*(1-flowshare[0]-flowshare[1]-flowshare[2]-flowshare[3]-flowshare[4]-flowshare[5])/singlesize;  // arrival rate of single packet flows
        if (poissonrate<1e-5) poissonrate=0;
        
        t=0;
        sumflows=0;
        for (cl=0;cl<2;cl++) if (flowrate[cl]>0) flowtime[cl]=cl+1; //exponential()/flowrate[cl];
        pollevent=1;
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
        sumlost=0;
        sumpackets=0;
        sumsingle=0;
        lostsingle=0;
        for (i=0;i<2;i++) {
            sumclsize[i]=0;
            sumcltime[i]=0;
            sumcllost[i]=0;
            sumcltd[i]=0;
            sumclfd[i]=0;
        }
        r_qt=0.;
        v_qt=0.;
        t0=t;
        for (k=0;k<1000;k++) delayhist[k]=0;
        lastflows=0;
        for (k=0;k<500;k++) flowshist[k]=0;
        nblsum=0;
        sampcount=0;
        
        for (k=0;k<200;k++) bursthist[k]=0;
        for (k=0;k<200;k++) servedhist[k]=0;
        numpolls=0;
        
        lastflows=0;
        lastsize=0;
        lasttime=0;
        lastpoisson=0;
        lastlost=0;
        lastpackets=0;
        lastsingle=0;
        lastepoch=t;
        lastrqt=0;
        
        while (sumflows <= 5000) {
            step();
            if (sumflows>lastflows && sumflows==(sumflows/1000)*1000) { // intermediate printout
                printf("%d  %6.3f   %6.3f   %6.3f   %6.3f  %6.3f  %6.3f   %6.3f    \n", sumflows, (sumsize-lastsize)/(sumtime-lasttime),
                       sumcltd[0]/(sumclsize[0]+sumcllost[0]),sumclfd[0]/(sumclsize[0]+sumcllost[0]),sumclsize[0]/sumcltime[0],
                       sumcltd[1]/(sumclsize[1]+sumcllost[1]),sumclfd[1]/(sumclsize[1]+sumcllost[1]),sumclsize[1]/sumcltime[1]);
                
                lastflows=sumflows;
                lastsize=sumsize;
                lasttime=sumtime;
                lastpoisson=sumpoisson;
                lastlost=sumlost;
                lastpackets=sumpackets;
                lastsingle=sumsingle;
                lastepoch=t;
                lastrqt=r_qt;
            }
        }
        printf("\nload, per class cycle/s and packet/s throughput, mean queue, activelist 90, 99 and 99.9 percentiles \n");
        printf("%4.2f ",load);
        for (i=0;i<2;i++)
            if (sumcltime[i]>0) printf(" cl%d %6.3f %6.3f %6.3f %6.3f %6.3f ", i, sumclsize[i]/sumcltime[i], sumclsize[i]/pktsz[i]/sumcltime[i],
                                       sumcllost[i]/(sumclsize[i]+sumcllost[i]),
                                       sumcltd[i]/(sumclsize[i]+sumcllost[i]),sumclfd[i]/(sumclsize[i]+sumcllost[i]));
        
        printf(" %6.3f ", r_qt/sumpackets);
        
        // activelist quantiles
        //printf(" %4.2f ", 1.*nblsum/sampcount);
        printf(" quantiles: ");
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
        
        printf("\nclass   cycles/s   pkts/s drop rate tail drop  fair drop\n");
        
        for (i=0;i<2;i++)
            if (sumcltime[i]>0) printf("  %d    %6.3f    %6.3f  %6.3f    %6.3f    %6.3f\n", i, sumclsize[i]/sumcltime[i], sumclsize[i]/pktsz[i]/sumcltime[i],
                                       sumcllost[i]/(sumclsize[i]+sumcllost[i]),
                                       sumcltd[i]/(sumclsize[i]+sumcllost[i]),sumclfd[i]/(sumclsize[i]+sumcllost[i]));
        printf("Poisson loss rate %5.3f \n", 1.*lostsingle/sumsingle);
    }  // end of load loop
}


int main()
{
    //  srandom(time(NULL));  // Init the random generator on local time
    srandom(1);  // for reproducible results
    // test();
    performance();
}
