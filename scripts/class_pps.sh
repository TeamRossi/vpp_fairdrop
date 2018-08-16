i=$3

#EXP=/home/vk/otcs
#RESULTS=/home/vk/otcs/results/FAIRDROP_CPU
#SCRIPTS=/home/vk/otcs/scripts

# ALG[0]=FAIRFX
# ALG[1]=FAIRAD
# ALG[2]=TAIL

# A[0]=fairfx
# A[1]=fairad
# A[2]=tail

# B[0]=fairdrop_cpu_shadowact_fxth
# B[1]=fairdrop_cpu_shadowact_adth
# B[2]=busyloops_nofairdrop
WEIGHT=$4
LIMIT=$WEIGHT*14
while [[ $WEIGHT -le $LIMIT ]];do
	j=FAIRDROP_CPU_"$WEIGHT"_"$1"_"$2"SIZE
	# if [[ $1 -eq 1 ]]; then
	CLASS_1=$(cat $RESULTS/"$j"_"$i"/flow_pps.dat | grep 4157820474 | awk '{print $1}')
	CLASS_2=$(cat $RESULTS/"$j"_"$i"/flow_pps.dat | head --lines 1 | awk '{print $1}')
	CLOCK_1=$(python -c "print($CLASS_1 * $WEIGHT)")
	CLOCK_2=$(python -c "print($CLASS_2 * 310)")

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
