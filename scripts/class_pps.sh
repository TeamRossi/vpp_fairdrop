# results folders naming ---> FAIRDROP_CPU_"$WEIGHT"_"$N_HEAVY"_"$DROP_MECH"SIZE_"$PKT_SIZE"
# $1=$N_HEAVY is the number of heavy flows used in the experiment. By default it is 2.
# $2=$DROP_MECH which is the dropping mechanism which belongs to {FAIRFX TAIL}
# $3=$PKT_SIZE which is the packet size used in the experiment. Default value is 64.
# $4 is the weight increment value. It is either 310 or 350 in our experiments. 350 for experiments of networking and 310 for experiments of otcs.

#####################################    HOW TO USE THIS SCRIPT    #######################################################
#After running a full cpu experiment with results from weight ratios 1 to 14, you can run this script without any errors.#
# class_pps.sh <N_HEAVY> <DROP_MECH> <PKT_SIZE> <WEIGHT_INC>                                                             #
##########################################################################################################################
i=$3

WEIGHT=$4
LIMIT=$WEIGHT*14
while [[ $WEIGHT -le $LIMIT ]];do
	j=FAIRDROP_CPU_"$WEIGHT"_"$1"_"$2"SIZE
	# if [[ $1 -eq 1 ]]; then
	CLASS_1=$(cat $RESULTS/"$j"_"$i"/flow_pps.dat | grep 4157820474 | awk '{print $1}')
	CLASS_2=$(cat $RESULTS/"$j"_"$i"/flow_pps.dat | head --lines 1 | awk '{print $1}')
	CLOCK_1=$(python -c "print($CLASS_1 * $WEIGHT)")
	CLOCK_2=$(python -c "print($CLASS_2 * $4)")

	SUM=$(python -c "print($CLASS_1+$CLASS_2)")
	SQ=$(python -c "print(($CLASS_1*$CLASS_1)+($CLASS_2*$CLASS_2))")
	JAIN=$(python -c "print(($SUM*$SUM)/(2*$SQ))")

	SUM1=$(python -c "print($CLOCK_1+$CLOCK_2)")
	SQ1=$(python -c "print(($CLOCK_1*$CLOCK_1)+($CLOCK_2*$CLOCK_2))")
	JAIN1=$(python -c "print(($SUM1*$SUM1)/(2*$SQ1))")
	echo -e  "$WEIGHT\t$i\tThroughput_Fairness_index\t$JAIN\tCycles_Fairness_index\t$JAIN1"

	# else
	# D=$RESULTS/"$j"_"$i"/plots
	# cat $D/flow_pps.dat | awk -v x=$i 'BEGIN{sum=0;sum1=0; sq=0;sq1=0}{ sum+=($1);sum1+=($1*$3); sq+= (($1)*($1)); sq1+=(($1*$3)*($1*$3)); }END{ print ( "WEIGHT\t",x,"\tThroughput_Fairness_index\t", (sum*sum)/(NR*sq),"\tCycles_Fairness_index\t", (sum1*sum1)/(NR*sq1) )}'
	# #cat $D/flow_pps.dat | awk -v x=$i 'BEGIN{sum=0; sq=0;}{ sum+=($1*$3); sq+= (($1*$3)*($1*$3)); }END{ print ( "WEIGHT\t", x, "Cycle Fairness index\t", (sum*sum)/(NR*sq) )}'
	# fi
	WEIGHT=$(( $WEIGHT+$4 ))
done
