reset

set xtic auto                          # set xtics automatically
set ytic auto 

set grid
set style data linespoints
set key left top

set xlabel "Time [min]"

set ylabel "Percentage covered"

set  linetype 1 linewidth 1 pt 1 ps 0.5

plot "1#st-up-s1" using 4:2 title '' with linespoints ,\
"1#st-up-s2" using 4:2 title '' with linespoints ,\
"1#st-up-s3" using 4:2 title '' with linespoints ,\
"1#st-up-s4" using 4:2 title '' with linespoints ,\
"1#st-up-s5" using 4:2 title '' with linespoints ,\
"1#st-up-s6" using 4:2 title '' with linespoints ,\
"1#st-up-s7" using 4:2 title '' with linespoints ,\
"1#st-up-s8" using 4:2 title '' with linespoints ,\
"1#st-up-s9" using 4:2 title '' with linespoints ,\
"1#st-up-s10" using 4:2 title '' with linespoints ,\
"1#st-up-s11" using 4:2 title '' with linespoints ,\
"1#st-up-s12" using 4:2 title '' with linespoints ,\
"1#st-up-s13" using 4:2 title '' with linespoints ,\
"1#st-up-s14" using 4:2 title '' with linespoints ,\

unset output
