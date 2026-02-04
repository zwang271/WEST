# AllSAT Verification Setup

This directory contains verification scripts that test WEST against other formally verified models and tools.

## Quick Setup for Verification Suites

Some verification suites require external dependencies. Use our automated setup script:

```bash
cd experiments/verification
./setup_verification.sh --allsat    # Setup AllSAT verification
./setup_verification.sh --r2u2      # Setup R2U2 verification  
./setup_verification.sh --all       # Setup all verification backends
```

This script will automatically:
- Install system dependencies (cmake, build tools)
- Initialize and build the MLTLMaxSAT-FORMATS submodule
- Configure Python environment and install required packages (z3-solver, pycosat)
- Create necessary output directories
- Run a test to verify everything works

## Manual Setup (if needed)

If the automated setup fails, you can set up manually:

1. **Install system dependencies:**
   ```bash
   sudo apt update && sudo apt install -y cmake build-essential
   ```

2. **Initialize MLTLMaxSAT submodule:**
   ```bash
   cd ../../  # Go to WEST root
   git submodule update --init --recursive experiments/verification/MLTLMaxSAT-FORMATS
   cd experiments/verification/MLTLMaxSAT-FORMATS
   ./installer.sh
   cd ..
   ```

3. **Install Python packages:**
   ```bash
   pip install z3-solver pycosat
   ```

4. **Create output directory:**
   ```bash
   mkdir -p maxsat_output
   ```

## Usage

After setup, run AllSAT verification:

```bash
python3 verify_allsat.py "your_mltl_formula"
```

## Other Verification Scripts

- `verify_string.py` - Compare WEST vs west_string (no additional setup needed)
- `verify_interpreter.py` - Compare WEST vs interpreter (no additional setup needed)  
- `verify_r2u2.py` - Compare WEST vs R2U2 (requires R2U2 setup)

## Troubleshooting

- If you see "ModuleNotFoundError: No module named 'z3'", run `./setup_verification.sh --allsat`
- If MLTLMaxSAT build fails, ensure cmake and build tools are installed
- For permission errors, make sure the setup script is executable: `chmod +x setup_verification.sh`
- For R2U2 issues, try `./setup_verification.sh --r2u2`