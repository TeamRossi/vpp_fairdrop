set term post eps color solid  font "Times-Roman,24" linewidth 2 
 set output 'fairness.eps'
 # set terminal aqua font "ariel,24"

     set termoption dash
# reset linetypes to base dash patterns
#
set for [i=1:5] linetype i dt i
      set size ratio .6
#     set bmargin 0
 #    set tmargin 0
 #     set size 1,1
      unset log                              # remove any log-scaling
      unset label                            # remove any previous labels
      set xtic auto                          # set xtics automatically
      set ytic auto                          # set ytics automatically
#set title "3 flows, overhead=.5"
set xrange [0 : 4]
#set yrange [0 : 1]
set xlabel "load"
set ylabel tc rgb "black" "normalized cycle/s"
set ytics tc rgb "black" .2
set xtics tc rgb "black" 1
#set tics out
 #    set key right top
set key at 3.5,.52
    set pointsize 2
    plot  "fairness.txt" using 2:4 title "cost 5" with lines axes x1y1 lc rgbcolor "blue"  lw 2,\
          "fairnessNodrop.txt" using 2:4 title "" with lines axes x1y1 lt 2 lc rgbcolor "blue"   lw 2,\
          "fairness.txt" using 2:8 title "cost 3" with lines axes x1y1 lt 1 lc rgbcolor "red"lw 2,\
          "fairnessNodrop.txt" using 2:8 title "" with lines axes x1y1 lt 2 lc rgbcolor "red"lw 2,\
          "fairness.txt" using 2:12 title "cost 1" with lines axes x1y1 lt 1 lc rgbcolor "black"  lw 2,\
          "fairnessNodrop.txt" using 2:12 title "" with lines axes x1y1 lt 2 lc rgbcolor "black"  lw 2

#          "fairness3min240.txt" using 2:6 title "" with lines axes x1y1 lc rgbcolor "0x990000"  lw 2,\
          "fairness3min240.txt" using 2:13 title "" with lines axes x1y1 lc rgbcolor "0xFF3333"lw 2,\
          "fairness3min240.txt" using 2:20 title "" with lines axes x1y1 lc rgbcolor "0x003366"  lw 2


#          "fairness3.txt" using 2:25 title "util" with lines axes x1y1 lc "black"  lw 2