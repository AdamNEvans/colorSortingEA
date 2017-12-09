import png
from node import Node
from color import Color


# Takes in a color list and creates a png
def listToPNG(colorList, directory, filename):
	# Create a writer object
	picheight = 80 # the height of the pic
	bandwidth = 2 # The width each color gets
	outputer = png.Writer(size=(bandwidth*len(colorList), picheight))

	# Create a 'layer' of colors
	colorlayer = []
	for col in colorList:
		for x in range(bandwidth):
			colorlayer.append(col.red)
			colorlayer.append(col.green)
			colorlayer.append(col.blue)

	# Repeat the layers until it satisfies the height
	totallayers = []
	for i in range(picheight):
		totallayers.append(colorlayer)

	# Open and write to the file
	picfile = open(directory+'/'+filename, 'wb')
	outputer.write(picfile, totallayers)

