#!/usr/bin/gnuplot -persist

set terminal postscript eps enhanced color font 'Helvetica,10'
set output 'gauss.eps'
set xlabel 'N'
set ylabel 'Time (microseconds)'

plot 'results/gauss_serial.txt' using 1:2 with linespoints lt rgb 'red' title 'Serial',\
'results/gauss_openmp.txt' using 1:2 with linespoints lt rgb 'blue' title 'OpenMP'

set output 'sieve.eps'
set logscale x
plot 'results/sieve_serial.txt' using 1:2 with linespoints lt rgb 'red' title 'Serial',\
'results/sieve_static.txt' using 1:2 with linespoints lt rgb 'blue' title 'OpenMP (static)',\
'results/sieve_dynamic.txt' using 1:2 with linespoints lt rgb 'green' title 'OpenMP (dynamic)',\
'results/sieve_synchronised.txt' using 1:2 with linespoints lt rgb 'black' title 'OpenMP (synchronised)'

exit

