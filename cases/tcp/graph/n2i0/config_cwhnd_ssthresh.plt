set terminal png
set output "dumbbell-tp2-cwnd-ssthresh.png"
set xlabel "Time(seconds)"
set ylabel "segmentos"
set title "Congestion Window Plot"
plot "dumbbell-tp2-20-cwnd.data" using 1:2 title "cwnd"with lines, \
"dumbbell-tp2-20-ssth.data" using 1:2 title "ssthresh" with lines
