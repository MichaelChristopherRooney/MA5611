#!/usr/bin/gnuplot -persist
set terminal postscript eps enhanced color font 'Helvetica,10'
set yrange [0:3200]
set output 'fitness.eps'
set xlabel 'Time'
set ylabel 'Fitness sum'
plot 'fitness.txt' using 1:2 with lines lt rgb 'green' title 'Fitness sum over time, 100 iterations, 100 population'
exit

