# WEST mLTL Truth Table Generator

The WEST mLTL Truth Table Generator is a program that parses well-formed mLTL formulas and outputs the regular expression
for the computations that satisfy the formula.
Our recursive algorithm is based on the regular expressions of mLTL operators and runs with approximately doubly exponential space and time complexity.

## Usage

After downloading and cloning this repository, use the commands

``` 
$ cd 2022-Iowa-State-REU-Temporal-Logic-
$ cd MLTL_reg
$ cd MLTL
$ make west
$ ./west
```

Then, the user will be prompted:

```
Please enter a MLTL formula.
```

See the Grammar section below for inputing mLTL formulas.

If the user enters a string that does not parse the grammar,
west will prompt the user to enter in another string:

```
Please enter a MLTL formula.
asdasd
Not a well formed formula!
Please enter a MLTL formula.
```

If the input string parses the grammar, West will prompt the user whether they wish to
simplify the output of reg:

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
```

Answering yes will apply the simplify() heuristic function on the output of West. <br />
This will lead to a nicer regular expression output, but at the expense of longer computation time. <br />
Answering yes to simplify:
<br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
y
Would you like to generate the truth table? (y / n)
n
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

s,s,s

Finished computing.
Size of vector: 1
Number of characters: 5
Please enter a MLTL formula.

```

<br />
Answering no to simplify:
<br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
n
Would you like to generate the truth table? (y / n)
n
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

s,1
s,0
s,s,1
s,s,0

Finished computing.
Size of vector: 4
Number of characters: 16
Please enter a MLTL formula.

```

<br />
<br />

After prompting the user whether they wish to simplify the output, West will prompt the user
whether they wish to print the full recursive truth-table or not:

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
n
Would you like to generate the truth table? (y / n)
```

<br />
Answering yes will print out the regular expression for every subformula of the input formula,
while no will simply output the regular expression for the input formula. <br />
Note that if simplify is turned on, West will simply the output for every subformula
as-well-as the output for the input formula. <br />
This is shown for the output of the following input formula: "((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))".<br />
<br />
No to simplify, No to truth table: <br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
n
Would you like to generate the truth table? (y / n)
n
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

s,1
s,0
s,s,1
s,s,0

Finished computing.
Size of vector: 4
Number of characters: 16
Please enter a MLTL formula.

```

<br />

<br />
Yes to simplify, No to truth table: <br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
y
Would you like to generate the truth table? (y / n)
n
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

s,s,s

Finished computing.
Size of vector: 1
Number of characters: 5
Please enter a MLTL formula.

```

<br />

<br />
No to simplify, Yes to truth table: <br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
n
Would you like to generate the truth table? (y / n)
y
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

Subformula: p0
1

Subformula: G[1:1]p0
s,1

Subformula: ~p0
0

Subformula: G[1:1]~p0
s,0

Subformula: (G[1:1]p0vG[1:1]~p0)
s,1
s,0

Subformula: G[2:2]p0
s,s,1

Subformula: G[2:2]~p0
s,s,0

Subformula: (G[2:2]p0vG[2:2]~p0)
s,s,1
s,s,0

((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))
s,1
s,0
s,s,1
s,s,0

Finished computing.
Size of vector: 4
Number of characters: 16
Please enter a MLTL formula.

```

<br />

<br />
Yes to simplify, Yes to truth table: <br />

```
Please enter a MLTL formula.
((G [1:1] p0 v G [1:1] ~p0) v (G [2:2] p0 v G [2:2] ~p0))
Would you like to simplify output of reg? (y / n)
y
Would you like to generate the truth table? (y / n)
y
NNF Formula: ((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))

Subformula: p0
1

Subformula: G[1:1]p0
s,1

Subformula: ~p0
0

Subformula: G[1:1]~p0
s,0

Subformula: (G[1:1]p0vG[1:1]~p0)
s,s

Subformula: G[2:2]p0
s,s,1

Subformula: G[2:2]~p0
s,s,0

Subformula: (G[2:2]p0vG[2:2]~p0)
s,s,s

((G[1:1]p0vG[1:1]~p0)v(G[2:2]p0vG[2:2]~p0))
s,s,s

Finished computing.
Size of vector: 1
Number of characters: 5
Please enter a MLTL formula.

```

<br />



## Grammar

For inputting a mLTL formula, the user must input a string that matches the following Context-Free Grammar:
```
Context-Free Grammar for a MLTL wff.

'True', 'False', 'Negation', 'Or', 'And', 'If and only if', and 'Implies' are represented by the symbols:
    'T', '!', '~', 'v', '&', '=', and '>' respectively.
    
‘Eventually’, ‘Always’, ‘Until’, and ‘Weak Until’ are represented by the symbols:
    ‘F’, ‘G’, ‘U’, and ‘W’ respectively.


Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ‘:’, ‘,’ ,
                       ‘T’, ‘!’,                
                       ‘~’, ‘v’, ‘&’, ‘=’, ‘>’, 
                       ‘F’, ‘G’, ‘U’, ‘R’ }


Digit         ->  ‘0’ | ‘1’ | … |’9’
Num           ->  Digit Num |  Digit
Interval      ->  ‘[’  Num ‘:’ Num ‘]’  
Prop_var      ->  ‘p’ Num

Prop_cons         ->  ‘T’ | ‘!’
Unary_Prop_conn   ->  ‘~’
Binary_Prop_conn  ->  ‘v’ | ‘&’ | ‘=’ | ‘>’

Assoc_Prop_conn   -> ‘v’ | ‘&’ | ‘=’
Array_entry       -> Wff ‘,’ Array_entry  |  Wff 

Unary_Temp_conn   ->  ‘F’ | ‘G’
Binary_Temp_conn  ->  ‘U’ | ‘R’


Wff   ->      Prop_var | Prop_cons
                       | Unary_Prop_conn Wff
	                     | Unary_Temp_conn  Interval  Wff
	            
                       | ‘(‘ Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ‘)’
                       | ‘(‘ Wff Binary_Prop_conn Wff ‘)’
                       | ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)    

```

To illustrate this grammar, here are 3 well-formed strings for each case of Wff:

## Prop_var:
Regular Propositional variable
```
p0
p1
p2
```

## Prop_cons:
Propositional constants 'True' and 'False'
```
T
!
```

## Unary_Prop_conn Wff:
Negation of a mLTL wff
```
~ (p2 R[1:2] p3)
~ (p3 R[9:9] p2)
~ ~p4
```

## Unary_Temp_conn Interval Wff:
One of the Temporal operators 'Finally' or 'Always', followed by an interval,
followed by another mLTL wff
```
G [7:9] F[4:5]p4
F [1:9] ~p4
F [4:5] (p3>p2)
```

## ‘(‘ Assoc_Prop_conn ‘[‘  Array_entry  ‘]’ ‘)’:
Given an associative propositional connective, such as 'v', this gives
an alternate array-notation to avoid un-needed parentheses. <br />
For instance, instead of entering "((p0 v p1) v p2)", one can instead
enter: "(v[p0, p1, p2])"
```
 ( v [ p0, p1, p2 ] )
 ( & [ F[1:9]~p4, ~~p4, ~p2 ] )
 ( = [ p1,  G[7:9]F[4:5]p4, p5] )
```

##  ‘(‘ Wff Binary_Prop_conn Wff ‘)’:
One of the binary propositional connectives: 'v', '&', '=', '>' <br />
in-between two mLTL formulas
```
 ( G[8:9]p3 = (p1=p4) )
 ( (p1>p2) & ~p1 )
 ( G[7:9]p0 v F[9:9]p3 )
```

## ‘(‘ Wff Binary_Temp_conn  Interval Wff ‘)':
One of the binary temporal connectives: 'U', 'R' <br />
followed by an interval and in-between two mLTL formulas
```  
( G[9:9]p0 U [7:8] G[2:8]p4 )
( (p4>p1) U [5:5] (p4=p0) )
( (p3>p3) R [5:9] (p1vp4) )
```
<br />
Note that for this grammar, whitespaces do not matter as west will first strip the input-string
of all whitespaces before parsing it.
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />
<br />






































# WEST mLTL Truth Table Generator

The WEST mLTL Truth Table Generator is a program that parses well-formed mLTL formulas and outputs the structure of the computations of these formulas and their substrings. Our recursive algorithm is based on the regular expressions of mLTL operators and runs with approximately doubly exponential space and time complexity.

## Usage

After downloading and cloning this repository, use the commands
``` 
$ cd 2022-Iowa-State-REU-Temporal-Logic-
$ cd MLTL_reg
$ cd MLTL
$ make west
$ ./west
```
Then, the user will be prompted:
```
Please enter a MLTL formula.
```
See the Grammar section below to inform input.


## Grammar
The WEST program strips whitespaces from input.
Non-empty intervals are recommended for meaningful truth table generation.

### **Propositional Variables and Constants**
**True:** T <br />
**False:** ! <br />
**First Variable:** p0 <br />
**Second Variable:** p1 <br />
And so on, where each consecutive variable is followed with the appropriate natural number. <br />

Let K be a well-formed formula, propositional variable, or propositional constant. <br />
Formulas do not necessarily need to be in negation normal form, as the WEST program converts formulas into this form and generates the truth table for the formula's translated syntax. <br />
The user does not necessarily need to start their propositional variables at p0. That is, a user can input a formula that, for example, includes only the propositional variables p3, p4, and p7. For faster runtime and less memory usage, however, it is not recommended to skip natural numbers like this. <br />

After inputting a well-formed mLTL formula, the user will be prompted:
```
Please enter number of propositional variables.
```
Where the user will enter an appropriate integer. This number must be at least one more than the largest number attached to a propositional variable. For example, for a formula with the propositional variables p3, p4, and p7, the user must enter at least 8. If the user enters too small of a number, the program will crash. <br />

If exclusively propositional constants are inputted, then the user should enter "1" for the number of propositional variables in order to generate a meaningful truth table. 

### **Unary Propositional Connectives**
The only unary propositional connective is negation. <br />
Negation does NOT use parentheses. <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />
**Negation:** ~ K <br />

### **Unary Temporal Connectives**
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Unary temporal operators do NOT use parentheses. <br />
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.  <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />

**Finally:** F[a:b] K <br />
**Globally:** G[a:b] K <br />


### **Binary Propositional Connectives**
All binary connectives must be enclosed with parentheses. <br />
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (K & L) <br />
**Or:** (K v L)  <br />
**Equivalence:** (K = L)  <br />
**Implies:** (K > L)  <br />


### **Binary Temporal Connectives**
All binary connectives must be enclosed with parentheses. <br />
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. Let ":" separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**Until:** (K U[a:b] L) <br />
**Release** (K R[a:b] L) <br />





### **Associative Propositional Connectives**
The entirety of the associative propositional connective formula string must be enclosed in parentheses.  <br />
The list of elements must be preceded by the associative propositional connective. <br />
Let "," separate each element in the list, and let "[" and "]" indicate the beginning and end of the list, respectively. <br />
Let K, L, ..., M be an arbitrarily-sized list of well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (&[K, L, ..., M]) <br />
**Or:** (v[K, L, ..., M])  <br />
**Equivalence:** (=[K, L, ..., M])  <br />
**Implies:** (>[K, L, ..., M])  <br />

**A note on the associative equivalence operator:** for lists with 2 elements, the equivalence operator functions identically to the binary propositional connective equivalence operator. For formulas with 3 or more elements, the associative equivalence operator does not mean "each element in the list is equivalent". Instead, it means that the equivalence of the first two elements in the list is equivalent to the next element in the list, and the truth value for this expression is equivalent to the next element, and so on. For example:
```
(=[p0,p1,p2]) is equivalent to ((p0=p1)=p2)
(=[p0,p1,p2,p3...]) is equivalent to (...(((p0=p1)=p2)=p3)...
But,
(=[p0,p1,p2]) is not equivalent to (p0=p1=p2)
```


Note that (p0=p1=p2) is not a valid input; therefore, if one wishes to generate the truth table for a formula that means "each element in the list is equivalent", then one could employ the transitivity of the equivalence operator with the and operator. For example,
```
(p0=p1=p2=p3) can be inputted as (&[(p0=p1), (p1=p2), (p2=p3])
```

## Computations
In a computation, "1" represents a true truth value, and "0" represents a false truth value; "S" represents an arbitrary truth value, i.e. true or false.
Time steps in a computation are separated by commas. The bit-strings at each time step represent the truth values of each propositional variable, in ascending order. <br />

**Examples:** <br />
The computation of a formula with one propositional variable and 5 time steps is represented as:
```
NNF Formula: G[0:4]p0

1,1,1,1,1
```
The computation of a formula with 5 propositional variables and one time step is represented as:
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
## Subformulas and Simplification Options
As seen in the example output above, the user may choose to display the satisfying computations of all the subformulas. By default, the WEST program will show subformulas.<br />

The WEST program also allows the user to choose whether or not to simplify the satisfying computations. In almost all cases, this will dramatically reduce the number of outputted regular expressions, improving readability. However, this setting does increase runtime. By default, the WEST program simplifies outputs.


## Troubleshooting Guide

### The Program Crashed
Ensure that the number of propositional variables was large enough. The number must be at least one larger than the largest natural number attached to a propositional variable, not just the number of propositional variables in the formula. For example, the formula
```
(p1U[0:2]p3)
```
requires at least 4 propositional variables, not 2.

### The Formula is not Well-Formed
Ensure that all time intervals use ':' to separate the bounds and not a comma.  <br />
Ensure that all binary and associative connectives have parentheses surrounding them.  <br />
Ensure that for each propositional variable, its corresponding natural number immediately follows the 'p', and that there isn't a '_' or space in between.  <br />
Ensure that all unary connectives do NOT have parentheses surrounding them.

## Contributors
This project is part of the 2022 Iowa State REU with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and [Laura Gamboa Guzmán](https://sites.google.com/view/lpgamboa/home).

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset.

INSERT LINK TO OUR PAPER HERE




