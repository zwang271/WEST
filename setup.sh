#!/bin/bash
# WEST Project Setup Script
echo "Setting up WEST project..."

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "Warning: Python3 not found. GUI features will not be available."
    PYTHON_AVAILABLE=false
else
    PYTHON_AVAILABLE=true
fi

echo "Cleaning previous builds..."
make clean

echo "Building all components..."
make all

# Set up Python environment if Python is available
if [ "$PYTHON_AVAILABLE" = true ]; then
    echo ""
    echo "Setting up Python environment for GUI..."
    
    # Create virtual environment if it doesn't exist
    if [ ! -d "west_env" ]; then
        echo "Creating Python virtual environment..."
        python3 -m venv west_env
    fi
    
    # Activate virtual environment and install GUI requirements
    echo "Installing GUI requirements..."
    source west_env/bin/activate
    pip install -r src/requirements.txt
    
    echo ""
    echo "Python environment ready! To use GUI:"
    echo "  source west_env/bin/activate"
    echo "  python gui.py"
fi

echo ""
echo "Build complete!"
echo "All executables are now available in bin/"
if [ "$PYTHON_AVAILABLE" = true ]; then
    echo "GUI environment is set up in west_env/"
fi
echo "Run 'make help' to see all available targets"
