import random


NODE_RED = 'RED'
NODE_GREEN = 'GREEN'
NODE_BLUE = 'BLUE'
NODE_HUE = 'HUE'
NODE_HSL_SAT = 'HSL_SAT'
NODE_HSV_SAT = 'HSV_SAT'
NODE_VALUE = 'VALUE'
NODE_LIGHTNESS = 'LIGHTNESS'
NODE_LUMINOSITY = 'LUMINOSITY'
NODE_CONST = 'CONST'

NODE_ADD = 'ADD'
NODE_SUB = 'SUB'
NODE_MUL = 'MUL'
NODE_DIV = 'DIV'
NODE_MOD = 'MOD'
NODE_RAND = 'RAND'
NODE_MIN = 'MIN'
NODE_MAX = 'MAX'


class Node:
	leaf_types = [NODE_RED, NODE_GREEN, NODE_BLUE, NODE_HUE, NODE_HSL_SAT,
					NODE_HSV_SAT, NODE_VALUE, NODE_LIGHTNESS, NODE_LUMINOSITY,
					NODE_CONST]
	bin_op_types = [NODE_ADD, NODE_SUB, NODE_MUL, NODE_DIV, NODE_MOD,
					NODE_RAND, NODE_MIN, NODE_MAX]
	all_types = leaf_types + bin_op_types
	
	# =========================================================================
	# nodeTypes: List of strings which match the values of the node type
	#            constants declared above. For constant nodes, the format is:
	#            "NODE_CONST <value>"
	# index: The index in nodeTypes for the type of this node. Used instead of
	#        defaulting to the first element so that slices don't have to be
	#        created when creating the node's children. Plus it gives a unique
	#        id to every node in the tree, which could be handy.
	# =========================================================================

	def __init__(self, nodeTypes, index=0):
		words = nodeTypes[index].split(" ")
		self.type = words[0];   # what operation/value this node represents
		self.left = None        # left child node
		self.right = None       # right child node
		self.size = 0           # num nodes in the tree that has a root of self
		self.value = None       # constant value if this is of type NODE_CONST
		self.index = index      # index of self in the preorder tree traversal
		
		if self.type in self.leaf_types:
			self.left = None
			self.right = None
			self.size = 1
			if self.type == NODE_CONST:
				self.value = float(words[1])
		elif self.type in self.bin_op_types:
			self.left = Node(nodeTypes, self.index + 1)
			self.right = Node(nodeTypes, self.index + self.left.size + 1)
			self.size = 1 + self.left.size + self.right.size
	
	# ====================================================================
	# color: The color to compute a value for
	# ====================================================================

	def eval(self, color):
		# Leaf nodes
		if   self.type == NODE_RED:        return color.red
		elif self.type == NODE_GREEN:      return color.green
		elif self.type == NODE_BLUE:       return color.blue
		elif self.type == NODE_HUE:        return color.hue
		elif self.type == NODE_HSL_SAT:    return color.HSLsat
		elif self.type == NODE_HSV_SAT:    return color.HSVsat
		elif self.type == NODE_VALUE:      return color.value
		elif self.type == NODE_LIGHTNESS:  return color.lightness
		elif self.type == NODE_LUMINOSITY: return color.luminosity
		elif self.type == NODE_CONST:      return self.value

		# Operator nodes
		left = self.left.eval(color)
		right = self.left.eval(color)

		if   self.type == NODE_RAND:       return random.uniform(left, right)
		elif self.type == NODE_MIN:        return min(left, right)
		elif self.type == NODE_MAX:        return max(left, right)
		elif self.type == NODE_ADD:        return left + right
		elif self.type == NODE_SUB:        return left - right
		elif self.type == NODE_MUL:        return left * right
		elif self.type == NODE_DIV:
			if abs(right) <= 0.001:
				if right > 0:
					return left / 0.001
				else:
					return right / -0.001
			else:
				return left / right
		elif self.type == NODE_MOD:
			if abs(right) <= 0.001:
				return 0
			else:
				return left % right

		# this node is not of a known type
		print("WARNING: Returning None for node type '{}'".format(self.type))
		return None
