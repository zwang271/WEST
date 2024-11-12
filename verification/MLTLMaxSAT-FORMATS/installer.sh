#!/usr/bin/bash

# Assumes you have cmake 3.22.1: 
# https://github.com/Kitware/CMake/releases/tag/v3.22.1
# For the linux download:
# https://github.com/Kitware/CMake/releases/download/v3.22.1/cmake-3.22.1-linux-x86_64.tar.gz
rm -rf build
mkdir build
cd build
cmake ..
make -j12
git clone https://github.com/Z3Prover/z3.git
cd z3
git checkout 49ee570b09047cbea94f7d26e3df10c86c9ce596
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../../usr
make -j12
make install