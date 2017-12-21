set terminal pdfcairo enhanced color font "sans,7" fontscale 1.0 linewidth 1 rounded dashlength 1 background "white" size 36cm,12cm 
set encoding utf8

set output "cv_01_du.pdf"

#definovani barvy os
set style line 80 linetype 1 linecolor "#808080" linewidth 2 
set border 3 back linestyle 80

set xdata time
set timefmt "%Y%m%d"
set format x "%Y"
set yrange [-70:100]
set xrange ["19730101":"20170901"]

#mrizka
set style line 80 lt 0
set style line 80 lt rgb "0x00D3D3D3"
set style line 80 lw 0.5
set grid back linestyle 80

# set for [i=0:11] xtics ("1959", 2629746 * 48 * i, "2017") nomirror 
# set for [i=0:11] xtics ("1973", 19730101 + 100000 * i, "2017") nomirror 
# set xtics 100000000 nomirror 
set xtics "19370101", 365 * 24 * 60 * 60 * 2, "20170901" nomirror 
# set xtics ("19370101", "" "19500101", "20170901") nomirror 
set ytics nomirror

set title "Avg. temperature in Zyryanka" font "sans-Bold"
set xlabel "Time [year]"
set ylabel "Avg. temp. [°C]"

f(x) = a * x + b
fit f(x) 'data2.txt' u 1:2 via a, b

# data
set style line 1 lc rgb '#0F50F0' lt 1 lw 0.5 pt 2 ps 1   # --- blue
plot "data2.txt" using 1:2 title "Temperature" with lines ls 1, f(x) title "Trend" with lines ls 2