# MLTL Interpretor

The MLTL Interpreter assesses whether a given trace satisfies an MLTL (Metric Temporal Logic) formula.

To compile, execute make all. This builds two executables:
* 'interpret': Evaluates a single trace against a formula.
* 'interpret_batch': Evaluates multiple traces against the same formula.

Usage for both executables is as follows:
```
interpret [formula file] [trace file] [output file]

interpret_batch [formula file] [traces file] [output file]
```

## Formula File
The formula file must adhere to standard MLTL syntax. If the file contains multiple formula lines, they are combined using a conjunction (AND) operator. For example:
```
G[0,2] p0
F[2,10] p1
p0 R[3, 8] p1
```
is treated as ```(G[0,2] p0 & F[2,10] p1 & p0 R[3, 8] p1)```.

## Trace File 
The format for trace files varies slightly depending on whether it's for a single trace ('interpret') or multiple traces ('interpret_batch').

#### Single Trace
In a single trace file, each line represents the state of boolean variables at a specific timestep.

For instance, consider a trace with three boolean variables (p0, p1, p2):
* At timestep 0, all variables are true: 111
* At timestep 1, all variables are true: 000
* At timestep 2, p0 and p2 are false, p1 is true: 010

Example file content:
```
111
000
010
```

#### Multiple Traces
In a multiple traces file, each line corresponds to a different trace. Here, boolean variables at each timestep are separated by commas.

For example:
```
111,000,010
101,001,001,100
110,001
...
```

Note: The number of boolean variables per timestep should remain constant across a trace, but the length of different traces can vary.

## Output File
'interpret' outputs 1 to the file if the input trace satisfies the formula, otherwise 0.

'interpret_batch' writes the evaluation result (1 or 0) for each trace on separate lines in the output file.



