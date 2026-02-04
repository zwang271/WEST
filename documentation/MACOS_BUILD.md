# macOS Build Instructions for WEST

## Issues Fixed

### 1. Compiler Compatibility
- **Problem**: Hardcoded `g++` in Makefiles
- **Solution**: Use `CXX` environment variable (defaults to `g++`, can be overridden)
- **Usage**: 
  ```bash
  # Use system default compiler (recommended on macOS)
  export CXX=clang++
  ./setup.sh
  
  # Or specify per-build
  CXX=clang++ ./setup.sh
  ```

### 2. Microsoft-Specific bitset Methods
- **Problem**: `_Find_first()` and `_Find_next()` don't exist on GCC/Clang
- **Solution**: Implemented portable `find_first()` and `find_next()` functions
- **Fixed in**: `src/WEST/bitoptimized/reg.cpp`

### 3. C++17 filesystem Library
- **Problem**: May require explicit linking on older systems
- **Solution**: Auto-detect if `-lstdc++fs` linking is needed
- **Note**: Requires GCC 8+ or Clang 7+

## macOS-Specific Build Commands

```bash
# Recommended approach for macOS
export CXX=clang++
export CXXFLAGS="-std=c++17 -O2"
./setup.sh

# Alternative: specify compiler per-build
CXX=clang++ CXXFLAGS="-std=c++17 -O2" make -C src/WEST
```

## Troubleshooting

### If filesystem errors occur:
```bash
# For older GCC versions, try:
export LDFLAGS="-lstdc++fs"
./setup.sh

# For Clang, try:
export LDFLAGS="-lc++fs"  
./setup.sh
```

### If Xcode Command Line Tools are missing:
```bash
xcode-select --install
```

### Verify your setup:
```bash
clang++ --version  # Should be 7.0+
g++ --version      # If using GCC, should be 8.0+
```

## Tested Configurations
- ✅ Ubuntu 24.04 + GCC 13.3.0 (your current setup)
- ⚠️ macOS with Clang++ (should work with these fixes)
- ⚠️ macOS with Homebrew GCC (should work with these fixes)