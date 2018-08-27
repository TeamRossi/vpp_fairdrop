echo -e "\n\n\n$VPP_ROOT\n\n\n"
sudo $SFLAG $BINS/vppctl -p vpp2 set ip arp $NAMELC1P1 $IPV4_VPP_IP_ROUTE $MACLC0P1 # 137.194.208.1 is just a random ip address. It can be anything. The same ip address should be used in ip route command. We are just forwarding all packets to one interface.
sudo $SFLAG $BINS/vppctl -p vpp2 ip route add 0.0.0.0/0 via $IPV4_VPP_IPROUTE # 137.194.208.1 This ip should be same the one used before. This is just to forward all packets to single output interface.
sudo $SFLAG $BINS/vppctl -p vpp2 set interface promiscuous on $NAMELC1P1
sudo $SFLAG $BINS/vppctl -p vpp2 set interface promiscuous on $NAMELC1P0
sudo $SFLAG $BINS/vppctl -p vpp2 set int ip addr $NAMELC1P1 $IPV4_VPP_INTERFACE0 #192.168.2.2/32
sudo $SFLAG $BINS/vppctl -p vpp2 set int ip addr $NAMELC1P0 $IPV4_VPP_INTERFACE1 #192.168.2.3/32
sudo $SFLAG $BINS/vppctl -p vpp2 set interface state $NAMELC1P0 up
sudo $SFLAG $BINS/vppctl -p vpp2 set interface state $NAMELC1P1 up
