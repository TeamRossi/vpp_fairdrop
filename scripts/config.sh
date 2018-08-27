#!/bin/bash

#############################################################################################################
#					PLEASE CONFIGURE THE VARIABLES ONLY THIS SECTION TO MATCH YOUR TESTBED                  #
#############################################################################################################

#################################################################
#                            ____________________________       #
#                           |                            |      #
#   Pktgen          FlowMon |            VPP             |      #
#   ######################  |   ######################   |      #
#   #     Line card 0    #  |   #     Line card 1    #   |      #
#   ##Port-0#####Port-1###  |   ##Port-0#####Port-1###   |      #
#   Tx  |      Rx /|\_______|      /|\Rx    Tx\|/________|      #
#      \|/__________________________|                           #
#                                                               #
#                                                               #
#                                                               #
#################################################################


# Linecards PCI Address
export LC0P0=0000:0b:00.0
export LC0P1=0000:0b:00.1
export LC1P0=0000:84:00.0
export LC1P1=0000:84:00.1

# Linecards dpdk format
export DPDK_LC0P0=0b:00.0
export DPDK_LC0P1=0b:00.1
export DPDK_LC1P0=84:00.0
export DPDK_LC1P1=84:00.1


# Router-friendly Names
export NAMELC0P0="TenGigabitEthernetb/0/0"
export NAMELC0P1="TenGigabitEthernetb/0/1"
export NAMELC1P0="TenGigabitEthernet84/0/0"
export NAMELC1P1="TenGigabitEthernet84/0/1"

# MAC addresses
export MACLC0P0="90:e2:ba:cb:f5:38"
export MACLC0P1="90:e2:ba:cb:f5:39"
export MACLC1P0="90:e2:ba:cb:f5:44"
export MACLC1P1="90:e2:ba:cb:f5:45"

export DEVLC0P0="enp11s0f0"
export DEVLC0P1="enp11s0f1"
export DEVLC1P0="enp132s0f0"
export DEVLC1P1="enp132s0f1"

# IP addresses

export IPV4_VPP_INTERFACE0="192.168.2.2/32"	#This can be left unchanged
export IPV4_VPP_INTERFACE1="192.168.2.3/32"	#This can be left unchanged
export IPV4_VPP_IPROUTE="137.194.208.1"		#This can be left unchanged

#	CPU CORES
export AVAILABLE_CPU_LCORES_FLOWMON=(9 10 11)
export AVAILABLE_CPU_LCORES_PKTGEN=(28 29 30 31 32)
export AVAILABLE_ISOCPU_LCORES_VPP=(20 21 22 23)

# Experiment directory
export EXP=/home/vk/otcs/reproducibility


#############################################################
#                  END OF CONFIGURATION                     #
#       NO FURTHER CHANGES ARE REQUIRED IN THIS FILE.       #
#############################################################

#######VPP CPU CORES#############
export VPP_CPUCORE_MAIN=${AVAILABLE_ISOCPU_LCORES_VPP[0]}			#VPP CPU MAIN CORE FOR ANY EXPERIMENT
export VPP_CPUCORE_BW_WORKER_1=${AVAILABLE_ISOCPU_LCORES_VPP[1]}	#VPP CPU worker core 1 for any bandwidth fairdrop experiment
export VPP_CPUCORE_BW_WORKER_2=${AVAILABLE_ISOCPU_LCORES_VPP[2]}	#VPP CPU worker core 2 for bandwidth fairdrop multicore experiment
export VPP_CPUCORE_BW_FD=${AVAILABLE_ISOCPU_LCORES_VPP[3]}			#VPP CPU fairdrop core for bandwidth fairdrop multicore experiment
export VPP_CPUCORE_WORKER_1=${AVAILABLE_ISOCPU_LCORES_VPP[1]}		#VPP CPU worker core for any cpu fairdrop experiment

########PKTGEN CPU CORES###################
export PKTGEN_MAINCORE=${AVAILABLE_CPU_LCORES_PKTGEN[0]}	#Pktgen Main core
export PKTGEN_CORE_1=${AVAILABLE_CPU_LCORES_PKTGEN[1]}		#Pktgen worker core 1 -rx
export PKTGEN_CORE_2=${AVAILABLE_CPU_LCORES_PKTGEN[2]}		#Pktgen worker core 2 -rx
export PKTGEN_CORE_3=${AVAILABLE_CPU_LCORES_PKTGEN[3]}		#Pktgen worker core 1 -tx
export PKTGEN_CORE_4=${AVAILABLE_CPU_LCORES_PKTGEN[4]}		#Pktgen worker core 2 -tx

#########FlowMon CPU CORES################
export FLOWMON_MAINCORE=${AVAILABLE_CPU_LCORES_FLOWMON[0]}	#FlowMon Main core
export FLOWMON_CORE_1=${AVAILABLE_CPU_LCORES_FLOWMON[1]}	#FlowMon Worker core 1
export FLOWMON_CORE_2=${AVAILABLE_CPU_LCORES_FLOWMON[2]}	#FlowMon Worker core 2

#Experiments
export RESULTS=$EXP/results
export SCRIPTS=$EXP/scripts
export PLOTS=$EXP/plots

# VPP
export VPP_ROOT=$EXP/vpp/vpp
export DPDK_INPUT=$VPP_ROOT/src/plugins/dpdk/device
export IP=$VPP_ROOT/src/vnet/ip
#export MOONGEN=/usr/local/src/MoonGen

# DPDK
#export RTE_SDK=/usr/local/src/dpdk-17.02
export RTE_SDK=$EXP/dpdk-stable-17.02.1
export RTE_PKTGEN=$EXP/pktgen-3.1.2
export RTE_TARGET=x86_64-native-linuxapp-gcc

#FLOW MONITOR
export FLOW=$EXP/FlowMon/DPDK-FlowMon

# Config
export CONFIG_DIR=/home/leos/vpp-bench/scripts
export PATH=$PATH:$CONFIG_DIR:$RTE_SDK/usertools
export STARTUP_CONF=$VPP_ROOT/startup.conf
export BINS="$VPP_ROOT/build-root/install-vpp-native/vpp/bin"
export PLUGS="$VPP_ROOT/build-root/install-vpp-native/vpp/lib64/vpp_plugins"
export SFLAG="env PATH=$PATH:$BINS"

# Aliases
#alias force-update-conf="svn export https://github.com/theleos88/vpp-bench/trunk/scripts --force $CONFIG_DIR && source $CONFIG_DIR/config.sh"
alias update-conf="source $SCRIPTS/config.sh"
alias show-conf="cat $CONFIG_DIR/config.sh"
alias list-scripts="ls -l $CONFIG_DIR/*.sh"
alias dpdk-setup="$RTE_SDK/usertools/dpdk-setup.sh"
alias vppctl="sudo $SFLAG vppctl"

