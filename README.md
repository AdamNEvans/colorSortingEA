## Overview ##

This is a C++ program that uses an evolutionary algorithm to evolve a function that assigns values to colors. Sorting these values in ascending/descending order should sort the colors in a visually appealing order. Currently the fitness function is the sum of the  euclidean distance in RGB space between consecutive colors in the sorted order.

## EA program ##

The EA program takes a specified configuration file and outputs a solution file containing the evolved evaluation tree in the `solutions` directory.

To compile:

    make

To run:

    ./run.sh [<config file>]

If the config file is left blank, "configurations/default.cfg" is used.

## Visualizer ##

The visualizer program provides a few examples of how a given evaluation tree performs. It takes an evaluation tree via stdin (like those in the `solutions` directory), and outputs several images of random colors sorted by the evaluation tree.

To run:

    python3 visualizer.py < <treeFile>
