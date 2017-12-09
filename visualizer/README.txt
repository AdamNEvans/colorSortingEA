Usage: python3 visualizer.py < <inputFile>

inputFile contains each node's type on a separate line. The node types can be
found at the top of node.py. All nodes simply have just the node type on the
line, but constant nodes have the node type, a space, then the value of the
constant. For example: "NODE_CONST 12.345".

It spits out three different sets of sorted random colors, and puts them in the images folder.
