#! xgp DATA/fair-drop_bell64_turbo_0.1.dat DATA/fair-drop_bell64_turbo_0.2.dat DATA/fair-drop_bell64_turbo_0.3.dat DATA/fair-drop_bell64_turbo_0.4.dat DATA/fair-drop_bell64_turbo_0.5.dat DATA/fair-drop_bell64_turbo_0.6.dat DATA/fair-drop_bell64_turbo_0.7.dat DATA/fair-drop_bell64_turbo_0.8.dat DATA/fair-drop_bell64_turbo_0.9.dat DATA/fair-drop_bell64_turbo_1.0.dat DATA/fair-drop_bell64_turbo_fifo_0.1.dat DATA/fair-drop_bell64_turbo_fifo_0.2.dat DATA/fair-drop_bell64_turbo_fifo_0.3.dat DATA/fair-drop_bell64_turbo_fifo_0.4.dat DATA/fair-drop_bell64_turbo_fifo_0.5.dat DATA/fair-drop_bell64_turbo_fifo_0.6.dat DATA/fair-drop_bell64_turbo_fifo_0.7.dat DATA/fair-drop_bell64_turbo_fifo_0.8.dat DATA/fair-drop_bell64_turbo_fifo_0.9.dat DATA/fair-drop_bell64_turbo_fifo_1.0.dat 2:1
# Created the Tue Jul 11 16:30:25 CEST 2017 with:
#   ____________________________
#  /                            \
# /        xgp        __________/
# \__________________/.:nonsns:.
#
 
#-------------------------------------
# SHAPES:
# (c)ircle  (t)riangle (i)nv.tri.
#           (d)iamond  (s)quare
# FILLING:
# (f)illed  (o)paque  (e)mpty (default)
#-------------------------------------
   s=4  ; es=s  ; fs=s+1 ; os=s+66
   c=6  ; ec=c  ; fc=c+1 ; oc=os+1
   t=8  ; et=t  ; ft=t+1 ; ot=oc+1
   i=10 ; ei=i  ; fi=i+1 ; oi=ot+1
   d=12 ; ed=d  ; fd=d+1 ; od=oi+1
   p=14 ; ep=p  ; fp=p+1 ; op=od+1
#-------------------------------------
#  e.g.:  
#  empty circles vs filled squares
#  plot "file" u 1:3 w p pt ec, 
#           "" u 1:4 w lp pt fs
#-------------------------------------

#Happy gnuplotting

#set out "figure4_otcs.pdf"
#set term pdf font "Times,18"
set pointsize 1.25

set key top righ
set ylabel "Per flow rate [pps]"
set xlabel "Flow rank"
set format y "%.1t^.10^%T"
set ytics 200000
set yran [0:1500000]
set xrange [0:19]
min(a,b)=a<b?a:b

plot \
'./../results/FAIRDROP_BW_MULTICORE_0.5/flow_pps.dat'    	u ($0):1   	t 'FD  ({/Symbol a}=0.5)'  	w  lp pt ec lc rgb "red", \
'./../results/TAILDROP_BW_MULTICORE_0.5/flow_pps.dat'	u ($0):1   	t 'TD  ({/Symbol a}=0.5)'	w  lp pt fc lc rgb "red", \
'./../results/FAIRDROP_BW_MULTICORE_1.0/flow_pps.dat'   		u ($0):1   	t 'FD  ({/Symbol a}=1.0)'  	w  lp pt ec lc rgb "black", \
'./../results/TAILDROP_BW_MULTICORE_1.0/flow_pps.dat'	u ($0):1   	t 'TD  ({/Symbol a}=1.0)'	w  lp pt fc lc rgb "black"

#'./../results/FAIRDROP_BW_MULTICORE_0.9/flow_pps.dat'       u ($0):1    t 'FD  ({/Symbol a}=0.9)'   w  lp pt ec lc rgb "blue", \
#'./../results/TAILDROP_BW_MULTICORE_0.9/flow_pps.dat'   u ($0):1    t 'TD  ({/Symbol a}=0.9)'   w  lp pt fc lc rgb "blue", \

