sudo $RTE_SDK/usertools/dpdk-devbind.py -b igb_uio  $DPDK_LC0P0 $DPDK_LC0P1 $DPDK_LC1P0 $DPDK_LC1P1
sudo $RTE_SDK/usertools/dpdk-devbind.py --status
