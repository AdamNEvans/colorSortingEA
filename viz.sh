#!/bin/bash

if [[ $# != 1 ]]; then
    echo "Usage: $0 <treeFile>"
    echo "   treeFile: Contains a color evaluation tree"
    exit 1
fi

cd visualizer
python3 visualizer.py < "../$1"
