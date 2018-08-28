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
set terminal pdf
set out "j_index_otcs.pdf"
#set size 1,0.7
i="./../results/"
ifilefd=i."jain_perclass_2_fairfx_160.dat"
ifiletd=i."jain_perclass_2_tail_160.dat"
#
set multi layout 1,2

set yrange [0:]
set ytics 0.2
set key top right

set ylabel "J Fairness index"
set yran   [.5:]
set ytics .1
set xrange [1:]

set label "J=" cen at 2,0.57
set label "(r+1)^{/*0.7 2}\n2(r^{/*0.7 2}+1)" cen at 4.5,0.6
set arrow from 2.7,0.57 to 6.5,0.57 nohead lw 0.4
set arrow from 6.6,0.575 to 8,0.6

######### Expected formula for J index #########
TOTALFLOWS=2
BADFLOWS=1
J(x)= ( (BADFLOWS*x) + (TOTALFLOWS-BADFLOWS) )**2 / (TOTALFLOWS* (BADFLOWS*x**2 + (TOTALFLOWS-BADFLOWS)) )

J(x) = (x+1)**2/(2*(x**2+1))
	###########################co####################
set pointsize 1
set size 0.55,.7
set title "FairDrop (FD)"
set xlab  " "
plot \
ifilefd u ($1/310):6 w p lw 2 pt ec lc rgb "blue" title "{CPU Cycles}",\
ifilefd u ($1/310):4  w p lw 2 pt es lc rgb "blue" title "{Throughput}",\
J(x) w l lt -1 lw 2 title ""

#unset arrow
#unset label
set ylab ""
set title "TailDrop (TD)"
set xlab "Ratio of max/min flow costs (r)" offset -12
set size 0.52,.7

plot \
ifiletd u ($1/310):6 w p lw 2 pt fc lc rgb 'red' title "{CPU Cycles}",\
ifiletd u ($1/310):4  w p lw 2 pt fs lc rgb "red" title "{Throughput}",\
J(x) w l lt -1 lw 2 title ""

