This program simulates cpu sharing with responsive flows. The responsiveness is realized by setting the input rate to "pktload" times the current fair rate (in cycles/s for fair drop and packet/s for tail drop).

Choose tail drop by setting "taildrop" to 1 (otherwise the program simulates fair drop)

All parameters are currently set in the program, including:
```
alpha, beta - adaptive threshold MIMD parameters
flowsize - constant size of flow in packets
buffer - buffer size
min_thresh, max_thresh - limits on VQ threshold, set high for tail drop
pktsz[] - size of packets for each class (up to 6 classes possible, only ever used 2) (cost in cycles)
overhead - cost of dropping a packet
clrate[] - initial rate of a new flow of given class (adjusted after next change in number of flows)
pktload - degree of responsiveness (eg 1.1 means flows emit 10% higher than fair rate)
singlesize - cost in cycles of singleton packets
vector - max batch size
load - overall load = flow arrival rate x size in packet x cycles/pkt summed over classes and singleton flows
```
NB: program currently has a loop over load values but may bug if there are too many cases.

An example run for load .55 gives the following output
```
FAIR DROP: VQthresh min 10.24, max 51200.00, 
MIMD adaptive rate parameters, alpha=0.50, beta=1.20 
Responsive flow rates set to 1.10 times fair rate
flowsize 30000.00, buffer 512, max batch 256 
load shares: class1 0.40, class2 0.40, class3 0.00, class4 0.00, class5 0.00, class6 0.00, single packets 0.20 
pktsize: class1 1.00, class2 10.00, class3 0.00, class4 0.00, class5 0.00, class6 0.00, single packets 1.00 o'head 0.50
initial rate: class1 1.50, class2 1.50, class3 0.00, class4 0.00, class5 0.00, class6 0.00 

1000   0.390    0.003    0.153    0.374   0.004   0.108    0.408    
2000   0.391    0.003    0.153    0.381   0.005   0.107    0.400    
3000   0.432    0.003    0.154    0.389   0.005   0.106    0.420    
4000   0.419    0.003    0.154    0.393   0.005   0.106    0.423    
5000   0.387    0.003    0.152    0.387   0.005   0.107    0.419    

load, per class cycle/s and packet/s throughput, mean queue, activelist 90, 99 and 99.9 percentiles 
0.55  cl0  0.388  0.388  0.155  0.003  0.152  cl1  0.419  0.042  0.111  0.005  0.107  153.665  quantiles:     90     143     153 
class   cycles/s   pkts/s drop rate tail drop  fair drop
  0     0.388     0.388   0.155     0.003     0.152
  1     0.419     0.042   0.111     0.005     0.107
Poisson loss rate 0.001 
```
This run was for 5000 flows only. The results in the paper are for 20000 flows per load value.

Graphical results are given by the gnuplot script. It requires data files TD10o5-plot-new.txt and FD10o5-plot-new.txt that were created manually by copying the line of output begining with the load for the set of loads from .05 to .95, ie lines like 
```
0.55  cl0  0.388  0.388  0.155  0.003  0.152  cl1  0.419  0.042  0.111  0.005  0.107  153.665  quantiles:     90     143     153 
```


