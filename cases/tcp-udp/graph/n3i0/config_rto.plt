set terminal png
set output "dumbbell-tp2-rto.png"
set xlabel "Time(seconds)"
set ylabel "rto"
set title "rto Plot"
plot "dumbbell-tp2-30-rto.data" using 1:2 title "rto" with lines
