#!/usr/bin/gnuplot -persist

set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'tsp.eps'
set xlabel 'N'
set ylabel 'Time in microseconds (log scale)'
set logscale y

plot 'data.txt' using 1:2 with linespoints lt rgb 'red' title 'Time vs N'

exit

