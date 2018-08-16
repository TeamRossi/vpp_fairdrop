cd $RTE_PKTGEN
sudo -E app/app/x86_64-native-linuxapp-gcc/pktgen -l 28,29-32 --file-prefix pktgenfd --socket-mem 8192,8192 --proc-type auto -w $LC0P0 -n 4 -- -P -T -m "[29-30:31-32].0" -f $SCRIPTS/pktgen_vpp_default.lua #,[17-18:19-20].1"
#sudo -E app/app/x86_64-native-linuxapp-gcc/pktgen -l 28,29-32 --file-prefix pktgenfd --socket-mem 8192,8192 --proc-type auto -w $LC0P0 -w $LC0P1 -n 4 -- -P -T -m "[29:30].0,[31:32].1" #-f /home/vk/otcs/scripts/pktgen_vpp_default.lua #,[17-18:19-20].1"
# -w $LC0P0 -w $LC0P1	-b $LC1P0 -b $LC1P1
