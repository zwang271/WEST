# WEST Tool Paper Artifact
This is an artifact for the paper "WEST: Interactive Validation of Mission-time Linear Temporal Logic (MLTL)" 
For the tool manual, see [here](src/WEST_Tool_Manual.md)

### Prerequisites
This artifact requires the following
- Linux (tested on Ubuntu 24.04.1 LTS)
- Python 3.10+
- Python pip
- Python venv
- c++ compiler (we use g++)
- make (we use GNU Make 4.3) 
- cmake (cmake 3.22.1)
- Java Runtime environment (see https://ubuntu.com/tutorials/install-jre#1-overview)
    - In short, get it with `sudo apt install default-jre`

### Building all binaries and dependencies
1. Create a python virtual enviroment by running 
`python3 -m venv west_env`
2. Activate the virtual envoronment by running `source west_env/bin/activate`
3. Install required libraries by running `pip install -r ./src/requirements.txt`
    - If needed, update pip by running `python3 -m pip install --upgrade pip`
4. Build binaries by running `./setup.sh`
5. You should now be able to launch the graphic user interface by running `cd src` and then `python3 gui.py`
    - If you encounter the following error: "This application failed to start because no Qt platform plugin could be initialized. Reinstalling the application may fix this problem."
    Please run: `sudo ln -sf /usr/lib/x86_64-linux-gnu/qt5/plugins/platforms/ /usr/bin/`

### Validation Scripts
These scripts replicates the validation steps outlined in Section 4, Figure 6 of the tool paper. Starting from the root directory: 
- `cd experiments/verification` 
- Run `python3 verify_string.py` to replicate section 4.1 (~10min local machine)
- Run `python3 verify_allsat.py` to replicate section 4.2 (~2min local machine)
- Run `python3 verify_r2u2_parallel.py` to replicate section 4.3 (~1hr on Iowa State University's high performance computing NOVA cluster, may vary depending on number of CPU cores in the machine)
- Run `python3 verify_interpreter.py` to replicate section 4.4 (not including FPROGG, since this was done independently in their work) (~1hr local machine)

### Plotting Scripts
Run the following to replicate figures 7a, 7b, and 7c.
- `cd ../benchmarking`
- run `./runall.sh` (~1hr) to benchmark WEST on the d, m, and n formula datasets 
    - it is not strictly necessary to run this in order to run the next plotting script as we already provide the results from our experiments. 
- generate the three plots presented in the paper by running `python3 plot.py`

### End
Thank you for evaluating our artifact! 

