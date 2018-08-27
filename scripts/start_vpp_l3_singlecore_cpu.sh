sudo $VPP_ROOT/build-root/install-vpp-native/vpp/bin/vpp unix {nodaemon} cpu { main-core $1 corelist-workers $2} api-segment {prefix vpp2} dpdk { dev $3 {num-rx-queues 2 num-rx-desc 2048 } dev $4 {num-rx-queues 2 num-rx-desc 2048} socket-mem 8192,8192} plugin_path $VPP_ROOT/build-root/install-vpp-native/vpp/lib64/vpp_plugins
#corelist-workers 29,30
#sudo $SFLAG vppctl -p vpp1 set ip arp $NAMELC1P1 137.194.208.1 $MACLC0P1
#sudo $SFLAG vppctl -p vpp1 ip route add 0.0.0.0/0 via 137.194.208.1
#sudo $SFLAG vppctl -p vpp1 set interface promiscuous on $NAMELC1P1
#sudo $SFLAG vppctl -p vpp1 set interface promiscuous on $NAMELC1P0
#sudo $SFLAG vppctl -p vpp1 set int ip addr $NAMELC1P1 192.168.2.2/24
#sudo $SFLAG vppctl -p vpp1 set int ip addr $NAMELC1P0 192.168.3.3/32
#sudo $SFLAG vppctl -p vpp1 set interface state $NAMELC1P0 up
#sudo $SFLAG vppctl -p vpp1 set interface state $NAMELC1P1 up
#sudo $SFLAG vppctl -p vpp1
