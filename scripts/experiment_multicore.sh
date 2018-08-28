sudo killall FlowMon-DPDK
sudo killall vpp_main
sudo killall pktgen
sudo killall MoonGen

	echo -e "\nPktgen-->VPP-->FlowMonitor\n"
	cd $FLOW
	sudo -E ./build/FlowMon-DPDK -l $FLOWMON_MAINCORE,$FLOWMON_CORE_1,$FLOWMON_CORE_2 --file-prefix flow --socket-mem 4096,4096 -b $LC1P1 -b $LC1P0 -b $LC0P0 > /tmp/flow_monitor.dat &
	sleep 30
	sudo -E $SCRIPTS/start_vpp_multicore.sh
	echo "NO ELOG"
	sudo -E $SCRIPTS/pktgen_vpp_default.sh
	echo -e "\nExperiment done\n"
	sleep 5
	#sudo cp /tmp/show $EXP/showrun.dat
	#sudo cp /tmp/data $EXP/showint.dat
#bandwidth
	cat /tmp/flow_monitor.dat | grep "Hash:" | awk '{print $4"\t"$12}' | awk -F "\t|:" '{if($1>0.1){print $1"\t"$2}} ' |sort -rnk1 > /tmp/flow_pps.dat
#cpu fairdrop
	#cat $EXP/flow_monitor.dat | grep "Hash:" | awk '{print $4"\t"$12}' | awk -F "\t|:" '{if ($1>1){if(($2==4157820474)||($2==2122681738)){print $1"\t"$2"\t'$COST'"}else print $1"\t"$2"\t"350}} ' |sort -rnk3 > $EXP/plots/flow_pps.dat

	#echo -e "$(cat $EXP/showrun.dat | grep "vector rates" | head --lines 2 |tail --lines 1 | awk '{print $4}' | awk -F "," '{print IN = $1}')\t$(cat $EXP/showrun.dat | grep "vector rates" | head --lines 2 |tail --lines 1 | awk '{print $6}' | awk -F "," '{print IN = $1}')" > $EXP/in_out.dat
