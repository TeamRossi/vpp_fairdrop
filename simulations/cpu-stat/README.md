This program is used to create Fig 3 showing cpu throughput for 3 static flows. Results for no packet dropping are derived with the same program with a very large fixed VQ threshold.
```
Parameters for the presented results are:
N - 3 flows
alpha, beta - MIMD VQthresh parameters set here to .9 and 1.1
packetsize[] - cycles cost of packet processing for flow (units don't matter, only relative values)
share[] - share of load for flow, set here so that packet arrival rates are the same
overhead - relative cost of dropped packets
buffer - input buffer size (not critical if big enough since no tail drops)
min_thresh, max_thresh - limits on VQthresh
```
Output with fair drop is in fairness.txt; output with tail drop is in fairnessNodrop.txt. The gnuplot script plots the figure from the paper (smaller granularity in range of loads than for paper figure explains slight differences)
