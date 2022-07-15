# WEST mLTL Truth Table Generator

The WEST mLTL Truth Table Generator is a program that parses well-formed mLTL formulas and outputs the structure of the computations of these formulas and their substrings. Our recursive algorithm is based on the regular expressions of mLTL operators and runs with approximately doubly exponential space and time complexity.

## Usage

After downloading and cloning this repository, use the commands
``` 
$ cd 2022-Iowa-State-REU-Temporal-Logic-
$ cd MLTL_reg
$ cd MLTL
$ make 
$ ./west
```
Then, the user will be prompted:
```
Please enter a MLTL formula.
```
See the Grammar section below to inform input


## Grammar
Whitespace for formula input is unrestricted.
Non-empty intervals are recommended for meaningful truth table generation.

### Propositional Variables and Constants
**True:** T <br />
**False:** ! <br />
**First Variable:** p0 <br />
**Second Variable:** p1 <br />
And so on, where each consecutive variable is followed with the appropriate natural number. <br />

Let K be a well-formed formula, propositional variable, or propositional constant. <br />
**Negation:** ~ K <br />
Formulas do not necessarily need to be in negation normal form, as our algorithm converts formulas into this form and generates the truth table for the formula's translated syntax. <br />

Note: after inputting a well-formed mLTL formula, the user will be prompted:
```
Please enter number of propositional variables.
```
Where the user will enter an appropriate integer. This number must be at least as large as the number of propositional variables defined in the formula. If desired, the user may input a formula that does not necessarily include p0, p1, etc. For example, a formula may exclusively use variable p2, and computations will be generated to reflect that there are no restrictions on the first and second variables (p0 and p1). 

If exclusively propositional constants are inputted, then the user should enter "1" for the number of propositional variables in order to generate a meaningful truth table. 

### Unary Temporal Connectives
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive upper bound of an interval, and let b be inclusive lower bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.  <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />

**Finally:** F[a:b] K <br />
**Globally:** G[a:b] K <br />

### Binary Propositional Connectives
All binary connectives must be enclosed with parentheses. <br />
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (K & L) <br />
**Or:** (K v L)  <br />
**Equivalence:** (K = L)  <br />
**Implies:** (K > L)  <br />

### Associative Propositional Connectives
The entirety of the associative propositional connective formula string must be enclosed in parentheses.  <br />
The list of elements must be preceded by the associative propositional connective. <br />
Let "," separate each element in the list, and let "[" and "]" indicate the beginning and end of the list, respectively. <br />
Let K, L, ..., M be an arbitrarily-sized list of well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (&[K, L, ..., M]) <br />
**Or:** (v[K, L, ..., M])  <br />
**Equivalence:** (=[K, L, ..., M])  <br />
**Implies:** (>[K, L, ..., M])  <br />

### Binary Temporal Connectives
All binary connectives must be enclosed with parentheses. <br />
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive upper bound of an interval, and let b be inclusive lower bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**Until:** (K U[a:b] L) <br />
**Release** (K R[a:b] L) <br />

## Computations
In a computation, "1" represents a true truth value, and "0" represents a false truth value; "s" represents an arbitrary truth value, i.e. true or false.
Time steps in a computation are separated by commas. The bit-strings at each time step represent the truth values of each propositional variable. <br />

**Examples:** <br />
The computation of a formula with one propositional variable that is true for 5 times steps is represented as:
```
NNF Formula: G[0:4]p0

1,1,1,1,1
```
The computation of a formula with 5 propositional variables that are true for one time step is represented as:
```
NNF Formula: G[0:0](&[p0,p1,p2,p3,p4])

11111
````
The computations of a formula with two propositional variables that are both eventually true at a third time step are represented as:
```
NNF Formula: F[0:2](p0&p1)

11,ss,ss
ss,11,ss
ss,ss,11
```
Where in each string separated by commas, the first digit represents the truth value of p0, and the second digit represents the truth value of p1.

## Example
An interesting mLTL formula to consider is that of the oscillation of the truth value of a propositional variable for each time step.
Here is how a user may generate the truth table for this formula:
```
Please enter a MLTL formula.
$ G[0:2] (&[(p0 > G[1:1]~p0), (~p0 > G[1:1]p0)])
Please enter number of propositional variables.
$ 1
Would you like to generate the truth table? (y / n)
$ y
NNF Formula: G[0:2](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])

Subformula: p0
1

Subformula: ~p0
0

Subformula: G[1:1]~p0
s,0

Subformula: (p0>G[1:1]~p0)
0,s
s,0

Subformula: G[1:1]p0
s,1

Subformula: (~p0>G[1:1]p0)
1,s
s,1

Subformula: (&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])
0,1
1,0

G[0:2](&[(p0>G[1:1]~p0),(~p0>G[1:1]p0)])
0,1,0,1
1,0,1,0

Finished computing.
Size of vector: 2
Number of characters: 14
```



## Contributors
This project is part of the 2022 Iowa State REU with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and Laura Gamboa Guzmán

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset

INSERT LINK TO OUR PAPER HERE




