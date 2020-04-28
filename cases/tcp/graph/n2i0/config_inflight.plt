set terminal png
set output "dumbbell-tp2-inflight.png"
set xlabel "Time(seconds)"
set ylabel "inflight"
set title "inflight Plot"
plot "dumbbell-tp2-20-inflight.data" using 1:2 title "inflight" with lines
