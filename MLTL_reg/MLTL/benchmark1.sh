#!/bin/bash
echo "WEST BENCHMARKING EXPERIMENTS \n"

echo "n ./complexity_graph/random_mltl1.txt 5 ./complexity_graph/complexities1.txt" | /usr/bin/time ./benchmark_west
echo "-------- Subset 1/4 done --------\n"

echo "n ./complexity_graph/random_mltl2.txt 10 ./complexity_graph/complexities2.txt" | /usr/bin/time ./benchmark_west
echo "-------- Subset 2/4 done --------\n"

echo "n ./complexity_graph/random_mltl3.txt 10 ./complexity_graph/complexities3.txt" | /usr/bin/time ./benchmark_west
echo "-------- Subset 3/4 done --------\n"

echo "n ./complexity_graph/random_mltl4.txt 10 ./complexity_graph/complexities4.txt" | /usr/bin/time ./benchmark_west
echo "-------- Subset 4/4 done --------\n"

echo "RESULTS READY TO PRODUCE THE PLOTS \n Bye-bye!"
