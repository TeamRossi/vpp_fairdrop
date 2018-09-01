Program to simulate bandwidth sharing with static set of flows emitting packets as constant rate Poisson process. Same program can simulate plain fair dropping and hierarchical weighted fair dropping (2 levels). The FD algorithm is coded as vstate7 for plain fairness, vstate6 for hierarchical weighted fairness.
 
Run data is set in the code:
N - number of flows
Ng - number of groups in hierarchy (not used for plain fairness)
buffer - real buffer capacity
VQthresh - threshold on virtual queues
theta - parameter alloowing more agressive dropping (service rate = theta) to limit latency
hierarchical - 0 for plain fairness, 1 for hierarchical weighted fairness
w[] - weight in group
gp[] - group to which flow belongs
wg[] - group weight 
fload[] - load offered by flow (relative to link capacity)
warmup, steady - length of simulation in number of simulated events (packet arrivals and departures)

For plain fairness, the same run gives results for FQ and FIFO. Throughputs are the same for both (by design). Latency is derived by Little's formula: for FIFO, latency is mean flow occumancy in real queue (rQ) / throughput; for FQ, latency is mean flow occupancy in virtual queue (vQ) / throughput.


Two runs are needed for the table output. For tail drop, we set VQthresh big enough to avoid drops: VQthresh >= buffer. For fair drop, the real buffer needs to be at least equal to the sum of VQthresh values: 3*VQthresh <= buffer. Program output:
```
tail drop -

buffer 30, VQthresh 30, theta 1.00 
Fair sharing
                flow 1                                 flow 2                                 flow 3
  load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    util 
  1.00  0.588  17.15 29.15 27.86 47.34   0.60  0.353  10.28 29.15  1.25  3.55   0.10  0.059   1.71 29.15  0.12  2.11   1.00
```
```
fair drop -

buffer 30, VQthresh 10, theta 1.00 
Fair sharing
                flow 1                                 flow 2                                 flow 3
  load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    load  th'pt    rQ   lat    vQ   lat    util 
  1.00  0.450   8.50 18.87  9.96 22.11   0.60  0.450   8.50 18.91  8.80 19.57   0.10  0.100   1.94 19.44  0.27  2.73    1.00
```

