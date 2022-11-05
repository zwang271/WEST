### ARTIFACT LINK:
[download the .zip file from the GitHub repo.](https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-)

### ADDITIONAL REQUIREMENTS:
- Posix environment (Linux, MacOS, Etc.)
- Python3 (version 3.6 or greater)
- C99 std compiler (gcc was the one used, probably others might work as well)
- Make

### EXPERIMENT RUNTIME:
We ran all of our experiments on a computer with the following hardware specifications: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM.

1. BENCHMARKING EXPERIMENT RUNTIME: On this experiment we were interested in measuring the time it would take to compute the truth tables of formulas with different complexities.
  - Subset 1: 2 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input:
	- Output:
	- Runtime:

  - Subset 2: 1 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 20, Interval Max = 20
	- Input:
	- Output:
	- Runtime:

  - Subset 3: 2 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 5, Interval Max = 10
	- Input:
	- Output:
	- Runtime:

  - Subset 4: 1 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
	- Input:
	- Output:
	- Runtime:


2. CORECTNESS-EXPERIMENT RUNTIME: Depth two (at most double-nesting temporal operators) test suite of 1640 formulas; Number of propositional variables fixed at n = 4; Largest computation length fixed at m = 5.
	-Runtime: 30 minutes



### REPRODUCIBILITY INSTRUCTIONS:

- MAKING ARTIFACT:
  1. Go to the following [github repo:] (https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-)
  2. After downloading and cloning this repository, open a terminal session in the ?? directory and run the following commands to make the WEST program.
  ```
  $ cd 2022-Iowa-State-REU-Temporal-Logic-
  $ cd MLTL_reg
  $ cd MLTL
  $ make west
  $ ./west
  ```
  3. For further usage and output, see the repo homepage.
