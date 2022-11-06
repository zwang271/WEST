### ARTIFACT LINK:
[download the .zip file from the GitHub repo.](https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-)

### ADDITIONAL REQUIREMENTS:
- Posix environment (Linux, MacOS, Etc.)
- Python3 (version 3.6 or greater)
- C99 std compiler (gcc was the one used, probably others might work as well)
- Make

### SPECIFICATIONS:
We ran all of our experiments on a computer with the following hardware specifications: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM.

1. BENCHMARKING EXPERIMENTS: In these experiments we were interested in measuring the time it would take to compute the truth tables of formulas with different complexities and their output lengths.
  The generated random MLTL formulas and their measured complexities are provided in the input/
  output files for each experiment below. Please take care to not overwrite the provided output files.
  
  - Terminal Command to build: g++ -std=c++17 utils.cpp reg.cpp grammar.cpp nnf_grammar.cpp
  simulation_main.cpp -o simulations
  - To run: ./simulations
  
  - Subset 1: 2 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input: random_mltl1.txt in .MLTL_reg/MLTL/complexity_graph; 'n' to generate formulas
	- Output: complexities1.txt in .MLTL_reg/MLTL/complexity_graph
	- Runtime: approximately 30 minutes

  - Subset 2: 1 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 20, Interval Max = 20
	- Input: random_mltl2.txt in .MLTL_reg/MLTL/complexity_graph; 'n' to generate formulas
	- Output: complexities2.txt in .MLTL_reg/MLTL/complexity_graph
	- Runtime: approximately 45 seconds

  - Subset 3: 2 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 5, Interval Max = 10
	- Input: random_mltl3.txt in .MLTL_reg/MLTL/complexity_graph; 'n' to generate formulas
	- Output: complexities3.txt in .MLTL_reg/MLTL/complexity_graph
	- Runtime: approximately 15 minutes

  - Subset 4: 1 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input: random_mltl4.txt in .MLTL_reg/MLTL/complexity_graph; 'n' to generate formulas
	- Output: complexities4.txt in .MLTL_reg/MLTL/complexity_graph
	- Runtime: approximately 1-2 seconds

  We generated the random MLTL formulas in the random_mltl files using by inputting 'y' to generate
  formulas and entering the respective parameters for each experiment. To generate the graphs seen in
  section 4.3 of the paper, 
  - cd to .MLTL_reg/MLTL/complexity graph/
  - Run python3 complexity_graph.py <experiment number> <plot_length>
  Set plot_length = 1 to plot length vs length
  Set plot_length = 0 to plot length vs time

2. CORRECTNESS EXPERIMENT: 
  The generated random MLTL formulas and their measured complexities are provided in the input
  output files for each experiment below. Please take care not to overwrite the provided output files.

  - Brute force solution outputs
  	- cd into MLTL_brute_forcer/Python/
	- Terminal Command: python3 <formulas file> <file to write outputs to> <number of propositional
	variables = 2^depth>
	- Depth 0: python3 .\MLTL_truth_table.py formulas_d0.txt /brute_force_outputs_d0 1
	- Depth 1: python3 .\MLTL_truth_table.py formulas_d1.txt /brute_force_outputs_d1 2
	- Depth 2: python3 .\MLTL_truth_table.py formulas_d2.txt /brute_force_outputs_d2 4
	- Runtime: approximately 9 hours

  - Verifying reg with brute force outputs
	- cd into MLTL_reg/MLTL/
	- Terminal Command to build: g++ -std=c++17 utils.cpp reg.cpp grammar.cpp nnf_grammar.cpp
	verify_main.cpp -o verify
	- To run: ./verify
	- Enter 'n' to generate new formulas (all formulas are provided)
	- Depth 0: depth is 0, write expanded outputs to './verify/reg_outputs_d0/', brute force outputs is
	'.verify/brute_force_outputs_d0/'
	- Depth 1: depth is 1, write expanded outputs to './verify/reg_outputs_d1/', brute force outputs is 
	'./verify/brute_force_outputs_d1/'
	- Depth 2: depth is 2, write expanded outputs to './verify/reg_outputs_d2/', brute force outputs is 
	'./verify/brute_force_outputs_d2/'
	- Runtime: 30 minutes
	


### REPRODUCIBILITY INSTRUCTIONS:

MAKING ARTIFACT:
  1. Go to the following [github repo:] (https://github.com/zwang271/2022-Iowa-State-REU-Temporal-
  Logic-)
  2. After downloading and cloning this repository, open a terminal session and run the following
  commands to make the WEST program.
  ```
  $ cd 2022-Iowa-State-REU-Temporal-Logic-
  $ cd MLTL_reg
  $ cd MLTL
  $ make west
  $ ./west
  ```
  For further usage and output, see the repo homepage or WESTREADME.md in the Documentation
  folder.
