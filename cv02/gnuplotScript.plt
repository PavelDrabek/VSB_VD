set terminal pdfcairo enhanced color font "sans,6" fontscale 1.0 linewidth 1 rounded dashlength 1 background "white" size 20cm,12cm 
set encoding utf8

set output "cv_01c.pdf"

#definovani barvy os
set style line 80 linetype 1 linecolor "#808080" linewidth 2 
set border 3 back linestyle 80

set timefmt "%Y%m%d"
set xdata time
set format x "%Y"

#mrizka
set style line 81 lt 0
set style line 81 lt rgb "0x00D3D3D3"
set style line 81 lw 0.5
set grid back linestyle 81



set for [i=0:5] xtics ("1959", 2629746 * 100 * i, "2017") nomirror 
set ytics nomirror

set title "Temperature Measurement Over Several Days" font "sans-Bold"
set xlabel "Time [year]"
set ylabel "Avg. temp. [°C]"

#set key bottom left Left title "Probes:" enhanced font "sans-Italic" #reverse box



set style line 1 lt rgb "#A00000" lw 2 pt 1

plot "data.txt" using 3:4 title "Daily temperatures in Zyryanka" with lines