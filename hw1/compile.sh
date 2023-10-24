#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source_file.cpp> <output_executable>"
    exit 1
fi

# Extract the source file and output executable names
source_file="$1"
output_executable="$2"

# Compile the C++ code
g++ "$source_file" -o "$output_executable"

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable: $output_executable"
    echo ""

    # Execute the compiled program
    ./"$output_executable"
else
    echo "Compilation failed."
fi
