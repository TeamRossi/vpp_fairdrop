#Note: This script can be used only after setting up dpdk environment and the interfaces are down.
#Binds the interfaces LC0P0 LC0P1 LC1P0 LC1P1 to dpdk driver specified by igb_uio

sudo $RTE_SDK/usertools/dpdk-devbind.py -b igb_uio  $DPDK_LC0P0 $DPDK_LC0P1 $DPDK_LC1P0 $DPDK_LC1P1
sudo $RTE_SDK/usertools/dpdk-devbind.py --status
