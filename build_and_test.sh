#!/bin/bash

# Exit immediately if any command fails
set -e

# Remove existing files in build directory
if [ -d "build" ]; then
    rm -rf build
fi

# Configure the CMake project to build source and tests
cmake -S . -B build -DBUILD_TESTS=ON

# Build
cmake --build build

# Run tests
cd build
ctest
cd ..

