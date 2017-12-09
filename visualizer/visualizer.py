import sys
import random
import utils
from node import Node
from color import Color


random.seed()

nodeList = [line.strip() for line in sys.stdin.readlines()]
tree = Node(nodeList, 0)

nimages = 3
ncolors = 1000

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
	directory = "./images"
	filename = 'sorted_' + str(i) + '.png'
	utils.listToPNG(colors, directory, filename)
