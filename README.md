## Overview ##

This is a C++ program that uses an evolutionary algorithm to evolve a function that assigns values to colors. Sorting these values in ascending/descending order should sort the colors in a visually appealing order. Currently the fitness function is the sum of the  euclidean distance in RGB space between consecutive colors in the sorted order.

## EA Program ##

The EA program takes a specified configuration file and outputs a solution file containing the evolved evaluation tree in the `solutions` directory.

To compile:

    make

To run:

    ./run.sh [<configFile>]

If the `configFile` is omitted, the default configuration file (`configurations/default.cfg`) is used.

## Visualizer ##

The visualizer program provides a few examples of how a given evaluation tree performs. It takes an evaluation tree via stdin (like those in the `solutions` directory), and outputs several images of random colors sorted by the evaluation tree.

To run:

    viz.sh [<treeFile>]

If `treeFile` is omitted, the default solution file (`solutions/default.sol`) is used.

## Tree Format ##

The EA prints solutions as the preorder traversal of the evolved tree, where each node is on a different line indented by a number of spaces equal to its depth in the tree times two. For example, the tree

               ADD
              /   \
            SUB   RED
           /   \
       GREEN   BLUE

is printed as the following:

    ADD
      SUB
        GREEN
        BLUE
      RED

The different node types can be found at either `src/node.cpp` in `print(FILE*, int)`, or the top of `visualizer/node.py`
