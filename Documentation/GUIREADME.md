# WEST GUI Instructions

This README describes how to use the WEST GUI. For instructions on using WEST directly from the terminal, see the file WESTREADME.md. For a video tutorial, click this [link](https://youtu.be/HoBJwdCq42c).
The WEST mLTL Truth Table Generator is a program that parses well-formed mLTL formulas and outputs the regular expression
for the computations that satisfy the formula.
Our recursive algorithm is based on the regular expressions of mLTL operators and runs with approximately doubly exponential space and time complexity in the worst case.

## Usage

After downloading and cloning this repository, use the commands

``` 
$ cd 2022-Iowa-State-REU-Temporal-Logic-
$ cd MLTL_reg
$ cd MLTL
$ make west_lib
$ python gui.py
```

Then, the user will be prompted:

```
Please enter a MLTL formula.
```

See the Grammar section below for how to properly format inputted mLTL formulas.

If the user enters a something other than a properly written mLTL formula,
the WEST program will prompt the user to enter in another string.

Along with entering the formula, the user can toggle checkmarks that run simplify or REST. The simplify function leads to a shorter list of outputted regular expressions, at the expense of longer computation. Note that if simplify is turned on, the WEST program will simply the output for every subformula,
as well as the output for the input formula. <br />
Toggling REST will run the REST algorithm. REST simplifies a particular form of regular expressions to the trivial computation, but is very computationally expensive. It is generally not recommended to use REST. <br />

Once the user has entered a well-formed formula, toggled the simplifications of their choice, and hit the enter key, they will be prompted:

``
Please select a subformula to explore below.
``
Below this will be each subformula of the inputted formula, including the full formula. The user may click on one or multiple of these subformulas; each one will open in their own tab. <br />

Within each tab, the user will see a table of checkboxes, one for each variable at each time step, and a list of regular expressions that represent satisfying computations for the subformula. The user can toggle the truth value of each variable at every time step or choose a random satisfying computation. Clicking any of the regular expressions will generate a random computation of that form, while clicking the button Rand Comp gives a random satisfying computation of any form. When the computation demonstrated by the check boxes satisfies the formula, the formula turns green. If not, it turns red. Additionally, any regular expressions that describe the computation turn green as well.


## Grammar
The WEST program strips whitespaces from input.
Non-empty intervals are recommended for meaningful truth table generation.

### **Propositional Variables and Constants**
**True:** true <br />
**False:** false <br />
**First Variable:** p0 <br />
**Second Variable:** p1 <br />
And so on, where each consecutive variable is followed with the appropriate natural number. <br />

Let K be a well-formed formula, propositional variable, or propositional constant. <br />
Formulas do not necessarily need to be in negation normal form, as the WEST program converts formulas into this form and generates the truth table for the formula's translated syntax. <br />
The user does not necessarily need to start their propositional variables at p0. That is, a user can input a formula that, for example, includes only the propositional variables p3, p4, and p7. For faster runtime and less memory usage, however, it is not recommended to skip natural numbers like this. <br />

### **Unary Propositional Connectives**
The only unary propositional connective is negation. <br />
Negation does NOT use parentheses. <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />
**Negation:** ~ K <br />

### **Unary Temporal Connectives**
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Unary temporal operators do NOT use parentheses. <br />
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. Let "," separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.  <br />
Let K be a well-formed formula, propositional variable, or propositional constant. <br />

**Finally:** F[a,b] K <br />
**Globally:** G[a,b] K <br />


### **Binary Propositional Connectives**
All binary connectives must be enclosed with parentheses. <br />
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (K & L) <br />
**Or:** (K | L)  <br />
**Equivalence:** (K = L)  <br />
**Implies:** (K -> L)  <br />


### **Binary Temporal Connectives**
All binary connectives must be enclosed with parentheses. <br />
All temporal operators must be followed by an interval. All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br />
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. Let "," separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.
Let K, L be well-formed formulas, propositional variables, or propositional constants. <br />

**Until:** (K U[a,b] L) <br />
**Release** (K R[a,b] L) <br />





### **Associative Propositional Connectives**
The entirety of the associative propositional connective formula string must be enclosed in parentheses.  <br />
The list of elements must be preceded by the associative propositional connective. <br />
Let "," separate each element in the list, and let "[" and "]" indicate the beginning and end of the list, respectively. <br />
Let K, L, ..., M be an arbitrarily-sized list of well-formed formulas, propositional variables, or propositional constants. <br />

**And:** (&[K, L, ..., M]) <br />
**Or:** (|[K, L, ..., M])  <br />
**Equivalence:** (=[K, L, ..., M])  <br />
**Implies:** (->[K, L, ..., M])  <br />

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


Below is the context-free grammar that a well-formed mLTL formula must follow. This is optional reading, and only included for the interested reader.
```
Context-Free Grammar for a MLTL well-formed formula (wff).

'True', 'False', 'Negation', 'Or', 'And', 'If and only if', and 'Implies' are represented by the symbols:
    'true', 'false', '~', '|', '&', '=', and '->' respectively.
    
‘Eventually’, ‘Always’, ‘Until’, and ‘Release’ are represented by the symbols:
    ‘F’, ‘G’, ‘U’, and ‘R’ respectively.


Alphabet = { ‘0’, ‘1’, …, ‘9’, ‘p’, ‘(‘, ‘)’, ‘[’, ‘]’, ‘,’ ,
                       ‘true’, ‘false’,                
                       ‘~’, ‘|’, ‘&’, ‘=’, ‘->’, 
                       ‘F’, ‘G’, ‘U’, ‘R’ }


Digit         ->  ‘0’ | ‘1’ | … |’9’
Num           ->  Digit Num |  Digit
Interval      ->  ‘[’  Num ‘,’ Num ‘]’  
Prop_var      ->  ‘p’ Num

Prop_cons         ->  ‘true’ | ‘false’
Unary_Prop_conn   ->  ‘~’
Binary_Prop_conn  ->  ‘|’ | ‘&’ | ‘=’ | ‘->’

Assoc_Prop_conn   -> ‘|’ | ‘&’ | ‘=’
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


## Computations
In a computation, "1" represents a true truth value, and "0" represents a false truth value; "s" represents an arbitrary truth value, i.e. either true or false.
Time steps in a computation are separated by commas. The bit-strings at each time step represent the truth values of each propositional variable, in ascending order. <br />

**Examples:** <br />
The computation of a formula with one propositional variable and 5 time steps is represented as:
```
NNF Formula: G[0,4]p0

1,1,1,1,1
```
The computation of a formula with 5 propositional variables and one time step is represented as:
```
NNF Formula: G[0,0](&[p0,p1,p2,p3,p4])

11111
````
The computations of a formula with two propositional variables that are both eventually true at a third time step are represented as:
```
NNF Formula: F[0,2](p0&p1)

11,ss,ss
ss,11,ss
ss,ss,11
```
Where in each string separated by commas, the first digit represents the truth value of p0, and the second digit represents the truth value of p1.


## Troubleshooting Guide

### The Formula is not Well-Formed
Ensure that all time intervals use ',' to separate the bounds.  <br />
Ensure that all binary and associative connectives have parentheses surrounding them.  <br />
Ensure that for each propositional variable, its corresponding natural number immediately follows the 'p', and that there isn't a '_' or space in between.  <br />
Ensure that all unary connectives do NOT have parentheses surrounding them.

## Contributors
This project is part of the 2022 Iowa State REU with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and [Laura Gamboa Guzmán](https://sites.google.com/view/lpgamboa/home).

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset.




