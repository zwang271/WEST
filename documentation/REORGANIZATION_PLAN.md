# WEST Repository Reorganization Plan

## Current Issues Identified
1. **Duplicate Code**: `backend/` mirrors `src/WEST/` exactly
2. **Scattered Build Artifacts**: Executables built in different locations
3. **Legacy Files**: Old Windows build files and debug artifacts
4. **Inconsistent Structure**: Mixed organization patterns

## Recommended Actions

### 1. Immediate Cleanup
```bash
# Remove duplicate backend directory (identical to src/WEST)
rm -rf backend/

# Remove Windows/Visual Studio artifacts
find . -name "*.tlog" -delete
find . -name "*.recipe" -delete
find . -name "*.manifest" -delete
find . -name "*.lastbuildstate" -delete
find . -name "*.idb" -delete
find . -name "*.pdb" -delete
find . -name "*.ilk" -delete

# Remove debug build directories
rm -rf src/MLTL_reg/legacy/x64/
rm -rf src/MLTL_reg/legacy/Debug/

# Clean build artifacts
rm -f src/west src/interpret src/interpret_batch
rm -f src/MLTL_reg/west_string
```

### 2. Consolidate Build System
Create a root-level Makefile that builds all components:

```makefile
# Root Makefile
.PHONY: all clean west interpreter reg

all: west interpreter reg

west:
	cd src/WEST && make

interpreter: 
	cd src/MLTL_interpreter && make

reg:
	cd src/MLTL_reg && make

clean:
	cd src/WEST && make clean
	cd src/MLTL_interpreter && make clean  
	cd src/MLTL_reg && make clean
	rm -f bin/*

install: all
	mkdir -p bin/
	cp src/west bin/
	cp src/MLTL_interpreter/interpret bin/
	cp src/MLTL_interpreter/interpret_batch bin/
	cp src/MLTL_reg/west_string bin/
```

### 3. Update Individual Makefiles
Modify each Makefile to build executables in a `bin/` directory:

**src/WEST/Makefile:**
```makefile
all: west

west:
	mkdir -p ../../bin
	g++ west.cpp reg.cpp utils.cpp parser.cpp -o ../../bin/west -std=c++17

clean:
	rm -f ../../bin/west
```

**src/MLTL_interpreter/Makefile:**
```makefile
all: interpret interpret_batch

interpret: 
	mkdir -p ../../bin
	g++ ./interpret.cpp ./evaluate_mltl.cpp ./utils.cpp -o ../../bin/interpret

interpret_batch: 
	mkdir -p ../../bin
	g++ ./interpret_batch.cpp ./evaluate_mltl.cpp ./utils.cpp -o ../../bin/interpret_batch

clean:
	rm -f ../../bin/interpret ../../bin/interpret_batch
```

**src/MLTL_reg/Makefile:**
```makefile
all: west_string

west_string: 
	mkdir -p ../../bin
	g++ string_grammar.cpp string_nnf_grammar.cpp string_reg.cpp string_rest.cpp string_utils.cpp string_west.cpp -o ../../bin/west_string -std=c++17

clean:
	rm -f ../../bin/west_string
```

### 4. Update setup.sh
```bash
#!/bin/bash
# Build all WEST components

echo "Building WEST components..."

# Create bin directory
mkdir -p bin

# Build all components
make clean
make all

echo "Build complete. Executables available in bin/"
echo "Available tools:"
echo "  - bin/west          # Main WEST tool"
echo "  - bin/interpret     # MLTL interpreter"  
echo "  - bin/interpret_batch # Batch MLTL interpreter"
echo "  - bin/west_string   # String-based WEST tool"
```

### 5. Directory Structure (After Cleanup)
```
WEST/
├── bin/                    # All built executables
├── src/                    # Source code
│   ├── WEST/              # Main WEST tool
│   ├── MLTL_interpreter/  # Interpreter components
│   ├── MLTL_reg/          # Regular expression components
│   ├── MLTL_brute_force/  # Brute force implementation
│   └── gui.py             # GUI interface
├── experiments/           # Verification and benchmarking
├── documentation/         # Project documentation  
├── docs/                 # Generated documentation
├── west_env/             # Python virtual environment
├── Makefile              # Root build system
├── setup.sh              # Build script
└── README.md             # Project documentation
```

### 6. Benefits After Reorganization
- **Single Source of Truth**: No duplicate code
- **Consistent Build**: All executables in `bin/`
- **Clean Repository**: No build artifacts in git
- **Better Documentation**: Clear structure
- **Easier Development**: Simplified workflow

## Implementation Priority
1. **High**: Remove duplicates and Windows artifacts
2. **Medium**: Consolidate build system
3. **Low**: Legacy cleanup (can be done incrementally)

Would you like me to implement any of these changes?