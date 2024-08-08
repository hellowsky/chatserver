#########################################################################
# File Name: autobuild.sh
# Created Time: 2024年08月08日 星期四 16时34分55秒
#########################################################################
#!/bin/bash

# Set the debug mode
set -x

# Create the build directory if it doesn't exist
mkdir -p "$PWD/build"

# Remove all files and subdirectories in the build directory
rm -rf "$PWD/build"/*

# Change directory to the build directory
cd "$PWD/build"

# Run cmake
cmake ..

# Build the project
make

