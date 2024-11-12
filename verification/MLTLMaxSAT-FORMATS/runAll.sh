#!/bin/bash
cd build
./benchmarker "../Benchmarks/MLTLSatisfiability/airspace_properties.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/contract_properties.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/extended_properties.mltl" 
./benchmarker "../Benchmarks/MLTLSatisfiability/nominal_properties.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/verification_properties.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/airspace_properties0.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/contract_properties0.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/extended_properties0.mltl" 
./benchmarker "../Benchmarks/MLTLSatisfiability/nominal_properties0.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/verification_properties0.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/airspace_properties00.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/contract_properties00.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/extended_properties00.mltl" 
./benchmarker "../Benchmarks/MLTLSatisfiability/nominal_properties00.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/verification_properties00.mltl"
./benchmarker "../Benchmarks/MLTLSatisfiability/randomList.mltl"
cd ../
./runner