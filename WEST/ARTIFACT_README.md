# WEST Tool Paper Artifact
This is meant to be an artifact for the paper "WEST: Interactive Validation of Mission-time Linear Temporal Logic (MLTL)" 

### Prerequisites
This artifact requires the following
- Linux (tested on Ubuntu 22.04.2 LTS)
- Python 3.10+
- c++ compiler with Make tool

### Building all binaries and dependencies
1. Create a python virtual enviroment by running 
`python3 -m venv west_env`
2. Activate the virtual envoronment by running
    - on Windows: `./west_env/Scripts/activate`
    - on Unix or MacOS: `source west_env/bin/activate`
3. Install required libraries by running `pip install -r ./requirements.txt`
    - If needed, update pip by running `python3 -m pip install --upgrade pip`
4. Build binaries by running `./setup.sh`
5. You should now be able to launch the graphic user interface by running `python3 gui.py`

### Validation Scripts
These scripts replicates the validation steps outlined in Section 4, Figure 6 of the tool paper. 
- `cd verification` 
- Run `python3 verify_string.py` to replicate section 4.1 (~1hr)
- Run `python3 verify_allsat.py` to replicate section 4.2 (~2min)
- Run `python3 verify_r2u2_parallel.py` to replicate section 4.3 (~4hr)
- Run `python3 verify_interpreter.py` to replicate section 4.4 (not including FPROGG, since this was done independently in their work) (~1hr)

### Plotting Scripts
Run the following to replicate figures 7a, 7b, and 7c.
- `cd ../benchmarking`
- run `./runall.sh` (~1hr) to benchmark WEST on the d, m, and n formula datasets and generate the three plots presented in the paper. 

### End
Thank you for evaluating our artifact! 

