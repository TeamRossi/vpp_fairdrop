#sudo $VPP_ROOT/build-root/install-vpp-native/vpp/bin/vpp  unix {nodaemon} cpu {main-core 28} api-segment {prefix vpp1} dpdk { dev $LC1P0  dev $LC1P1 socket-mem 4096,4096} plugin_path $VPP_ROOT/build-root/install-vpp-native/vpp/lib64/vpp_plugins
#ip4
#source /home/vk/scripts_cpu/config.sh
#BINS=/home/vk/vppdev/vpp/build-root/install-vpp-native/vpp/bin

######
echo -e "\n\n\n$VPP_ROOT\n\n\n"
#BINS=$VPP_ROOT/build-root/install-vpp-native/vpp/bin

sudo $SFLAG $BINS/vppctl -p vpp2 set ip arp $NAMELC1P1 137.194.208.1 $MACLC0P1
sudo $SFLAG $BINS/vppctl -p vpp2 ip route add 0.0.0.0/0 via 137.194.208.1
sudo $SFLAG $BINS/vppctl -p vpp2 set interface promiscuous on $NAMELC1P1
sudo $SFLAG $BINS/vppctl -p vpp2 set interface promiscuous on $NAMELC1P0
sudo $SFLAG $BINS/vppctl -p vpp2 set int ip addr $NAMELC1P1 192.168.2.2/32
sudo $SFLAG $BINS/vppctl -p vpp2 set int ip addr $NAMELC1P0 192.168.2.3/32
sudo $SFLAG $BINS/vppctl -p vpp2 set interface state $NAMELC1P0 up
sudo $SFLAG $BINS/vppctl -p vpp2 set interface state $NAMELC1P1 up
#####

#ip6
#sudo $SFLAG $BINS/vppctl -p vpp2 set interface ip address $NAMELC1P0 $IP6LC1P0/128
#sudo $SFLAG $BINS/vppctl -p vpp2 set interface ip address $NAMELC1P1 $IP6LC1P1/128

#sudo -E $SFLAG $BINS/vppctl -p vpp2 ip route add ::/0 via $DEFAULTIP6 $NAMELC1P1
#sudo -E $SFLAG $BINS/vppctl -p vpp2 set ip6 neighbor $NAMELC1P1 $DEFAULTIP6 $MACLC1P0 static

#####

#dpdk-interface placement
#sudo $SFLAG $BINS/vppctl -p vpp2 set dpdk interface placement TenGigabitEthernet84/0/0 queue 1 thread 1
#sudo $SFLAG $BINS/vppctl -p vpp2 set dpdk interface placement TenGigabitEthernet84/0/1 queue 0 thread 2

#XCONNECT
#sudo $SFLAG $BINS/vppctl -p vpp2 set interface l2 xconnect TenGigabitEthernet84/0/0 TenGigabitEthernet84/0/1
#sudo $SFLAG $BINS/vppctl -p vpp2 set interface l2 xconnect TenGigabitEthernet84/0/1 TenGigabitEthernet84/0/0
