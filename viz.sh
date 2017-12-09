#!/bin/bash

solutionFile="solutions/default.sol"

if [[ $# == 0 ]]; then
    echo "Visualizing default solution file..."
elif [[ $# == 1 ]]; then
    solutionFile="$1"
else
    echo "Usage: $0 [<treeFile>]"
    echo "   treeFile: Contains a color evaluation tree"
    exit 1
fi

cd visualizer
python3 visualizer.py < "../$solutionFile"
