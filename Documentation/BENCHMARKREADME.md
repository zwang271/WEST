### ARTIFACT LINK:
[download the .zip file from the GitHub repo.](https://github.com/zwang271/WEST)

### ADDITIONAL REQUIREMENTS:
- Posix environment (Linux, MacOS, Etc.)
- Python3 (version 3.6 or greater)
- C99 std compiler (gcc was the one used, others will likely work as well)
- Make

### SPECIFICATIONS:
We ran all of our experiments on a computer with the following hardware specifications: Intel(R) Xeon(R) Gold 6140 CPU @ 2.30GHz with 376 GB RAM.
All of these experiments could be run on a standard laptop in a reasonable time frame. Please allow additional runtime.

### 1. WEST BENCHMARKING EXPERIMENTS: 
In these experiments we measure the time it takes to compute the truth tables of formulas with different complexities (input formula length) and their output lengths. The generated random MLTL formulas and their measured complexities are provided in the input/output files for each experiment below. Please take care to not overwrite the provided input/output files.
  - From root directory, `cd ./MLTL_reg/MLTL/`
  - Run `make benchmark_west`
  
  - **Option A (Recommended): Run bash script**
  	- run `sh benchmark1.sh`
	
  - Option B (Alternative): Run each experiment individually
	- Subset 1: 2 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
		- Run `./benchmark_west`
		- Inputs
			- `n` to "Generate formulas? (y/n)"
			- `./complexity_graph/random_mltl1.txt` to "Enter pathname of formula file"
			- `5` to "NUM_PROP_VARS:"
			- `./complexity_graph/complexities1.txt` to "Enter name of output file"
		- Runtime: approximately 3 minutes

	- Subset 2: 1 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 20, Interval Max = 20
		- Run `./benchmark_west`
		- Inputs
			- `n` to "Generate formulas? (y/n)"
			- `./complexity_graph/random_mltl2.txt` to "Enter pathname of formula file"
			- `10` to "NUM_PROP_VARS:"
			- `./complexity_graph/complexities2.txt` to "Enter name of output file"
		- Runtime: several seconds

	- Subset 3: 2 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 5, Interval Max = 10
		- Run `./benchmark_west`
		- Inputs
			- `n` to "Generate formulas? (y/n)"
			- `./complexity_graph/random_mltl3.txt` to "Enter pathname of formula file"
			- `10` to "NUM_PROP_VARS:"
			- `./complexity_graph/complexities3.txt` to "Enter name of output file"
		- Runtime: Approximately 1 minute

	- Subset 4: 1 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
		- Run `./benchmark_west`
		- Inputs
			- `n` to "Generate formulas? (y/n)"
			- `./complexity_graph/random_mltl4.txt` to "Enter pathname of formula file"
			- `10` to "NUM_PROP_VARS:"
			- `./complexity_graph/complexities4.txt` to "Enter name of output file"
		- Runtime: less than a second

  **Generating the graphs seen in section 4.3 of the paper**
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/complexity graph/`
  - Run each of the following commands
	- NOTE: if any errors arise involving Qt or PyQt5, please run `sudo apt install qt5dxcb-plugin`
  	- `python3 complexity_graphWEST.py 1 0`
	- `python3 complexity_graphWEST.py 1 1`
	- `python3 complexity_graphWEST.py 2 0`
	- `python3 complexity_graphWEST.py 2 1`
	- `python3 complexity_graphWEST.py 3 0`
	- `python3 complexity_graphWEST.py 3 1`
	- `python3 complexity_graphWEST.py 4 0`
	- `python3 complexity_graphWEST.py 4 1`
 
### 2. REST BENCHMARKING EXPERIMENT:
 In these experiments we measure the time it takes to run REST for sets of randomly generated regular expressions satisfying the conditions of REST.
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/`
  - Build using `make benchmark_rest`
  - run `sh benchmark2.sh`
  - Runtime: Approximately an hour
  - Note: it can take a minute or two for progress update to be printed to terminal
	
 To generate the graphs seen in section 6.1 of the paper, 
  - From the WEST tool root directory, `cd ./MLTL_reg/MLTL/complexity_graph/`
  - Run `python3 complexity_graphREST.py`

### 3. WEST CORRECTNESS EXPERIMENT: 
  This experiment verifies the correctness of the WEST tool by comparing the outputs of a bruteforce Python program and the outputs of WEST on an indentical set of formulas. 
  - Run `sh benchmark3.sh`
	- Bruteforcing portion of the script will take ~9 hours
	- Verifying using WEST should take ~30 minutes for depths 0, 1, and 2


### Running the User Interface:

Run the following commands to make the WEST program.
  ```
  $ cd WEST/MLTL_reg/MLTL
  $ make west_lib
  $ python gui.py
  ```
- Note that various Python packages may need to be installed (through pip), such as Lark, dd, PyQt5, etc.
- Enter the formula `((p0 & G[0,3] p1) -> p2)` in the textbox to visualize the example mentioned in section 7
- Press the `Run` button
- Now clicking on each subformula button will show a pop-up window to visualize it
- See https://youtu.be/HoBJwdCq42c for more detailed explanation

