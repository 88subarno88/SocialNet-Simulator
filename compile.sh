#!/bin/bash

# Compile the SocialNet Simulator
g++ -std=c++17 -O2 -Wall -Wextra socialnet.cpp -o socialnet

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful! Executable: socialnet"
    echo ""
    echo "Usage:"
    echo "  Interactive mode:  ./socialnet"
    echo "  From file:         ./socialnet < input.txt"
    echo "  To file:           ./socialnet < input.txt > output.txt"
else
    echo "Compilation failed!"
    exit 1
fi