#!/bin/bash

echo "WEST CORRECTNESS EXPERIMENT"

cd ./MLTL_brute_forcer/Python/
echo "Brute-forcing..."
python3 ./MLTL_truth_table.py formulas_d0.txt /brute_force_outputs_d0 1
python3 ./MLTL_truth_table.py formulas_d1.txt /brute_force_outputs_d1 2
python3 ./MLTL_truth_table.py formulas_d2.txt /brute_force_outputs_d2 4
echo "Done! Now, it is time to verify the correctness of WEST."

cd ../../MLTL_reg/MLTL
echo "Compiling the verifier."
g++ -std=c++17 utils.cpp reg.cpp grammar.cpp nnf_grammar.cpp verify_main.cpp -o verifier

echo "n 1 ./verify/reg_outputs_d0 ./verify/brute_force_outputs_d0" | /usr/bin/time ./verifier
echo "Depth 0 formulas verified!"

echo "n 2 ./verify/reg_outputs_d1 ./verify/brute_force_outputs_d1" | /usr/bin/time ./verifier
echo "Depth 1 formulas verified!"

echo "n 4 ./verify/reg_outputs_d2 ./verify/brute_force_outputs_d2" | /usr/bin/time ./verifier
echo "Depth 2 formulas verified!"

echo "EXPERIMENT COMPLETED!"
