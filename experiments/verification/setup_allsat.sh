#!/bin/bash

# Setup script for verification suite dependencies
# Author: Auto-generated for WEST project
# This script automates the setup of verification dependencies including AllSAT and R2U2

set -e  # Exit on any error

# Parse command line arguments
SETUP_ALLSAT=false
SETUP_R2U2=false
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
        --all)
            SETUP_ALL=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [--allsat] [--r2u2] [--all]"
            echo "  --allsat    Setup AllSAT verification dependencies (Z3, MLTLMaxSAT)"
            echo "  --r2u2      Setup R2U2 verification dependencies"
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
if [[ "$SETUP_ALLSAT" == false && "$SETUP_R2U2" == false && "$SETUP_ALL" == false ]]; then
    SETUP_ALLSAT=true
fi

if [[ "$SETUP_ALL" == true ]]; then
    SETUP_ALLSAT=true
    SETUP_R2U2=true
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
    
    # Check if R2U2 submodule is initialized
    echo "📁 Initializing R2U2 submodule..."
    cd ../../  # Go to project root
    git submodule update --init --recursive experiments/verification/r2u2
    cd experiments/verification
    
    # R2U2 specific setup (placeholder for now)
    if [[ -f "r2u2/setup.sh" ]]; then
        echo "🔨 Running R2U2 setup..."
        cd r2u2
        ./setup.sh
        cd ..
    else
        echo "⚠️  R2U2 setup script not found, manual setup may be required"
    fi
    
    # Create R2U2 output directories
    echo "📁 Creating R2U2 output directories..."
    mkdir -p r2u2_output
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
    # Add R2U2 specific packages here
    PACKAGES+=(numpy)  # Example - adjust based on actual R2U2 requirements
fi

if [[ ${#PACKAGES[@]} -gt 0 ]]; then
    echo "📦 Installing required Python packages: ${PACKAGES[*]}..."
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

    # Test with a simple formula
    echo "🎯 Running AllSAT quick test..."
    if $PYTHON_CMD verify_allsat.py "p0" > /tmp/allsat_test.log 2>&1; then
        echo "✅ AllSAT verification working!"
    else
        echo "⚠️  AllSAT test failed. Check /tmp/allsat_test.log for details"
    fi
fi

if [[ "$SETUP_R2U2" == true ]]; then
    echo "🎯 Testing R2U2 setup..."
    # Add R2U2 specific tests here
    echo "✅ R2U2 setup completed (manual verification may be needed)"
fi

echo ""
echo "🎉 Verification setup complete!"
if [[ "$SETUP_ALLSAT" == true ]]; then
    echo "   AllSAT: $PYTHON_CMD verify_allsat.py \"your_formula_here\""
fi
if [[ "$SETUP_R2U2" == true ]]; then
    echo "   R2U2: $PYTHON_CMD verify_r2u2.py \"your_formula_here\""
fi
echo "   String: $PYTHON_CMD verify_string.py"
echo "   Interpreter: $PYTHON_CMD verify_interpreter.py"
echo ""