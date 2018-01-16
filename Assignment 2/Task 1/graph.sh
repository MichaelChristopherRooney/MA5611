#!/usr/bin/gnuplot -persist
set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'fitness.eps'
set xlabel 'Time'
set ylabel 'Fitness [0-1]'
plot 'fitness.txt' using 1:2 with linespoints lt rgb 'black' title 'Fitness over time'
exit

