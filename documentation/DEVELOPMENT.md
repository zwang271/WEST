# WEST Development Guide

## Project Structure

```
WEST/
├── bin/                    # Installed executables (created by 'make install')
├── build/                  # Build artifacts and temporary files
├── documentation/          # Project documentation
├── docs/                   # Web documentation
├── experiments/           # Benchmarking and verification scripts
├── src/                   # Source code
│   ├── WEST/             # Main WEST algorithm implementation
│   │   ├── bitoptimized/ # Optimized bitset operations
│   │   └── ...           # Core C++ files
│   ├── MLTL_interpreter/ # MLTL formula interpreter
│   ├── MLTL_reg/         # Regular expression generation
│   ├── MLTL_brute_force/ # Brute force verification
│   ├── gui.py            # Main GUI application
│   └── requirements.txt  # Python dependencies
├── west_env/             # Python virtual environment
├── Makefile              # Unified build system
└── build.config          # Build configuration
```

## Development Workflow

### Setting Up Development Environment

1. **Clone and setup:**
   ```bash
   git clone <repository>
   cd WEST
   ./setup.sh
   ```

2. **Create virtual environment:**
   ```bash
   python3 -m venv west_env
   source west_env/bin/activate  # Unix/macOS
   # or ./west_env/Scripts/activate  # Windows
   ```

3. **Install dependencies:**
   ```bash
   pip install -r src/requirements.txt
   ```

### Building Components

The project uses a unified Makefile system with these targets:

- `make all` - Build all components
- `make west` - Build main WEST component
- `make interpreter` - Build MLTL interpreter  
- `make reg` - Build regex component
- `make clean` - Clean build artifacts
- `make install` - Install to bin/
- `make help` - Show all targets

### Code Organization

#### Core Components

1. **WEST Algorithm (`src/WEST/`)**
   - `west.cpp` - Main entry point
   - `reg.cpp` - Regular expression generation
   - `parser.cpp` - Formula parsing
   - `utils.cpp` - Utility functions
   - `bitoptimized/` - Optimized bitset operations

2. **MLTL Interpreter (`src/MLTL_interpreter/`)**
   - `interpret.cpp` - Single formula interpretation
   - `interpret_batch.cpp` - Batch processing
   - `evaluate_mltl.cpp` - Core evaluation logic

3. **Regex Generator (`src/MLTL_reg/`)**
   - `string_west.cpp` - String-based regex generation
   - Various helper files for string operations

### Coding Standards

#### C++ Code Style
- Use C++17 features
- Follow existing naming conventions
- Document complex algorithms
- Use consistent indentation (4 spaces)

#### Key Conventions
- `MAXBITS` constant controls bitset size
- Cross-platform compatibility (Windows/Unix)
- Error handling with meaningful messages

### Testing

Currently, testing is primarily done through:
1. **Benchmarking scripts** (`experiments/benchmarking/`)
2. **Verification scripts** (`experiments/verification/`)  
3. **GUI testing** (manual)

### Platform Support

The codebase supports:
- **Linux** (primary development)
- **macOS** (with clang++)
- **Windows** (with g++ or MSVC)

Platform-specific considerations:
- Compiler detection in Makefiles
- Path separators in build scripts
- Executable extensions (.exe on Windows)

### Performance Considerations

1. **Bitset Operations**: Core algorithm uses `std::bitset<MAXBITS>`
2. **Memory Usage**: MAXBITS determines memory footprint
3. **Compiler Optimization**: Use `-O2` or `-O3` for release builds

### Adding New Features

1. **New Operators**: Modify parser and evaluation logic
2. **GUI Features**: Update `src/gui.py`
3. **Optimizations**: Consider `bitoptimized/` directory
4. **Tests**: Add to `experiments/` directory

### Build Configuration

The `build.config` file contains:
- Compiler settings
- Platform detection
- Common flags and paths
- MAXBITS configuration

### Troubleshooting

Common issues:
1. **Missing C++ compiler**: Install g++ or clang++
2. **Python version**: Requires Python 3.10+
3. **Missing make**: Install build-essential (Linux) or Xcode tools (macOS)
4. **Bitset issues**: Check MAXBITS configuration

### Contributing

1. Follow existing code style
2. Test on multiple platforms if possible
3. Update documentation for new features
4. Use meaningful commit messages