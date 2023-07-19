# Welcome to the WEST tool
The WEST package provides an automated way to generate *regular expressions*[^1] describing the set of **all satisfying computations to [Mission-time Linear Temporal Logic formulas](https://link.springer.com/chapter/10.1007/978-3-030-25543-5_1#Sec2)** (i.e., that describe the valuations, or the rows of the 'truth table', of a given mLTL formula that satisfy it), as well as a GUI to visualize them and the scripts and data sets used for testing this tool during its development. For a more detailed description of how the algorithms were designed and implemented, please refer to the LaTeX document located under the [paper folder](https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-/tree/master/paper). To compile it, follow the commands that are included in the README file under that folder. 

### Contributors
This project is part of the [2022 Iowa State Math REU](https://reu.math.iastate.edu/projects.html#ROZIER) with mentors [Kristin Yvonne Rozier](https://www.aere.iastate.edu/kyrozier/) and [Laura Gamboa Guzmán](https://sites.google.com/view/lpgamboa/home).

WEST is an acronym for the last names of the undergraduate mathematicians who collaborated on this project: Zili Wang, Jenna Elwing, Jeremy Sorkin, and Chiara Travesset.

[^1]: We use this term in a rather liberal way, since we only deal with regular languages containining strings of a fixed lenght and we use the character `s` as a shorthand for `0 or 1`, although it is technically not an element of the formal alphabet. 


## Download

The latest version of WEST can be found on GitHub: https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-

---------------
**!!! TO-DO !!!**
- [x] Include a brief description of the artifact goal, authors, reference to the paper, and indication on how to cite the artifact. Include the mLTL syntax used for input/output.
- [ ] Link instructions to configure and build. It may start as `From the <insert directory name> directory configure and build as follows:`
- [ ] Hardware requirements.
- [ ] How to run the benchmarking scripts and interpret/visualize the results. Include how to do this with limited resources!
- [ ] List of software dependencies (any python libraries used to produce the graphs?)
- [ ] Make sure everything runs smoothly in the VM, more details on that [here](https://liacs.leidenuniv.nl/~bonsanguemm/ifm23/artifacts.html).

----------------

## Usage

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

### Reproducing the benchmark results


## The WEST mLTL syntax

