#!/bin/bash

# Setup script for verification suite dependencies
# Author: Auto-generated for WEST project
# This script automates the setup of verification dependencies including AllSAT and R2U2

set -e  # Exit on any error

# Parse command line arguments
SETUP_ALLSAT=false
SETUP_R2U2=false
SETUP_ISABELLE=false
SETUP_ALL=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --allsat)
            SETUP_ALLSAT=true
            shift
            ;;
        --r2u2)
            SETUP_R2U2=true
            shift
            ;;
        --isabelle)
            SETUP_ISABELLE=true
            shift
            ;;
        --all)
            SETUP_ALL=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [--allsat] [--r2u2] [--isabelle] [--all]"
            echo "  --allsat    Setup AllSAT verification dependencies (Z3, MLTLMaxSAT)"
            echo "  --r2u2      Setup R2U2 verification dependencies"
            echo "  --isabelle  Setup Isabelle/Haskell verification dependencies"
            echo "  --all       Setup all verification dependencies"
            echo "  -h, --help  Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use -h or --help for usage information"
            exit 1
            ;;
    esac
done

# Default to AllSAT if no specific option given
if [[ "$SETUP_ALLSAT" == false && "$SETUP_R2U2" == false && "$SETUP_ISABELLE" == false && "$SETUP_ALL" == false ]]; then
    SETUP_ALLSAT=true
fi

if [[ "$SETUP_ALL" == true ]]; then
    SETUP_ALLSAT=true
    SETUP_R2U2=true
    SETUP_ISABELLE=true
fi

echo "🔧 Setting up verification dependencies..."

# Check if we're in the right directory
if [[ ! -f "verify_string.py" ]]; then
    echo "❌ Error: Must be run from the experiments/verification directory"
    exit 1
fi

if [[ "$SETUP_ALLSAT" == true ]]; then
    echo "📦 Setting up AllSAT verification..."
    
    # Check if cmake is installed
    if ! command -v cmake &> /dev/null; then
        echo "📦 Installing cmake and build tools..."
        sudo apt update
        sudo apt install -y cmake build-essential
    fi

    # Initialize MLTLMaxSAT-FORMATS submodule if not already done
    echo "📁 Initializing MLTLMaxSAT-FORMATS submodule..."
    cd ../../  # Go to project root
    git submodule update --init --recursive experiments/verification/MLTLMaxSAT-FORMATS
    cd experiments/verification

    # Run the MLTLMaxSAT installer if main executable doesn't exist
    if [[ ! -f "MLTLMaxSAT-FORMATS/build/main" ]]; then
        echo "🔨 Building MLTLMaxSAT-FORMATS translator..."
        cd MLTLMaxSAT-FORMATS
        ./installer.sh
        cd ..
    else
        echo "✅ MLTLMaxSAT-FORMATS already built"
    fi

    # Create necessary directories
    echo "📁 Creating AllSAT output directories..."
    mkdir -p maxsat_output
fi

if [[ "$SETUP_R2U2" == true ]]; then
    echo "📦 Setting up R2U2 verification..."
    
    # Check if make and gcc are installed
    if ! command -v make &> /dev/null || ! command -v gcc &> /dev/null; then
        echo "📦 Installing build tools..."
        sudo apt update
        sudo apt install -y make gcc build-essential
    fi
    
    # Check if R2U2 submodule is initialized
    echo "📁 Initializing R2U2 submodule..."
    cd ../../  # Go to project root
    git submodule update --init --recursive experiments/verification/r2u2
    cd experiments/verification
    
    # Build R2U2 monitor
    if [[ ! -f "r2u2/monitors/static/build/r2u2" ]]; then
        echo "🔨 Building R2U2 monitor..."
        cd r2u2/monitors/static
        make clean all
        cd ../../../
        if [[ -f "r2u2/monitors/static/build/r2u2" ]]; then
            echo "✅ R2U2 monitor built successfully"
        else
            echo "❌ R2U2 monitor build failed"
            exit 1
        fi
    else
        echo "✅ R2U2 monitor already built"
    fi
    
    # Create R2U2 output directories
    echo "📁 Creating R2U2 output directories..."
    mkdir -p r2u2_output
fi

if [[ "$SETUP_ISABELLE" == true ]]; then
    echo "📦 Setting up Isabelle/Haskell verification..."
    
    # Check if GHC (Haskell compiler) is installed
    if ! command -v ghc &> /dev/null; then
        echo "📦 Installing Haskell (GHC)..."
        sudo apt update
        sudo apt install -y haskell-platform ghc
    fi
    
    # Check if Isabelle verification directory exists
    if [[ ! -d "isabelle_verification" ]]; then
        echo "❌ Isabelle verification directory not found"
        echo "   This should have been created during integration"
        exit 1
    fi
    
    # Build Haskell executables
    echo "🔨 Building Haskell executables..."
    cd isabelle_verification/haskell
    
    # Compile the Haskell programs
    if [[ ! -f "run_west" ]]; then
        echo "   Compiling run_west..."
        ghc -o run_west run_west.hs
    fi
    
    if [[ ! -f "check_equiv" ]]; then
        echo "   Compiling check_equiv..."
        ghc -o check_equiv check_equiv.hs
    fi
    
    cd ../../
    
    # Verify executables were created
    if [[ -f "isabelle_verification/haskell/run_west" && -f "isabelle_verification/haskell/check_equiv" ]]; then
        echo "✅ Haskell executables built successfully"
    else
        echo "❌ Haskell executable build failed"
        exit 1
    fi
    
    # Create Isabelle output directories
    echo "📁 Creating Isabelle output directories..."
    mkdir -p isabelle_output
fi

# Configure Python environment
echo "🐍 Configuring Python environment..."
cd ../../  # Go to project root to use west_env

# Check if we're in a virtual environment or can use west_env
if [[ -z "$VIRTUAL_ENV" ]]; then
    if [[ -f "west_env/bin/activate" ]]; then
        echo "🔄 Activating west_env virtual environment..."
        source west_env/bin/activate
        PYTHON_CMD="python"
    else
        echo "⚠️  No virtual environment found, using system python3"
        PYTHON_CMD="python3"
    fi
else
    echo "✅ Using active virtual environment: $VIRTUAL_ENV"
    PYTHON_CMD="python"
fi

# Install required Python packages
PACKAGES=()
if [[ "$SETUP_ALLSAT" == true ]]; then
    PACKAGES+=(z3-solver pycosat)
fi
if [[ "$SETUP_R2U2" == true ]]; then
    # R2U2 uses the C2PO compiler which requires basic Python
    # No additional Python packages required beyond standard library
    echo "📝 R2U2 uses standard Python libraries (no additional packages needed)"
fi
if [[ "$SETUP_ISABELLE" == true ]]; then
    # Isabelle verification uses local requirements file
    echo "📦 Installing Isabelle verification Python dependencies..."
    if [[ -f "requirements.txt" ]]; then
        $PYTHON_CMD -m pip install -r requirements.txt
    else
        # Fallback to manual installation
        PACKAGES+=(lark tqdm)
    fi
fi

if [[ ${#PACKAGES[@]} -gt 0 ]]; then
    echo "📦 Installing additional Python packages: ${PACKAGES[*]}..."
    $PYTHON_CMD -m pip install "${PACKAGES[@]}"
fi

# Test the setup
echo "🧪 Testing setup..."

if [[ "$SETUP_ALLSAT" == true ]]; then
    cd experiments/verification
    if $PYTHON_CMD -c "import z3, pycosat; print('✅ AllSAT Python dependencies OK')" 2>/dev/null; then
        echo "✅ AllSAT Python packages installed successfully"
    else
        echo "❌ AllSAT Python package installation failed"
        exit 1
    fi

    if [[ -f "MLTLMaxSAT-FORMATS/build/main" ]]; then
        echo "✅ MLTLMaxSAT translator built successfully"
    else
        echo "❌ MLTLMaxSAT translator build failed"
        exit 1
    fi
fi

if [[ "$SETUP_R2U2" == true ]]; then
    cd experiments/verification
    echo "🎯 Testing R2U2 setup..."
    
    # Test R2U2 monitor executable
    if [[ -f "r2u2/monitors/static/build/r2u2" ]]; then
        echo "✅ R2U2 monitor executable found"
    else
        echo "❌ R2U2 monitor executable not found"
        exit 1
    fi
    
    # Test C2PO compiler
    if [[ -f "r2u2/compiler/c2po.py" ]]; then
        echo "✅ R2U2 C2PO compiler found"
    else
        echo "❌ R2U2 C2PO compiler not found"
        exit 1
    fi
fi

if [[ "$SETUP_ISABELLE" == true ]]; then
    cd experiments/verification
    echo "🎯 Testing Isabelle setup..."
    
    # Test Python dependencies
    if $PYTHON_CMD -c "import lark; print('✅ Isabelle Python dependencies OK')" 2>/dev/null; then
        echo "✅ Isabelle Python packages installed successfully"
    else
        echo "❌ Isabelle Python package installation failed"
        exit 1
    fi
    
    # Test Haskell executables
    if [[ -f "isabelle_verification/haskell/run_west" ]]; then
        echo "✅ Isabelle run_west executable found"
    else
        echo "❌ Isabelle run_west executable not found"
        exit 1
    fi
    
    if [[ -f "isabelle_verification/haskell/check_equiv" ]]; then
        echo "✅ Isabelle check_equiv executable found"  
    else
        echo "❌ Isabelle check_equiv executable not found"
        exit 1
    fi
    
    # Test grammar file
    if [[ -f "isabelle_verification/haskell/west_mltl_grammar.txt" ]]; then
        echo "✅ Isabelle grammar file found"
    else
        echo "❌ Isabelle grammar file not found"
        exit 1
    fi
fi

echo ""
echo "🎉 Verification setup complete!"
if [[ "$SETUP_ALLSAT" == true ]]; then
    echo "   AllSAT: $PYTHON_CMD verify_allsat.py \"your_formula_here\""
fi
if [[ "$SETUP_R2U2" == true ]]; then
    echo "   R2U2: $PYTHON_CMD verify_r2u2.py \"your_formula_here\""
fi
if [[ "$SETUP_ISABELLE" == true ]]; then
    echo "   Isabelle: $PYTHON_CMD verify_isabelle.py \"your_formula_here\""
fi
echo "   String: $PYTHON_CMD verify_string.py"
echo "   Interpreter: $PYTHON_CMD verify_interpreter.py"
echo ""