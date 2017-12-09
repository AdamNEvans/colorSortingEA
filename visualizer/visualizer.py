import os
import sys
import random
import utils
from node import Node
from color import Color


nimages = 3
ncolors = 1000
directory = "./images"

random.seed()

# read in the tree
nodeList = [line.strip() for line in sys.stdin.readlines()]
tree = Node(nodeList)

# make sure we can output our fancy images
if not os.path.exists(directory):
	os.mkdir(directory)
elif not os.path.isdir(directory):
	print("ERROR: Cannot create output directory '{}'".format(directory))
	sys.exit(1)

# generate the sorted color images
for i in range(nimages):
	# Generate the list of random colors to sort
	list = []
	for x in range(ncolors):
		color = Color()
		color.randomize()
		pair = (tree.eval(color), color)
		list.append(pair)

	list.sort()
	colors = [x[1] for x in list]

	# save the sort as an image
	filename = 'sorted_' + str(i) + '.png'
	utils.listToPNG(colors, directory, filename)
