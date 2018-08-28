sudo killall FlowCount
sudo killall vpp_main
sudo killall pktgen


	echo -e "\nPktgen-->VPP-->FlowMonitor\n"
	cd $FLOW
	sudo -E ./build/FlowMon-DPDK -l $FLOWMON_MAINCORE,$FLOWMON_CORE_1,$FLOWMON_CORE_2 --file-prefix flow --socket-mem 4096,4096 -b $LC1P1 -b $LC1P0 -b $LC0P0 > /tmp/flow_monitor.dat &
	sleep 30
	sudo -E $SCRIPTS/start_vpp_singlecore_cpu.sh
	echo "NO ELOG"
	sudo -E $SCRIPTS/pktgen_vpp_default_rate.sh
	echo -e "\nExperiment done\n"
	sleep 5

	cat /tmp/flow_monitor.dat | grep "Hash:" | awk '{print $4"\t"$12}' | awk -F "\t|:" '{if($1>0.1){print $1"\t"$2}} ' |sort -rnk1 > /tmp/flow_pps.dat

	echo  "$(cat /tmp/showrun.dat | grep "vector rates" | head --lines 2 |tail --lines 1 | awk '{print $4}' | awk -F "," '{print IN = $1}')\t$(cat /tmp/showrun.dat | grep "vector rates" | head --lines 2 |tail --lines 1 | awk '{print $6}' | awk -F "," '{print IN = $1}')" > /tmp/in_out.dat
