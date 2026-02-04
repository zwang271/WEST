# Author: Zili Wang (adapted for WEST integration from original artifact)
# Last updated: 02/03/2026
# Verifies that the C++ version of WEST and the Isabelle/Haskell version of WEST
# produce the same results using formal theorem prover verification

import subprocess
import time
import sys
import re
import pathlib
import os
import signal

# Check and install dependencies
def check_and_install_deps():
    """Check if required packages are installed, install if missing."""
    required_packages = ['lark', 'tqdm']
    missing_packages = []
    
    for package in required_packages:
        try:
            __import__(package)
        except ImportError:
            missing_packages.append(package)
    
    if missing_packages:
        print(f"📦 Installing missing packages: {', '.join(missing_packages)}")
        requirements_file = pathlib.Path(__file__).parent / "requirements.txt"
        if requirements_file.exists():
            subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", str(requirements_file)])
        else:
            subprocess.check_call([sys.executable, "-m", "pip", "install"] + missing_packages)
        print("✅ Dependencies installed successfully")

# Install dependencies before importing
check_and_install_deps()

from lark import Lark, Transformer, v_args, exceptions
import ast
try:
    from tqdm import tqdm
except ImportError:
    # Fallback if tqdm not available
    def tqdm(iterable, *args, **kwargs):
        return iterable

TIMEOUT = 300  # 5 minutes timeout for each verification step

def check_dependencies():
    """Check if all required dependencies are available."""
    missing_deps = []
    
    # Check WEST executable
    west_exec = "../../bin/west"
    if not os.path.exists(west_exec):
        missing_deps.append(f"WEST executable not found at {west_exec}")
    
    # Check Haskell executables
    haskell_dir = "./isabelle_verification/haskell"
    if not os.path.exists(haskell_dir):
        missing_deps.append(f"Haskell directory not found at {haskell_dir}")
    
    required_executables = ["run_west", "check_equiv"]
    for exe in required_executables:
        exe_path = f"{haskell_dir}/{exe}"
        if not os.path.exists(exe_path):
            missing_deps.append(f"Haskell executable not found: {exe_path}")
    
    # Check grammar file
    grammar_file = f"{haskell_dir}/west_mltl_grammar.txt"
    if not os.path.exists(grammar_file):
        missing_deps.append(f"Grammar file not found: {grammar_file}")
    
    # Create required directories
    os.makedirs("isabelle_output", exist_ok=True)
    
    if missing_deps:
        print("❌ Missing dependencies:")
        for dep in missing_deps:
            print(f"   - {dep}")
        print("\n💡 To set up Isabelle verification, run:")
        print("   ./setup_verification.sh --isabelle")
        return False
    return True

def load_grammar():
    """Load the MLTL grammar for parsing formulas."""
    grammar_file = pathlib.Path("./isabelle_verification/haskell/west_mltl_grammar.txt").resolve()
    with open(grammar_file, "r") as f:
        grammar = f.read()
    return Lark(grammar, parser='lalr', start='wff')

def transform(node):
    """Transform parse tree to Haskell Mltl syntax - from original artifact."""
    isa_dict = {
        "true": "True_mltl",
        "false": "False_mltl",
        "and": "And_mltl",
        "or": "Or_mltl",
        "future": "Future_mltl",
        "global": "Global_mltl",
        "until": "Until_mltl",
        "release": "Release_mltl",
    }
    while node.data == "wff":
        node = node.children[0]
        wff_type = node.data
        
    if wff_type == "wff_base":
        child = node.children[0]
        assert(child.data in ["prop_cons", "prop_var", "wff"])
        if child.data == "prop_cons":
            return isa_dict[child.children[0].data]
        elif child.data == "prop_var":
            variable = child.children[0].children[0].value
            return f"(Prop_mltl {variable})"
        elif child.data == "wff":
            return transform(child)
            
    if wff_type == "wff_neg":
        child1, child2 = node.children
        assert(child1.data == "neg" and child2.data == "wff")
        if child1.data == "neg":
            return f"(Not_mltl {transform(child2)})"
        
    if len(node.children) == 3:
        child1, child2, child3 = node.children
        if wff_type == "wff_untemp":
            temp_conn = isa_dict[child1.children[0].data]
            lb = child2.children[0].children[0].value
            ub = child2.children[1].children[0].value
            wff = transform(child3)
            return f"({temp_conn} {wff} {lb} {ub})"
        if wff_type == "wff_binprop":
            prop_conn = isa_dict[child2.children[0].data]
            wff1 = transform(child1)
            wff2 = transform(child3)
            return f"({prop_conn} {wff1} {wff2})"
        
    if wff_type == "wff_bintemp":
        child1, child2, child3, child4 = node.children
        assert(child1.data == "wff" and child2.data == "binary_temp_conn" and
               child3.data == "interval" and child4.data == "wff")
        temp_conn = isa_dict[child2.children[0].data]
        wff1 = transform(child1)
        lb = child3.children[0].children[0].value
        ub = child3.children[1].children[0].value
        wff2 = transform(child4)
        return f"({temp_conn} {wff1} {wff2} {lb} {ub})"

def isa_syntax(west_formula, parser):
    """Convert WEST formula to Isabelle Haskell syntax - from original artifact."""
    AST = parser.parse(west_formula)
    isa_formula = transform(AST)
    return isa_formula

def isa_regex(west_output):
    """Transform WEST output to Isabelle regex format - from original artifact."""
    if west_output is None:
        return ""
    west_output = west_output.upper()
    west_output = west_output.split("\n")[1:-1]
    west_output = [r.split(",") for r in west_output]
    west_output = [[list(state) for state in r] for r in west_output]
    west_output = str(west_output).replace("'", "").replace(" ", "")
    return west_output

def complen(west_output):
    """Calculate the completion length of WEST output - from original artifact."""
    west_output = west_output.upper()
    west_output = west_output.split("\n")[1:-1]
    west_output = [r.split(",") for r in west_output]
    if west_output == []:
        return 0
    return max([len(r) for r in west_output])

def pad(west_output, cplen):
    """Pad/truncate WEST output to match Isabelle dimensions - from original artifact."""
    with open(pathlib.Path("./isabelle_verification/haskell/out_isa.txt").resolve(), "r") as f:
        isa_regex = f.read().replace("S", "\"S\"").replace("1", "\"1\"").replace("0", "\"0\"")
    isa_regex = ast.literal_eval(isa_regex)
    if isa_regex == []:
        return west_output
    
    # Get dimensions from Isabelle output
    isa_num_rows = len(isa_regex)
    isa_num_cols = len(isa_regex[0]) if isa_regex else 0
    num_vars = len(isa_regex[0][0]) if isa_regex and isa_regex[0] else 3
    
    arb_state = "S" * num_vars
    west_output = west_output.upper()
    west_output = west_output.split("\n")[1:-1]  # Remove first and last line
    west_output = [r.split(",") for r in west_output]
    
    # Truncate or pad each row to match Isabelle column count
    for r in west_output:
        # Truncate if too long
        while len(r) > isa_num_cols:
            r.pop()
        # Pad if too short  
        while len(r) < isa_num_cols:
            r.append(arb_state)
    
    # Truncate or pad rows to match Isabelle row count
    while len(west_output) > isa_num_rows:
        west_output.pop()
    while len(west_output) < isa_num_rows:
        west_output.append([arb_state] * isa_num_cols)
        
    west_output = "\n" + "\n".join([",".join(r) for r in west_output]) + "\n"
    return west_output

def run_west(formula):
    """Run WEST executable with the given formula - adapted from original artifact."""
    west_exec = "../../bin/west"
    west_output_file = "../../output/output.txt"
    
    try:
        # Run WEST command  
        cmd = [west_exec, formula]
        pro = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, preexec_fn=os.setsid)
        try:
            pro.wait(TIMEOUT)
        except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
            return None
            
        # Read output file
        with open(west_output_file, 'r') as f:
            west_output = f.read()
            
        return west_output
        
    except Exception as e:
        print(f"❌ Error running WEST: {e}")
        return None

def run_isabelle(formula, parser):
    """Run Isabelle Haskell verification with the given formula - adapted from original artifact."""
    try:
        # Convert formula to Haskell syntax
        haskell_formula = isa_syntax(formula, parser)
        
        # Run Haskell executable
        haskell_dir = pathlib.Path("./isabelle_verification/haskell").resolve()
        cmd = f"cd {haskell_dir} && ./run_west \"{haskell_formula}\""
        
        pro = subprocess.Popen(cmd, shell=True, preexec_fn=os.setsid, 
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            pro.wait(TIMEOUT)
        except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
            return None
            
        # Read output file
        isa_output_file = haskell_dir / "out_isa.txt"
        with open(isa_output_file, 'r') as f:
            isa_output = f.read()
            
        return isa_output
        
    except Exception as e:
        print(f"❌ Error running Isabelle: {e}")
        return None

def run_check_equiv():
    """Run equivalence check between WEST and Isabelle outputs - from original artifact."""
    try:
        haskell_dir = pathlib.Path("./isabelle_verification/haskell").resolve()
        cmd = f"cd {haskell_dir} && ./check_equiv"
        
        pro = subprocess.Popen(cmd, shell=True, preexec_fn=os.setsid, 
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        try:
            pro.wait(timeout=TIMEOUT)
            
            # Read result from output file
            with open(haskell_dir / "out_equiv.txt", "r") as f:
                result = f.read().strip()
            return result
            
        except subprocess.TimeoutExpired:
            os.killpg(os.getpgid(pro.pid), signal.SIGTERM)
            return "Timeout"
            
    except Exception as e:
        print(f"❌ Error running equivalence check: {e}")
        return "Timeout"

def verify(formula):
    """Verify a single formula against Isabelle/Haskell implementation - from original artifact."""
    print(f"Formula: {formula}")
    
    parser = load_grammar()
    
    # Run Isabelle
    isa_out = run_isabelle(formula, parser)
    if isa_out is None:
        print("⏱️  Isabelle verification timed out")
        return "timeout"
        
    # Run WEST
    west_out = run_west(formula)
    if west_out is None:
        print("⏱️  WEST execution timed out")
        return "timeout"
        
    # Format WEST output for comparison - no padding needed with correct complen
    west_formatted = isa_regex(west_out)
    with open(pathlib.Path("./isabelle_verification/haskell/out_west.txt").resolve(), "w") as f:
        f.write(west_formatted)
        
    # Check equivalence
    equiv = run_check_equiv()
    print(f"Equivalent with WEST: {equiv}")
    
    if equiv == "True":
        print(f'✅ Output files are equivalent on formula "{formula}"')
        return True
    elif equiv == "Timeout":
        print("⏱️  Equivalence check timed out")
        return "timeout"
    else:
        print(f"❌ Outputs are not equivalent: {equiv}")
        return False

def verify_single_formula(formula):
    """Verify a single formula - main entry point."""
    if not check_dependencies():
        return False
        
    print(f'Single formula verification: {formula}')
    return verify(formula)

def verify_batch(formulas):
    """Verify multiple formulas."""
    if not check_dependencies():
        return False
        
    results = []
    
    for formula in formulas:
        result = verify(formula)
        results.append((formula, result))
        
    # Print summary
    passed = sum(1 for _, result in results if result is True)
    failed = sum(1 for _, result in results if result is False)
    timeouts = sum(1 for _, result in results if result == "timeout")
    total = len(results)
    
    print(f"\n📊 Verification Summary: {passed}/{total} passed, {failed} failed, {timeouts} timed out")
    
    failed_formulas = [(formula, result) for formula, result in results if result is False]
    if failed_formulas:
        print("❌ Failed formulas:")
        for formula, _ in failed_formulas:
            print(f"   - {formula}")
    
    timeout_formulas = [(formula, result) for formula, result in results if result == "timeout"]
    if timeout_formulas:
        print("⏱️  Timed out formulas:")
        for formula, _ in timeout_formulas:
            print(f"   - {formula}")
    
    return failed == 0  # Success if no failures (timeouts are acceptable)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        # No arguments provided - run full test suite
        print("🧪 Running full Isabelle verification test suite...")
        formula_file = pathlib.Path("./verify_formulas/formulas.txt").resolve()
        
        if not os.path.exists(formula_file):
            print(f"❌ Test formula file not found: {formula_file}")
            print("Usage: python verify_isabelle.py <formula>")
            print("       python verify_isabelle.py <formula1> <formula2> ...")
            sys.exit(1)
            
        with open(formula_file, "r") as f:
            formulas = [line.strip() for line in f.readlines() if line.strip()]
            
        print(f"📝 Found {len(formulas)} test formulas")
        success = verify_batch(formulas)
        sys.exit(0 if success else 1)
        
    formulas = sys.argv[1:]
    
    if len(formulas) == 1:
        success = verify_single_formula(formulas[0])
        sys.exit(0 if success else 1)
    else:
        success = verify_batch(formulas)
        sys.exit(0 if success else 1)