 # WEST Project Root Makefile
# Unified build system for all components

.PHONY: all clean west interpreter reg brute_force help test

# Default target
all: west interpreter reg

# Individual component builds
west:
	@echo "Building WEST main component..."
	@cd src/WEST && $(MAKE)

interpreter:
	@echo "Building MLTL interpreter..."
	@cd src/MLTL_interpreter && $(MAKE)

reg:
	@echo "Building MLTL regular expressions component..."
	@cd src/MLTL_reg && $(MAKE)

brute_force:
	@echo "Building MLTL brute force component..."
	@cd src/MLTL_brute_force && $(MAKE)

# Cleaning
clean:
	@echo "Cleaning all build artifacts..."
	@cd src/WEST && $(MAKE) clean 2>/dev/null || true
	@cd src/MLTL_interpreter && $(MAKE) clean 2>/dev/null || true
	@cd src/MLTL_reg && $(MAKE) clean 2>/dev/null || true
	@cd src/MLTL_brute_force && $(MAKE) clean 2>/dev/null || true
	@find . -name "*.o" -delete 2>/dev/null || true
	@find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true

# Note: Executables are built directly into bin/ directory
# No separate install step needed

# Test target (placeholder for future tests)
test:
	@echo "Running tests..."
	@echo "Note: Test suite not yet implemented"

# Help target
help:
	@echo "WEST Project Build System"
	@echo "========================="
	@echo "Available targets:"
	@echo "  all         - Build all components (default)"
	@echo "  west        - Build WEST main component"
	@echo "  interpreter - Build MLTL interpreter"
	@echo "  reg         - Build MLTL regular expressions"
	@echo "  brute_force - Build MLTL brute force component"
	@echo "  clean       - Remove all build artifacts"
	@echo "  test        - Run test suite (not implemented)"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Note: Executables are built directly into bin/ directory"
	@echo ""
	@echo "Environment variables:"
	@echo "  CXX         - C++ compiler (default: g++)"
	@echo "  CXXFLAGS    - Additional C++ compiler flags"