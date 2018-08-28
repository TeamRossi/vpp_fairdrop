# $1 represents weight of heavy flow in the corresponding experiment
# $2 represents the packet size in the experiment

#FAIRDROP_CPU_3100_2_FAIRFXSIZE_160

if [ "$3" == "networking" ];then
DROP_COST=208
F_NAME=("FAIRNET" "TAILNET")
D_NAME=("fairnet" "tailnet")
fi
if [ "$3" == "otcs" ];then
	if [ $4 -eq 160 ];then
		DROP_COST=150
	fi
	if [ $4 -eq 64 ];then
		DROP_COST=125
	fi
F_NAME=("FAIRFX" "TAIL")
D_NAME=("fairfx" "tail")
fi

cat $RESULTS/sanki_"$4"_"$3".dat | grep $1 | grep $2 > /tmp/sanki.dat

FD_RX_MISS=$(cat /tmp/sanki.dat | head -n 1 | awk '{print $5}')
FD_RX_MISS_X=$(python -c "print(int($FD_RX_MISS*10))")

TD_RX_MISS=$(cat /tmp/sanki.dat | tail -n 1 | awk '{print $5}')
TD_RX_MISS_X=$(python -c "print(int($TD_RX_MISS*10))")

FD_DROP=$(cat /tmp/sanki.dat | head -n 1 | awk '{print $9}')
FD_DROP_X=$(python -c "print(int($FD_DROP*10))")

FD_INPUT=$(cat /tmp/sanki.dat | head -n 1 | awk '{print $7}')
TD_INPUT=$(cat /tmp/sanki.dat | tail -n 1 | awk '{print $7}')

FD_OUTPUT=$(cat /tmp/sanki.dat | head -n 1 | awk '{print $11}')
TD_OUTPUT=$(cat /tmp/sanki.dat | tail -n 1 | awk '{print $11}')

FD_TX=$(cat /tmp/sanki.dat | head -n 1 | awk '{print $13}')
TD_TX=$(cat /tmp/sanki.dat | tail -n 1 | awk '{print $13}')

F_DROP=$(python -c "print(round((($FD_RX_MISS/$FD_TX)*100),2))")
T_DROP=$(python -c "print(round((($TD_RX_MISS/$TD_TX)*100),2))")

FD_CYCLES=$(python -c "print(round($FD_DROP*$DROP_COST/1000 , 2))")

FDO_CYCLES=$(python -c "print(round(2.6-$FD_CYCLES , 2))")
TDO_CYCLES=2.6



HFDTX_PPS=$(python -c "print(round(($FD_TX/20.0),2))")
HTDTX_PPS=$(python -c "print(round(($TD_TX/20.0),2))")
LFDTX_PPS=$HFDTX_PPS
LTDTX_PPS=$HTDTX_PPS

HFD_PPS=$(cat $RESULTS/FAIRDROP_CPU_"$1"_2_"${F_NAME[0]}"SIZE_"$2"/flow_pps.dat | tail -n 1 | awk '{print $1}') #$(python -c "print(round(($FD_OUTPUT/182.0),2))")
HFD_PPS=$(python -c "print(round(($HFD_PPS/1000000.0),3))")

LFD_PPS=$(cat $RESULTS/FAIRDROP_CPU_"$1"_2_"${F_NAME[0]}"SIZE_"$2"/flow_pps.dat | head -n 1 | awk '{print $1}') #$(python -c "print(round(($FD_OUTPUT*10/182.0),2))")
LFD_PPS=$(python -c "print(round(($LFD_PPS/1000000.0),3))")

HTD_PPS=$(cat $RESULTS/FAIRDROP_CPU_"$1"_2_"${F_NAME[1]}"SIZE_"$2"/flow_pps.dat | tail -n 1 | awk '{print $1}') #$(python -c "print(round(($FD_OUTPUT/20.0),2))")
HTD_PPS=$(python -c "print(round(($HTD_PPS/1000000.0),3))")

LTD_PPS=$(cat $RESULTS/FAIRDROP_CPU_"$1"_2_"${F_NAME[1]}"SIZE_"$2"/flow_pps.dat | head -n 1 | awk '{print $1}') #$HTD_PPS
LTD_PPS=$(python -c "print(round(($LTD_PPS/1000000.0),3))")

# HFDTX_CKS=$(python -c "print(round(($HFDTX_PPS*3100),3))")
# HTDTX_CKS=$(python -c "print(round(($HTDTX_PPS*3100),3))")
# LFDTX_CKS=$(python -c "print(round(($LFDTX_PPS*310),3))")
# LTDTX_CKS=$(python -c "print(round(($LTDTX_PPS*310),3))")

# HFD_CKS=$(python -c "print(round(($HFD_PPS*3100),3))")
# LFD_CKS=$(python -c "print(round(($LFD_PPS*310),3))")

# HTD_CKS=$(python -c "print(round(($HTD_PPS*3100),3))")
# LTD_CKS=$(python -c "print(round(($LTD_PPS*310),3))")

FDH_DROP=98
FDL_DROP=2
FDH_CYCLES=5
FDL_CYCLES=5
TDH_CYCLES=25
TDL_CYCLES=2.8

if [[ $FD_RX_MISS_X -eq 0 && TD_RX_MISS_X -eq 0 ]];then
	F=1
fi
if [[ $FD_RX_MISS_X -eq 0 && TD_RX_MISS_X -gt 0 && FD_DROP_X -gt 0 ]];then
	cp $SCRIPTS/zero_miss_fd.svg $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_TX/$FD_TX/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_TX/$TD_TX/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_INPUT/$FD_INPUT/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_INPUT/$TD_INPUT/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_OUTPUT/$FD_OUTPUT/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_OUTPUT/$TD_OUTPUT/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_DROP/$FD_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FD_CYCLES/$FD_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDH_DROP/$FDH_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDL_DROP/$FDL_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FDH_CYCLES/$FDH_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDL_CYCLES/$FDL_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/TAIL_DROP/$TD_RX_MISS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/T_DROP/$T_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/TDH_CYCLES/$TDH_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TDL_CYCLES/$TDL_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FDO_CYCLES/$FDO_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TDO_CYCLES/$TDO_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFDTX_PPS/$HFDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFDTX_PPS/$LFDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTDTX_PPS/$HTDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTDTX_PPS/$LTDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFD_PPS/$HFD_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFD_PPS/$LFD_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTD_PPS/$HTD_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTD_PPS/$LTD_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFDTX_CKS/$HFDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFDTX_CKS/$LFDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTDTX_CKS/$HTDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTDTX_CKS/$LTDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFD_CKS/$HFD_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFD_CKS/$LFD_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTD_CKS/$HTD_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTD_CKS/$LTD_CKS/" $PLOTS/sanki_$1_$2_$3.svg

fi
if [[ $FD_RX_MISS_X -gt 0 && TD_RX_MISS_X -gt 0 && FD_DROP_X -gt 0 ]];then
	cp $SCRIPTS/miss_fd_td.svg $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FD_NIC_DROP/$FD_RX_MISS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/F_DROP/$F_DROP/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/TAIL_DROP/$TD_RX_MISS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/T_DROP/$T_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_TX/$FD_TX/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_TX/$TD_TX/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_INPUT/$FD_INPUT/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_INPUT/$TD_INPUT/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_OUTPUT/$FD_OUTPUT/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TAIL_OUTPUT/$TD_OUTPUT/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FD_DROP/$FD_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FD_CYCLES/$FD_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDH_DROP/$FDH_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDL_DROP/$FDL_DROP/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FDH_CYCLES/$FDH_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/FDL_CYCLES/$FDL_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/TDH_CYCLES/$TDH_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TDL_CYCLES/$TDL_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	
	sed -i -e "s/FDO_CYCLES/$FDO_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/TDO_CYCLES/$TDO_CYCLES/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFDTX_PPS/$HFDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFDTX_PPS/$LFDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTDTX_PPS/$HTDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTDTX_PPS/$LTDTX_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFD_PPS/$HFD_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFD_PPS/$LFD_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTD_PPS/$HTD_PPS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTD_PPS/$LTD_PPS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFDTX_CKS/$HFDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFDTX_CKS/$LFDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTDTX_CKS/$HTDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTDTX_CKS/$LTDTX_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HFD_CKS/$HFD_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LFD_CKS/$LFD_CKS/" $PLOTS/sanki_$1_$2_$3.svg

	sed -i -e "s/HTD_CKS/$HTD_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	sed -i -e "s/LTD_CKS/$LTD_CKS/" $PLOTS/sanki_$1_$2_$3.svg
	
fi
inkscape $PLOTS/sanki_$1_$2_$3.svg --export-pdf=$PLOTS/sanki_$1_$2_$3.pdf