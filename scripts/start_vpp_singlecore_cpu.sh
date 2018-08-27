MAIN=$VPP_CPUCORE_MAIN
WORKER1=$VPP_CPUCORE_WORKER_1
DPDK1=$LC1P0
DPDK2=$LC1P1
sudo killall vpp_main
cd $VPP_ROOT
sudo make build-release
sudo -E $SCRIPTS/start_vpp_l3_singlecore_cpu.sh $MAIN $WORKER1 $DPDK1 $DPDK2 &
sleep 25
sudo -E  $SCRIPTS/ctl_singlecore_cpu.sh

if [[ $MAIN -le 11 || ($MAIN -gt 23 && $MAIN -le 35) ]];then
MAIN_NUMA=0
else
MAIN_NUMA=1
fi
if [[ $WORKER1 -le 11 || ($WORKER1 -gt 23 && $WORKER1 -le 35) ]];then
WORKER_NUMA=0
else
WORKER_NUMA=1
fi
if [[ $(echo $DPDK1 | awk -F ":" '{print $2}') -eq 84 ]];then
DPDK1_NUMA=1
else
DPDK1_NUMA=0
fi
if [[ $(echo $DPDK1 | awk -F ":" '{print $2}') -eq 84 ]];then
DPDK2_NUMA=1
else
DPDK2_NUMA=0
fi
echo -e "VPP Configuration:\nMain-Core on Core: $MAIN Numa: $MAIN_NUMA\nWorker on core: $WORKER Numa: $WORKER_NUMA\nDPDK Interface-RX $DPDK1 on Numa: $DPDK1_NUMA\nDPDK Interface-TX $DPDK2 on Numa $DPDK2_NUMA\n"
#	sleep 0.01
#	sudo killall vpp_main
#sudo $SFLAG $BINS/vppctl -p vpp3
#sudo vppctl -p vpp1
