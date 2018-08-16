#Configuration
if [[ "$1" == "otcs" ]];then
export FILE=("$VPP_ROOT/src/plugins/dpdk/device/flow_table.h" "$VPP_ROOT/src/plugins/dpdk/device/fifo.h")
export GIT_BRANCH=("bandwidth_fairdrop_multicore_op" "bandwidth_taildrop_multicore_op")
export F_NAME=("FAIRDROP" "TAILDROP")
export CEXP=MULTICORE
export FE=otcs
export ALPHA_ARRAY=(0.5 1.0)
export EXPERIMENT="experiment_multicore"
fi

if [[ "$1" == "networking" ]];then
export FILE=("$VPP_ROOT/src/plugins/dpdk/device/flow_table.h" "$VPP_ROOT/src/vnet/ip/fifo.h")
export GIT_BRANCH=("bandwidth_fairdrop_networking18" "bandwidth_fifo")
export F_NAME=("FAIRDROP" "TAILDROP")
export CEXP=SINGLECORE
export FE=networking
export ALPHA_ARRAY=(0.1 0.4 0.6)
export EXPERIMENT="experiment_bw"
fi

#DPDK-Pktgen
cp $SCRIPTS/pktgen-ipv4-bandwidth_"$FE".c $RTE_PKTGEN/app/pktgen-ipv4.c
cd $RTE_PKTGEN
make

#Experiment fairdrop vs taildrop
for i in 0 1;do
	cd $VPP_ROOT
	git checkout -f ${GIT_BRANCH[$i]}
	sudo make rebuild-release
	for ALPHA in ${ALPHA_ARRAY[@]};do
		sudo killall vpp_main
		mkdir $RESULTS/"${F_NAME[$i]}"_BW_"$CEXP"_"$ALPHA"
		sed -i "s/^\(#define ALPHA \).*/\1$ALPHA/" ${FILE[$i]}
		$SCRIPTS/"$EXPERIMENT".sh
		cp /tmp/flow_monitor.dat $RESULTS/"${F_NAME[$i]}"_BW_"$CEXP"_"$ALPHA"/
		cp /tmp/flow_pps.dat $RESULTS/"${F_NAME[$i]}"_BW_"$CEXP"_"$ALPHA"/
	done
sudo killall vpp_main
sudo killall pktgen
done
cd $PLOTS
gnuplot figure4_"$FE".gp
echo "FINSISHED EXPERIMENT"
