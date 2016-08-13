reset

set xtic auto                          # set xtics automatically
set ytic auto 

set grid
set style data linespoints
set key left top

set xlabel "Time [min]"

set ylabel "Percentage covered"

set  linetype 1 linewidth 1 pt 1 ps 0.5

plot "5#14" using 4:2 title '' with linespoints

unset output
