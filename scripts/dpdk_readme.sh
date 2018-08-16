echo "###############################################################################################"
echo ""
echo "Now please run Dpdk and Pktgen Installation. Please check ReadMe for instructions to install"
echo ""
echo "###############################################################################################"


# remove_igb_uio_module()
# {
#     echo "Unloading any existing DPDK UIO module"
#     /sbin/lsmod | grep -s igb_uio > /dev/null
#     if [ $? -eq 0 ] ; then
#         sudo /sbin/rmmod igb_uio   
#     fi
# }


# load_igb_uio_module()
# {
#     if [ ! -f $RTE_SDK/$RTE_TARGET/kmod/igb_uio.ko ];then
#         echo "## ERROR: Target does not have the DPDK UIO Kernel Module."
#         echo "       To fix, please try to rebuild target."
#         return
#     fi

#     remove_igb_uio_module

#     /sbin/lsmod | grep -s uio > /dev/null
#     if [ $? -ne 0 ] ; then
#         modinfo uio > /dev/null
#         if [ $? -eq 0 ]; then
#             echo "Loading uio module"
#             sudo /sbin/modprobe uio
#         fi
#     fi

#     # UIO may be compiled into kernel, so it may not be an error if it can't
#     # be loaded.

#     echo "Loading DPDK UIO module"
#     sudo /sbin/insmod $RTE_SDK/$RTE_TARGET/kmod/igb_uio.ko
#     if [ $? -ne 0 ] ; then
#         echo "## ERROR: Could not load kmod/igb_uio.ko."
#         quit
#     fi
# }

