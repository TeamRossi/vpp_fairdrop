FILE=$VPP_ROOT/src/plugins/dpdk/device/flow_table_cpu.h

if [[ "$1" == "otcs" ]];then
export GIT_BRANCH=("fairdrop_cpu_shadowact_fxth" "busyloops_nofairdrop")
export F_NAME=("FAIRFX" "TAIL")
export D_NAME=("fairfx" "tail")
export CEXP=otcs
export STEP=310
export SANKI=(3100 160)
export SIZE_ARRAY=(64 160)
fi
if [[ "$1" == "networking" ]];then
export GIT_BRANCH=("fairdrop_cpu_networking18" "busyloops_nofairdrop")
export F_NAME=("FAIRNET" "TAILNET")
export D_NAME=("fairnet" "tailnet")
export CEXP=networking
export STEP=350
export SANKI=(3500 64)
export SIZE_ARRAY=(64)
fi



LIMIT=$(( $STEP*14 ))

cp $SCRIPTS/pktgen-ipv4-cpu.c $RTE_PKTGEN/app/pktgen-ipv4.c
cd $RTE_PKTGEN
make

for i in 0 1;do
	#2 Heavy Flows
	cd $VPP_ROOT
	git checkout -f ${GIT_BRANCH[$i]}
	sudo make rebuild-release

	for SIZE in ${SIZE_ARRAY[@]};do
		sed -i "s/^\(SIZE=\).*/\1$SIZE/" $SCRIPTS/pktgen_vpp_default_rate.lua
		WEIGHT=$STEP
		while [[ $WEIGHT -le $LIMIT ]];do
			sudo killall vpp_main
			mkdir $RESULTS/FAIRDROP_CPU_"$WEIGHT"_2_"${F_NAME[$i]}"SIZE_"$SIZE"
			sed -i "s/^\(#define WEIGHT_CLASS_1 \).*/\1$WEIGHT/" $FILE
			$SCRIPTS/experiment_cpu_rate.sh
			$SCRIPTS/copy_dat.sh FAIRDROP_CPU_"$WEIGHT"_2_"${F_NAME[$i]}"SIZE_"$SIZE"
			WEIGHT=$(( $WEIGHT+$STEP ))
		done
	    $SCRIPTS/in_out.sh 2 ${F_NAME[$i]} $SIZE $STEP > $RESULTS/in_out_2_"${D_NAME[$i]}"_"$SIZE".dat
    	$SCRIPTS/class_pps.sh 2 ${F_NAME[$i]} $SIZE $STEP > $RESULTS/jain_perclass_2_"${D_NAME[$i]}"_"$SIZE".dat
		$SCRIPTS/sanki_fair_size.sh ${F_NAME[0]} ${F_NAME[1]} $STEP $SIZE > $RESULTS/sanki_"$SIZE"_"$CEXP".dat
	done

	sudo killall vpp_main
	sudo killall pktgen
	echo "FINSISHED EXPERIMENT with 2 Heavy Flows and $CPU_GIT"

	$SCRIPTS/sanki_art.sh ${SANKI[0]} ${SANKI[1]} $CEXP $SIZE
	cd $PLOTS
	gnuplot j_index_$CEXP.gp
	echo "###################"
	echo ""
	echo "All the plots in $EXP/plots folder"
	echo ""
	echo "###################"


done

echo "###################"
echo ""
echo "EXPERIMENT FINSIHED"
echo ""
echo "###################"
