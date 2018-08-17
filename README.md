# Fairdrop VPP

This repository contains all the scripts required to reproduce the experimental section in the paper "V.Addanki, L.Linguaglossa, J.Roberts, D.Rossi Controlling software router resources by fair packet dropping" presented in IFIP networking 2018. The scripts can also reproduce the experimental section in the extended journal submission of the paper in OTCS. VPP stands for "Vector Packet Processing", a novel packet processing application which is part linux foundation project FD.io. We use VPP as the software routing application in our experiments.

## Test-bed

A test-bed similar to the figure below is required to run the experiments. An exact same machine will generate plots same as shown in the paper. But, it is not necessary to have the exact same machine to verify the validity of the algorithm. Using the scripts in this repository, any machine with similar setup will be able to generate similar results (similar but not exact) as presented in the paper.
```
TGS - Traffic generator and Sink
DUT - Device Under Test
```
![alt text](https://raw.githubusercontent.com/vamsiDT/fairdrop-results/master/plots/testbed.png)

### Minimum Requirements
```
4x DPDK compatible Network interfaces (Physical or Virtual)
12x cpu cores for packet generator, Flow monitor and Packet processing application.
24 Gb Hugepage memory (This is not a strict requirement. The scripts can be modified to adjust the hugepage requirements for the applications but at a compromise for performance.
```
### Our Testbed

#### *Cisco UCS Rack Mount Server*
```
CPU
Model name: Intel(R) Xeon(R) CPU E5-2690 v3 @ 2.60GHz
Sockets: 2
Numa nodes: 2
cores per socket: 12
threads per core: 2

```
```
Network Interfaces
2x Intel X520 NICs, dual prort, 10Gbps, full duplex, directly connected with SFP+ cables.
```
# Getting Started
