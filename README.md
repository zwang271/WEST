# Welcome to the WEST tool
The WEST package provides an automated way to generate *regular expressions*[^1] describing the set of **all satisfying computations to [Mission-time Linear Temporal Logic formulas](https://link.springer.com/chapter/10.1007/978-3-030-25543-5_1#Sec2)** (i.e., that describe the valuations, or the rows of the 'truth table', of a given mLTL formula that satisfy it), as well as a GUI to visualize them and the scripts and data sets used for testing this tool during its development. This tool was developed by the Temporal Logic group, consisting of professor Kristin Rozier, PhD student Laura P. Gamboa Guzman, and undergraduate students Chiara Travesset, Zili Wang, Jeremy Sorkin and Jenna Elwing, for the [2022 Math REU at Iowa State University](https://reu.math.iastate.edu/projects.html#ROZIER).

For a more detailed description of how the algorithms were designed and implemented, please refer to the LaTeX document located under the [paper folder](https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-/tree/master/paper). To compile it, follow the commands that are included in the README file under that folder. 

[^1]: We use this term in a rather liberal way, since we only deal with regular languages containining strings of a fixed lenght and we use the character `s` as a shorthand for `0 or 1`, although it is technically not an element of the formal alphabet. 


## Download

The latest version of WEST can be found on GitHub: https://github.com/zwang271/2022-Iowa-State-REU-Temporal-Logic-

---------------
**!!! TO-DO !!!**
- [ ] Include a brief description of the artifact goal, authors, reference to the paper, and indication on how to cite the artifact. Include the mLTL syntax used for input/output.
- [ ] Write instructions to configure and build. It may start as `From the <insert directory name> directory configure and build as follows:`
- [ ] Hardware requirements.
- [ ] Instructions on how to use the tool from the terminal line, include an example or two.
- [ ] How to use the GUI, don't forget to include some examples!
- [ ] How to run the benchmarking scripts and interpret/visualize the results. Include how to do this with limited resources.
- [ ] List of software dependencies (any python libraries used to produce the graphs?)
- [ ] Make sure everything runs smoothly in the VM, more details on that [here](https://liacs.leidenuniv.nl/~bonsanguemm/ifm23/artifacts.html).

## Build

## Usage


### The WEST tool (terminal line)

### WEST tool GUI

### Reproducing the benchmark results


## The WEST mLTL syntax

