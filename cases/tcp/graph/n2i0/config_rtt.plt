set terminal png
set output "dumbbell-tp2-rtt.png"
set xlabel "Time(seconds)"
set ylabel "rtt"
set title "rtt Plot"
plot "dumbbell-tp2-20-rtt.data" using 1:2 title "rtt" with lines
