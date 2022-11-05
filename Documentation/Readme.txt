ARTIFACT LINK: https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-

ADDITIONAL REQUIREMENTS:
- Posix environment (Linux, MacOS, Etc.)
- Python3 (version 3.6 or greater)
- C99 std compiler (gcc was the one used, probably others might work as well)
- Make

CORECTNESS-EXPERIMENT RUNTIME: 

	FULL EXPERIMENT:
		TEST SUITE: Depth two (at most double-nesting temporal operators) test suite of 1640 formulas;
			    Number of propositional variables fixed at n = 4; 
			    Largest computation length fixed at m = 5
		
		HARDWARE: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM

		RUNTIME: 30 minutes		  

	SUBSET OF EXPERIMENT: ???

SIMULATION_1-EXPERIMENT RUNTIME: 

	FULL EXPERIMENT:
		TEST SUITE: 2 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
		
		HARDWARE: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM

		RUNTIME: ???		  

	SUBSET OF EXPERIMENT: ???

SIMULATION_2-EXPERIMENT RUNTIME: 

	FULL EXPERIMENT:
		TEST SUITE: 1 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 20, Interval Max = 20
		
		HARDWARE: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM

		RUNTIME: ???		  

	SUBSET OF EXPERIMENT: ???

SIMULATION_3-EXPERIMENT RUNTIME: 

	FULL EXPERIMENT:
		TEST SUITE: 2 Iterations (Depth), 10 Prop vars, Delta (Max interval length) = 5, Interval Max = 10
		
		HARDWARE: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM

		RUNTIME: ???		  

	SUBSET OF EXPERIMENT: ???

SIMULATION_4-EXPERIMENT RUNTIME: 

	FULL EXPERIMENT:
		TEST SUITE: 1 Iterations (Depth), 5 Prop vars, Delta (Max interval length) = 10, Interval Max = 10
		
		HARDWARE: Intel(R) Core(TM) i7-4770S CPU at 3.10GHz with 32gb RAM

		RUNTIME: ???		  

	SUBSET OF EXPERIMENT: ???

REPRODUCIBILITY INSTRUCTIONS:

	MAKING ARTIFACT:
		After downloading and cloning this repository, use the commands

		$ cd 2022-Iowa-State-REU-Temporal-Logic-
		$ cd MLTL_reg
		$ cd MLTL
		$ make west
		$ ./west

		to make the WEST program.
		For further usage and output, see: https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-

	RUNNING CORRECTNESS-EXPERIMENT: ???

	RUNNING SIMULATION_1: ???

	RUNNING SIMULATION_2: ???

	RUNNING SIMULATION_3: ???

	RUNNING SIMULATION_4: ??? 
