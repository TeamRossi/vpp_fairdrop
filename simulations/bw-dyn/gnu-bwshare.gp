
set term post eps color solid  font "Times-Roman,24" linewidth 2 
 set output 'bwperf.eps'
#  set terminal aqua font "ariel,24"

     set termoption dash
      set size ratio .6
     set bmargin 0
     set tmargin 0
 #     set size 1,1
      unset log                              # remove any log-scaling
      unset label                            # remove any previous labels
      set xtic auto                          # set xtics automatically
      set ytic auto                          # set ytics automatically
 #   set title "batching 10"
unset title
set xrange [0 : 1]
set x2range [0 : 1]
set yrange [0 : 1]
set y2range [0 : 60]
set xlabel "load"
set ylabel tc rgb "black" "flow throughput (b/s)"
set y2label tc rgb "blue" "ActiveList 99 percentile"
set ytics nomirror
set y2tics tc rgb "blue" 10
set ytics tc rgb "black" .2
set tics out
#set autoscale  y
#set autoscale y2
#     set key right top
#   set xtics 1, 10, 100
#   set mxtics 1
    set pointsize 2

     plot           1-x title "" with lines axes x1y1  lc rgbcolor "0x80A0A0"  lw 3, \
          ceil(log(.01)/log(x)) title "" with lines axes x2y2 lc rgbcolor "0x66B2FF" lw 3, \
         "bw-greedyFD-plot.txt" using 1:3 title "" with points axes x1y1 lc rgb "black" pt 1 lw 1,\
          "bw-greedyFD-plot.txt" using 1:20 title "" with points axes x2y2 lc rgb "blue" pt 1 lw 1

#         "bw-batch10-newFD.txt" using 1:3 title "" with points axes x1y1 lc rgb "black" pt 4 lw 1,\
#         "bw-batch10-newFD.txt" using 1:20 title "" with points axes x2y2 lc rgb "blue" pt 4 lw 1,\

#          "bw-greedyFDbatch10-1-plot.txt" using 1:3 title "" with points axes x1y1 lc rgb "black" pt 4 lw 1,\
#          "bw-greedyFDbatch10-1-plot.txt" using 1:20 title "" with points axes x2y2 lc rgb "blue" pt 4 lw 1,\

