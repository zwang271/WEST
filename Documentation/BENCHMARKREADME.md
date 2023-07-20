### ARTIFACT LINK:
[download the .zip file from the GitHub repo.](https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-)

### ADDITIONAL REQUIREMENTS:
- Posix environment (Linux, MacOS, Etc.)
- Python3 (version 3.6 or greater)
- C99 std compiler (gcc was the one used, others will likely work as well)
- Make

### SPECIFICATIONS:
We ran all of our experiments on a computer with the following hardware specifications: Intel(R) Xeon(R) Gold 6140 CPU @ 2.30GHz with 376 GB RAM.

1. WEST BENCHMARKING EXPERIMENTS: 
In these experiments we measure the time it takes to compute the truth tables of formulas with different complexities (input formula length) and their output lengths. The generated random MLTL formulas and their measured complexities are provided in the input/output files for each experiment below. Please take care to not overwrite the provided output files.
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/`
  - Run the script `make benchmark_west`
  - Then run `./benchmark_west`
  
  - Subset 1: 2 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input: `random_mltl1.txt` in `./MLTL_reg/MLTL/complexity_graph`; use the option `n` to generate formulas;
	`NUM_PROP_VARS = 5`
	- Output: `complexities1.txt` in `./MLTL_reg/MLTL/complexity_graph`
	- Runtime: approximately 3 minutes

  - Subset 2: 1 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 20, Interval Max = 20
	- Input: `random_mltl2.txt` in `./MLTL_reg/MLTL/complexity_graph`; use the option `n` to generate formulas;
	`NUM_PROP_VARS = 10`
	- Output: `complexities2.txt` in `./MLTL_reg/MLTL/complexity_graph`
	- Runtime: less than a second

  - Subset 3: 2 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 5, Interval Max = 10
	- Input: `random_mltl3.txt` in `./MLTL_reg/MLTL/complexity_graph`; put `n` to generate formulas;
	`NUM_PROP_VARS = 10`
	- Output: `complexities3.txt` in `./MLTL_reg/MLTL/complexity_graph`
	- Runtime: Approximately 1 minute

  - Subset 4: 1 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input: `random_mltl4.txt` in `./MLTL_reg/MLTL/complexity_graph`; `n` to generate formulas;
	`NUM_PROP_VARS = 5`
	- Output: `complexities4.txt` in `./MLTL_reg/MLTL/complexity_graph`
	- Runtime: less than a second

  We generated the random MLTL formulas in the random_mltl files using by inputting `y` to generate
  formulas and entering the respective parameters for each experiment. To generate the graphs seen in
  section 4.3 of the paper, 
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/complexity graph/`
  - Run `python3 complexity_graphWEST.py <experiment number> <plot_length>`
  Experiment number should be one of {1, 2, 3, 4}
  Set `plot_length = 1` to plot length vs length
  Set `plot_length = 0` to plot length vs time
 
2. REST BENCHMARKING EXPERIMENT:
 In these experiments we measure the time it takes to run REST for sets of randomly generated regular expressions satisfying the conditions of REST. Please take care to not overwrite the provided output file.
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/`
  - Build using `make benchmark_rest`
  - Then run it with `./benchmark_rest`
  
  -Subset 1: min_size = 10, variation = 15
 	- Input: `min_size = 10`; `variation = 15`
	- Output: `RESTcomplexities.txt` in `.MLTL_reg/MLTL/complexity_graph`
	- Runtime: Approximately an hour
	
 To generate the graphs seen in section 6.1 of the paper, 
  - From the WEST tool root directory, `cd .MLTL_reg/MLTL/complexity graph/`
  - Run `python3 complexity_graphREST.py`

3. WEST CORRECTNESS EXPERIMENT: 
  The generated random MLTL formulas and their measured complexities are provided in the input
  output files for each experiment below. Please take care not to overwrite the provided output files.

  - Brute force solution outputs
  	- From the WEST tool root directory, `cd .MLTL_brute_forcer/Python/`
	- Terminal Command: `python3 <formulas file> <file to write outputs to> <number of propositional
	variables = 2^depth>`
	- Depth 0: `python3 .\MLTL_truth_table.py formulas_d0.txt /brute_force_outputs_d0 1`
	- Depth 1: `python3 .\MLTL_truth_table.py formulas_d1.txt /brute_force_outputs_d1 2`
	- Depth 2: `python3 .\MLTL_truth_table.py formulas_d2.txt /brute_force_outputs_d2 4`
	- Runtime: ~9 hours for all depths

  - Verifying reg with brute force outputs
	- From the WEST tool root directory `cd .MLTL_reg/MLTL/`
	- Terminal Command to build: `g++ -std=c++17 utils.cpp reg.cpp grammar.cpp nnf_grammar.cpp
	verify_main.cpp -o verify`
	- To run: `./verify`
	- Enter `n` to generate new formulas (all formulas are provided, don't overwrite!)
	- Depth 0: depth is 0, write expanded outputs to `./verify/reg_outputs_d0/`, brute force outputs is
	`.verify/brute_force_outputs_d0/`
	- Depth 1: depth is 1, write expanded outputs to `./verify/reg_outputs_d1/`, brute force outputs is 
	`./verify/brute_force_outputs_d1/`
	- Depth 2: depth is 2, write expanded outputs to `./verify/reg_outputs_d2/`, brute force outputs is 
	`./verify/brute_force_outputs_d2/`
	- Runtime: ~30 minutes for all depths
	


### REPRODUCIBILITY INSTRUCTIONS:

MAKING ARTIFACT:
  1. Download the .zip file containing the source code (https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-).
  2. Open a terminal session at the directory where the folder containing the source code files 
  and run the following commands to make the WEST program.
  ```
  $ cd 2022-Iowa-State-REU-Temporal-Logic-
  $ cd MLTL_reg
  $ cd MLTL
  $ make west_lib
  $ python gui.py
  ```
  For further usage and output, see the repo homepage or GUIREADME.md in the Documentation folder.
