set terminal png
set output "dumbbell-tp2-cwnd-ssthresh.png"
set xlabel "Time(seconds)"
set ylabel "Segmentos"
set title "Congestion Window Plot"
plot "dumbbell-tp2-20-cwnd.data" using 1:2 title "cwnd-n2i0"with lines, \
"dumbbell-tp2-20-ssth.data" using 1:2 title "ssthresh-n2i0" with lines, \
"dumbbell-tp2-30-cwnd.data" using 1:2 title "cwnd-n3i0"with lines, \
"dumbbell-tp2-30-ssth.data" using 1:2 title "ssthresh-n3i0" with lines
