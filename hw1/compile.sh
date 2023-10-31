#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <output_executable_name>"
    exit 1
fi

# Extract the output executable name
output_executable="$1"

# Add ".cpp" extension to create the source file name
source_file="${output_executable}.cpp"

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
