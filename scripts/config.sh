#!/bin/bash

# Linecards
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
export IPLC0P0="1.1.1.11"
export IPLC0P1="1.1.1.12"
export IPLC1P0="1.1.1.21"
export IPLC1P1="1.1.1.22"

# IP6 addresses
export IP6LC1P0="2011:2::ffff"
export IP6LC1P1="2011:3::ffff"

export IPLOOP="10.0.0.0"
export IP6LOOP="2011:1::ffff"

# Default routes
export DEFAULTIP="99.99.99.99"
export DEFAULTIP6="2211:2::ffff"

#Experiments
export EXP=/home/vk/otcs/reproducibility
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
export FLOW="/home/vk/FLOW_MONITOR/DPDK-FlowCount"

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

