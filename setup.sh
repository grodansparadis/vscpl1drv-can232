#!/bin/bash
#
# Setup script for vscpl1drv-can232 CMake build
# This initializes dependencies and prepares the build environment

set -e

echo "Setting up vscpl1drv-can232 CMake build environment..."
echo

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "VERSION.m4" ]; then
    echo "Error: Please run this script from the project root directory"
    exit 1
fi

# Initialize git submodules
echo "Initializing VSCP submodule..."
git submodule update --init --recursive
echo "✓ Submodule initialized"
echo

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir -p build
    echo "✓ Build directory created"
else
    echo "Build directory already exists"
fi
echo

# Configure with CMake
echo "Configuring with CMake..."
cd build
cmake ..
echo "✓ Configuration complete"
echo

echo "Setup complete! You can now build with:"
echo "  cd build"
echo "  cmake --build ."
echo
echo "To install:"
echo "  sudo cmake --install . --prefix /var/lib/vscp"
echo
