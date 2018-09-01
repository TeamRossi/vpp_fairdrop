This program simulates bandwidth sharing with fair drop. Flows have a fixed rate. The value of this rate is not entirely arbitrary, We found the need to have a high rate at low loads so that one flow alone can saturate the link. On the other hand, a high rate when there are many flows in progress (eg, at high loads) leads to some spurious performance effects. For the presented results we used a rate of 1.5 (1 saturates the link) at lower loads decreasing to 1 for loads greater than .5. Hard for reproducibility !

Program parameters are set in the code:
```
flowsize - constant flow size in packets
theta - allows virtual dequeues at rate theta times less than link rate to reduce latency (not used in paper)
load - link load, flow arrival rate x average flow size
flowshare[] - gives share of load due to flows of given class; current data for two identical classes; residual load due to singleton flows
VQthresh - common fixed threshold on virtual queues
VPthresh - threshold on virtual queue occupancy for deciding if packets get priority (not used in paper, inactive when set <0)
clrate[] - packet rate of flows (rate of 1 saturates link)
vector - max batch size, set to 1 for paper results
vectortime - time out for batch, batch formed if less than vector packets arrive before vectortime since last batch
NB: results not very good with batch>1 since throughput often lost waiting for batch formation
warmup, sumflows - used to limit warm up perion and run length (limits in code 1000 and 20000)
```
```
flowsize 3000.00, theta 1.00, VQthresh 10.00 VPthresh 0.00 
shares: class1 0.40, class2 0.40, class3 0.00, class4 0.00, class5 0.00, class6 0.00, single packets 0.20, total 
rate: class1 1.50, class2 1.50, class3 0.00, class4 0.00, class5 0.00, class6 0.00 
batching: max number 1, max time   10.0 

1000  	 0.426 	 0.448 	  0.72 	 19.38 	  0.81
2000  	 0.410 	 0.446 	  0.73 	 20.18 	  0.81
3000  	 0.457 	 0.452 	  0.69 	 18.01 	  0.82
4000  	 0.403 	 0.453 	  0.73 	 20.47 	  0.82
5000  	 0.403 	 0.413 	  0.73 	 20.81 	  0.79
6000  	 0.456 	 0.450 	  0.69 	 18.08 	  0.81
7000  	 0.453 	 0.441 	  0.70 	 18.26 	  0.81
8000  	 0.547 	 0.487 	  0.63 	 14.66 	  0.84
9000  	 0.439 	 0.434 	  0.71 	 18.92 	  0.80
10000  	 0.453 	 0.445 	  0.70 	 18.20 	  0.81
11000  	 0.446 	 0.453 	  0.70 	 18.44 	  0.82
12000  	 0.418 	 0.449 	  0.72 	 19.78 	  0.81
13000  	 0.483 	 0.471 	  0.68 	 16.85 	  0.83
14000  	 0.425 	 0.457 	  0.72 	 19.34 	  0.82
15000  	 0.417 	 0.449 	  0.72 	 19.82 	  0.81
16000  	 0.467 	 0.461 	  0.69 	 17.52 	  0.82
17000  	 0.493 	 0.457 	  0.67 	 16.76 	  0.82
18000  	 0.447 	 0.458 	  0.70 	 18.34 	  0.82
19000  	 0.467 	 0.449 	  0.69 	 17.45 	  0.81
20000  	 0.452 	 0.433 	  0.70 	 18.35 	  0.81

load, per class throughput and drop rate; 90, 99 and 99.9 percentiles  of queue length, priority queue, activelist 
0.55  cl0 th'put  0.445  0.702  cl1 th'put  0.446  0.702    18.474   31    64    116    0.815    0      1      2       1.76     3     8     13 
class   thruput   drop rate 
  0     0.445     0.702
  1     0.446     0.702
```

Graphical results from gnuplot script gnu-bwshare.txt using manually created data file bw-greedyFD-plot.txt. This is a list of lines like 
```
0.55  cl0 th'put  0.445  0.702  cl1 th'put  0.446  0.702    18.474   31    64    116    0.815    0      1      2       1.76     3     8     13
```
taken from each run for a given load. The program has a for loop for running a set of loads (with the same clrate parameters).
