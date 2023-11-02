#!/bin/bash

# Check for the correct number of arguments
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <source_file_name_without_extension>"
    exit 1
fi

# Extract the source file name without the extension
source_file="$1"

# Compile the C++ code with debugging information
g++ -g "$source_file.cpp" -o "$source_file"

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful with debugging information. Executable: $source_file"
    echo ""

    # Start debugging with GDB
    gdb ./"$source_file"
else
    echo "Compilation failed."
fi
