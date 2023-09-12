#!/bin/bash

echo "WEST CORRECTNESS EXPERIMENT\n"

echo "Compiling the verifier."
g++ -std=c++17 utils.cpp reg.cpp grammar.cpp nnf_grammar.cpp verify_main.cpp -o verifier

cd ./../../MLTL_brute_forcer/Python/
echo "Brute-forcing depth 0 formulas"
python3 ./MLTL_truth_table.py formulas_d0.txt /brute_force_outputs_d0 1
cd ../../MLTL_reg/MLTL
echo "Verifying depth 0 outputs with WEST \n"
echo "n 1 ./verify/reg_outputs_d0 ./verify/brute_force_outputs_d0" | /usr/bin/time ./verifier
echo "Depth 0 formulas verified!\n"

cd ./../../MLTL_brute_forcer/Python/
echo "Brute-forcing depth 1 formulas"
python3 ./MLTL_truth_table.py formulas_d1.txt /brute_force_outputs_d1 2
cd ../../MLTL_reg/MLTL
echo "Verifying depth 1 outputs with WEST \n"
echo "n 2 ./verify/reg_outputs_d1 ./verify/brute_force_outputs_d1" | /usr/bin/time ./verifier
echo "Depth 1 formulas verified!\n"

cd ./../../MLTL_brute_forcer/Python/
echo "Brute-forcing depth 2 formulas. WARNING: RUN TIME ~9 HOURS"
python3 ./MLTL_truth_table.py formulas_d2.txt /brute_force_outputs_d2 4
cd ../../MLTL_reg/MLTL
echo "Verifying depth 2 outputs with WEST \n"
echo "n 4 ./verify/reg_outputs_d2 ./verify/brute_force_outputs_d2" | /usr/bin/time ./verifier
echo "Depth 1 formulas verified!\n"

echo "EXPERIMENT COMPLETED!"