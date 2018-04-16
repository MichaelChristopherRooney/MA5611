#!/usr/bin/gnuplot -persist

set terminal postscript eps enhanced color font 'Helvetica,10'
set xlabel 'N'
set ylabel 'Time in microseconds (log scale)'
set logscale y

set output 'tsp-fixed.eps'
plot 'data-fixed.txt' using 1:2 with linespoints lt rgb 'red' title 'Time vs N'

exit

