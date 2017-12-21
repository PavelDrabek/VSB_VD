set terminal pdfcairo enhanced color font "sans,5" fontscale 1.0 linewidth 1 rounded dashlength 1 background "white" size 10cm, 6cm
set output 'cv01.pdf'

set datafile separator ","
set xdata time
set timefmt "%Y-%m-%d"
set format x "%Y"
set yrange [-40:40]
set xrange ["1931-12-07":"2016-12-07"]

# definovani barvy os
set style line 80 linetype 1 linecolor "#232323" linewidth 1
set border 3 black linestyle 80

# grid
set style line 81 lt 0
set style line 81 lt rgb "#232323"
set style line 81 lw 0.5
set grid back linestyle 81

set xtics nomirror
set ytics nomirror

set title "Temperature Measurement Over Several Days" font "sans-Bold"
set xlabel "Time [day]"
set ylabel "Avg. temp. [°C]"

set style line 1 lc rgb '#2979FF' lt 1 lw 0.5 pt 2 ps 1   # --- blue
set style line 2 lc rgb '#FF0000' lt 1 lw 2 pt 2 ps 1   # --- trend

# trend
# y = -0.0000678486x + 4.6891537189
f(x) = a * x + b
fit f(x) 'data.txt' u 3:4 via a, b

# set title "Průměrné teploty v Los Angeles (1940 - 2017)"
# plot "dataset_la.csv" using 1:(($2 - 32) * 5.0/9.0) title "teploty" with lines

set title "Avg. temp. Berlin (1930 - 2017)"
plot "data.txt" using 1:2 title "Temps" with lines ls 1, f(x) title "Trend" with lines ls 2
#plot "dataset_be.csv" using 1:2 title "Temps" with lines ls 1 