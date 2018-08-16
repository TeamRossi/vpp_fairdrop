#	USAGE
#	./in_out.sh <number of heavy flows used in the experiment> <Algorithm>
#	Number of heavy flows used in the experiment = 2 or 5 in these experiments
#	Algorithm = FAIRAD or FAIRFX or TAIL
#	FAIRAD is fairdrop with adaptive threshold
#	FAIRFX is fairdrop with fixed threshold
#	TAIL is taildrop

i=$3;

#EXP=/home/vk/otcs
#RESULTS=/home/vk/otcs/results/FAIRDROP_CPU
#SCRIPTS=/home/vk/otcs/scripts
WEIGHT=$4
LIMIT=$WEIGHT*14
while [[ $WEIGHT -le $LIMIT ]];do
IN=$(cat $RESULTS/FAIRDROP_CPU_"$WEIGHT"_"$1"_"$2"SIZE_$i/showrun.dat | grep "vector rates" | tail -n 1 | awk '{print $4}' | awk -F "," '{print $1}')
OUT=$(cat $RESULTS/FAIRDROP_CPU_"$WEIGHT"_"$1"_"$2"SIZE_$i/showrun.dat | grep "vector rates" | tail -n 1 | awk '{print $6}' | awk -F "," '{print $1}')
echo  -e "Rate = 10Gbps\tPktsize = $i\theavy_weight = $WEIGHT\tinput = $IN\tOutput = $OUT"
WEIGHT=$(( $WEIGHT+$4 ))
done
