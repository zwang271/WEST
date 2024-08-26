# Welcome to the WEST tool
The WEST package provides an automated way to generate *regular expressions*[^1] describing the set of **all satisfying computations to [Mission-time Linear Temporal Logic formulas](https://link.springer.com/chapter/10.1007/978-3-030-25543-5_1#Sec2)** (i.e., that describe the valuations, or the rows of the 'truth table', of a given MLTL formula that satisfy it), as well as a GUI to visualize them and the scripts and data sets used for testing this tool during its development. For a more detailed description of how the algorithms were designed and implemented, please refer to our paper [here](https://temporallogic.org/research/WEST/WEST_extended.pdf).


## Build and Usage guides

Requirements: g++ compiler for the command-line tool, Python 3.7+, pip
+ Go to WEST folder by running `cd WEST`
+ Build binaries by running `cd src ; make ; cd ..`
+ Create a python virtual enviroment by running 
`python -m venv west_env`
+ Activate the virtual envoronment by running
    + on Windows: `./west_env/Scripts/activate`
    + on Unix or MacOS: `source west_env/bin/activate`
+ Install required libraries by running `pip install -r .\requirements.txt`
+ Launch the graphic user interface by running `python gui.py`

To run the WEST tool from the terminal line, please refer to the document [`./Documentation/WESTREADME.md`](https://github.com/zwang271/WEST/blob/master/Documentation/WESTREADME.md).

## A quick overview of the GUI
Simple examples involving each of the operators are as follows:
+ Propositional variable: `p0`
+ And: `p0 & p1`
+ Or: `p0 | p1`
+ Logical implication: `p0 -> p1`
+ Negation: `p0 & !p1`
+ Globally: `G[0,3] p1`
+ Future: `F[0,3] p1`
+ Until: `p0 U[0,3] p1`
+ Release: `p0 R[0,3] p1`

As a more complex example, consider the formula $(p_0 \wedge G_{[0,2]}\ p_1) \to p_2$. This is saying that, *if* $p_0$ *is true at the beginning and* $p_1$ *is true during the first three time-steps, then* $p_2$ *has to be true at the beginning as well.* 

To examine this formula on the WEST-GUI tool, we need to input it as `((p0 & G[0,2]p1)->p2)`.

![Input Example GUI](https://github.com/zwang271/WEST/blob/master/paper/images/initial.png)

This is what the WEST tool outputs when we select this same formula from the subformulae options, without selecting the **Apply REST** functionality, which tries to look for a reduction of the number of disjunctions of regular expressions (but it may take long to produce a result!)

![Example `(p_0 & G_{[0,2]} p_1) \to p_2` GUI](https://github.com/zwang271/WEST/blob/master/paper/images/subformula.png)

The check-boxes shows us the truth assignment of the three propositional variables in question (`p0, p1, p2`) on each one of the first three time steps of a time-line, representing the computation described above (`111,100,110`) that satisfies the formula. In the box below, we can find the regular expressions that capture all the computations of length 3 (since we only care about the first three steps of the time-line for this specific formula) that can satisfy this formula. By clicking on each one of them, you will be able to see a random computation that matches such expression. In addition, the buttons **Rand SAT** and **Rand UNSAT** can be used to produce a random computation of the same length that satisfies or unsatisfies the given formula, respectively, whereas the **Backbone Analysis** button allows one to see the analysis of necessary conditions for a computation to Satisfy or Unsatisfy the formula.

![Backbone Example](https://github.com/zwang271/WEST/blob/master/paper/images/backbone.png)


### Contributors
This project began as part of the [2022 Iowa State Math REU](https://reu.math.iastate.edu/projects.html#ROZIER) with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and [Laura Gamboa Guzmán](https://sites.google.com/view/lpgamboa/home).

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset.

[^1]: We use this term in a rather liberal way, since we only deal with regular languages containing strings of a fixed length and we use the character `s` as a shorthand for `0 or 1`, although it is technically not an element of the formal alphabet. 


### The WEST MLTL syntax

Below is the context-free grammar that a well-formed MLTL formula must follow. This is optional reading, and only included for the interested reader.
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

## Troubleshooting Guide

### The Formula is not Well-Formed
Ensure that all time intervals use ',' to separate the bounds.  <br />
Ensure that all binary and associative connectives have parentheses surrounding them.  <br />
Ensure that for each propositional variable, its corresponding natural number immediately follows the 'p', and that there isn't a '_' or space in between.  <br />
Ensure that all unary connectives do NOT have parentheses surrounding them.

