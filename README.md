# Welcome to the WEST tool
The WEST package provides an automated way to generate *regular expressions*[^1] describing the set of **all satisfying computations to [Mission-time Linear Temporal Logic formulas](https://link.springer.com/chapter/10.1007/978-3-030-25543-5_1#Sec2)** (i.e., that describe the valuations, or the rows of the 'truth table', of a given MLTL formula that satisfy it), as well as a GUI to visualize them and the scripts and data sets used for testing this tool during its development. For a more detailed description of how the algorithms were designed and implemented, please refer to the LaTeX document located under the [paper folder](https://github.com/zwang271/WEST). To compile it, follow the commands that are included in the README file under that folder. 

### Contributors
This project is part of the [2022 Iowa State Math REU](https://reu.math.iastate.edu/projects.html#ROZIER) with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and [Laura Gamboa Guzmán](https://sites.google.com/view/lpgamboa/home).

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset.

[^1]: We use this term in a rather liberal way, since we only deal with regular languages containing strings of a fixed length and we use the character `s` as a shorthand for `0 or 1`, although it is technically not an element of the formal alphabet. 


## Download

The latest version of WEST can be found on GitHub: https://github.com/zwang271/WEST

---------------


## Build and Usage guides

+ To run the WEST tool from the terminal line, please refer to the document [`./Documentation/WESTREADME.md`](https://github.com/zwang271/WEST/blob/master/Documentation/WESTREADME.md).
+ To run it from the GUI, please refer to the document [`./Documentation/GUIREADME.md`](https://github.com/zwang271/WEST/blob/master/Documentation/GUIREADME.md). A video tutorial is also available on [YouTube](https://youtu.be/HoBJwdCq42c).
+ To reproduce the results on the paper, please refer to the document [`./Documentation/BENCHMARKREADME.md`](https://github.com/zwang271/WEST/blob/master/Documentation/BENCHMARKREADME.md). This file also includes the specifications on the computer used to run the tests.

## A quick overview of the GUI

Consider the formula $(p_0 \wedge G_{[0,2]}\ p_1) \to p_2$. This is saying that, *if* $p_0$ *is true at the beginning and* $p_1$ *is true during the first three time-steps, then* $p_2$ *has to be true at the beginning as well.* 

To examine this formula on the WEST-GUI tool, we need to input it as `((p0 & G[0,2]p1)->p2)`, following the grammar that is described below.

![Input Example GUI](https://github.com/zwang271/WEST/blob/master/paper/images/initial.png)

This is what the WEST tool outputs when we select this same formula from the subformulae options, without selecting the **Apply REST** functionality, which tries to look for a reduction of the number of disjunctions of regular expressions (but it may take long to produce a result!)

![Example `(p_0 & G_{[0,2]} p_1) \to p_2` GUI](https://github.com/zwang271/WEST/blob/master/paper/images/subformula.png)

The check-boxes shows us the truth assignment of the three propositional variables in question (`p0, p1, p2`) on each one of the first three time steps of a time-line, representing the computation described above (`111,100,110`) that satisfies the formula. In the box below, we can find the regular expressions that capture all the computations of length 3 (since we only care about the first three steps of the time-line for this specific formula) that can satisfy this formula. By clicking on each one of them, you will be able to see a random computation that matches such expression. In addition, the buttons **Rand SAT** and **Rand UNSAT** can be used to produce a random computation of the same length that satisfies or unsatisfies the given formula, respectively, whereas the **Backbone Analysis** button allows one to see the analysis of necessary conditions for a computation to Satisfy or Unsatisfy the formula.

![Backbone Example](https://github.com/zwang271/WEST/blob/master/paper/images/backbone.png)


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

