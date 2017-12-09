#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "node.h"


using std::cout;
using std::endl;


#define MAX_CONST_VALUE   100


// ============================================================================
//                            FUNCTION DECLARATIONS
// ============================================================================

static int numOperandsForType(NodeType type);

// ============================================================================
//                                 CONSTANTS
// ============================================================================

const int NODE_TYPE_2_OPERAND = NODE_TYPE_ADD | NODE_TYPE_SUB | NODE_TYPE_MUL |
                                NODE_TYPE_DIV | NODE_TYPE_RAND;
const int NODE_TYPE_INPUT =     NODE_TYPE_CONST | NODE_TYPE_RED |
                                NODE_TYPE_GREEN | NODE_TYPE_BLUE |
                                NODE_TYPE_HUE | NODE_TYPE_HSV_SAT |
                                NODE_TYPE_HSL_SAT | NODE_TYPE_VALUE |
                                NODE_TYPE_LIGHTNESS | NODE_TYPE_LUMINOSITY;

// ============================================================================
// Constructs a default node
// ============================================================================

Node::Node()
{
  type = NODE_TYPE_NONE;
  maxHeight = 0;
  treeSize = 0;
  validNodes = NODE_TYPE_ALL_NODES;
  
  for (int i = 0; i < NODE_MAX_OPERANDS; i++)
  {
    operands[i].n = NULL;
  }
}

// ============================================================================
// Constructs a Node using the ramped half and half initialization.
// @param inHeight The max height the tree is allowed to grow to
// @param nodes The valid nodes mask
// ============================================================================

Node::Node(int inHeight, int nodes)
{
  maxHeight = inHeight;
  type = NODE_TYPE_NONE;
  validNodes = nodes;
  
  // null out operands so createSubtree doesn't try to free garbage
  for (int i = 0; i < NODE_MAX_OPERANDS; i++)
  {
    operands[i].n = NULL;
  }
  
  bool useGrow = (random() < INT_MAX / 2);
  createSubtree(maxHeight, useGrow);
}

// ============================================================================
// Destructs a Node
// ============================================================================

Node::~Node()
{
  for (int i = 0; i < numOperandsForType(type); i++)
  {
    delete operands[i].n;
  }
}

// ============================================================================
// Creates a tree with this Node as the root using the ramped half and half
// grow method. Deletes old nodes to accomodate the new nodes.
// @param inHeight The max height the tree is allowed to grow to
// @param grow True if the grow method should be used. False if the full method
// should be used.
// ============================================================================

void Node::createSubtree(int inHeight, bool grow)
{
  if (inHeight <= 0)
  {
    cout << "ERROR: creating tree with height 0" << endl;
    exit(1);
  }
  
  maxHeight = inHeight;
  
  // check for old operand nodes that need to be freed
  for (int i = 0; i < numOperandsForType(type); i++)
  {
    if (operands[i].n != NULL)
    {
      delete operands[i].n;
      operands[i].n = NULL;
    }
  }
  
  // ---------------------------------------
  // generate the new type for this node
  type = (NodeType)(1 << (random() % NODE_TYPE_NUM_TYPES));
  
  if (maxHeight == 1)    // base case
  {
    while ((type & NODE_TYPE_INPUT & validNodes) == 0)
    {
      type = (NodeType)(1 << (random() % NODE_TYPE_NUM_TYPES));
    }
  }
  else if (!grow)
  {
    while ((type & (~NODE_TYPE_INPUT) & validNodes) == 0)
    {
      type = (NodeType)(1 << (random() % NODE_TYPE_NUM_TYPES));
    }
  }
  else
  {
    while ((type & validNodes) == 0)
    {
      type = (NodeType)(1 << (random() % NODE_TYPE_NUM_TYPES));
    }
  }
  
  // ---------------------------------------
  // do recursion to generate the tree
  switch (type)
  {
    case NODE_TYPE_ADD:
    case NODE_TYPE_SUB:
    case NODE_TYPE_MUL:
    case NODE_TYPE_DIV:
    case NODE_TYPE_RAND:
      operands[0].n = new Node(inHeight - 1, validNodes);
      operands[1].n = new Node(inHeight - 1, validNodes);
      treeSize = operands[0].n->treeSize +
                 operands[1].n->treeSize + 1;  // +1 for self
      break;
      
    case NODE_TYPE_CONST:
      operands[0].f = (float)((random() / (double)INT_MAX) * MAX_CONST_VALUE);
      treeSize = 1;
      break;
      
    default:
      treeSize = 1;
  }
}

// ============================================================================
//                          MUTATION AND RECOMBINATION
// ============================================================================
// Mutates the tree rooted at the index'th node in the preorder traversal of
// the tree rooted at this node.
// @param index The index of the root of the tree to mutate
// ============================================================================

void Node::mutateSubtree(int index)
{
  // ----------------------------------------
  // check if we should mutate this node
  if (index == 0)
  {
    createSubtree(maxHeight, true);
    return;
  }
  
  // ----------------------------------------
  // otherwise, mutate a descendant
  if ((type & NODE_TYPE_2_OPERAND) != 0)     // go deeper
  {
    index--;
    int size;
    int noperands = numOperandsForType(type);
    
    for (int i = 0; i < noperands; i++)
    {
      size = operands[i].n->treeSize;
      if (index < size)
      {
        operands[i].n->mutateSubtree(index);
        
        // recompute the tree size
        treeSize = 1;
        
        for (int j = 0; j < noperands; j++)
        {
          treeSize += operands[j].n->treeSize;
        }
        
        break;   // we found the node to mutate, so stop looking
      }
      else
      {
        index -= size;
      }
    }
  }
}

// ============================================================================
// Recombines the tree rooted at this node with the tree rooted at other to
// produce a third tree which is returned. This tree serves as the base tree
// with the replacement tree being provided by other.
// @param other The other tree to recombine with
// @return The new tree generated
// ============================================================================

Node* Node::recombine(Node* other)
{
  // kid based off of this -> copy it to make new member
  Node* tree1 = copy();
  Node* tree2 = other;
  
  // figure out which subtrees to swap
  int index1 = random() % tree1->treeSize;
  int index2 = random() % tree2->treeSize;
  
  Node* replacement = tree2->getNode(index2)->copy();
  Node* replaced = tree1->setNode(index1, replacement);
  
  if (replaced == NULL)
  {
    delete tree1;
    tree1 = replacement;
  }
  else
  {
    delete replaced;
  }
  
  return tree1;
}

// ============================================================================
//                                 UTILITIES
// ============================================================================
// Returns a deep copy of this tree
// @return A deep copy of this tree
// ============================================================================

Node* Node::copy()
{
  Node* node = new Node();
  node->type = type;
  node->treeSize = treeSize;
  node->maxHeight = maxHeight;
  node->validNodes = validNodes;
  
  int noperands = numOperandsForType(type);
  
  for (int i = 0; i < noperands; i++)
  {
    node->operands[i].n = operands[i].n->copy();
  }
  
  if (type == NODE_TYPE_CONST)
  {
    node->operands[0].f = operands[0].f;
  }
  
  return node;
}

// ============================================================================
// Returns the index'th node of the tree rooted at this node
// @param index The index of the node to get in the preorder traversal of this
// tree
// @return The index'th node of the tree rooted at this node in the preorder
// traversal. Returns NULL if index out of bounds.
// ============================================================================

Node* Node::getNode(int index)
{
  // base cases
  if (index < 0)
  {
    return NULL;
  }
  if (index == 0)
  {
    return this;
  }
  
  index--;
  int noperands = numOperandsForType(type);
  int size;
  
  for (int i = 0; i < noperands; i++)
  {
    size = operands[i].n->treeSize;
    
    if (index < size)
    {
      return operands[i].n->getNode(index);
    }
    
    index -= size;
  }
  
  return NULL;
}

// ============================================================================
// Sets the node at the given index to be the given tree (no copy)
// @param index The index of the node to replace
// @param tree The replacement tree/node
// @return A pointer to the node that was replaced (and its tree)
// ============================================================================

Node* Node::setNode(int index, Node* tree)
{
  if (index == 0)      // can only happen if we replace the root
  {
    return NULL;
  }
  
  int noperands = numOperandsForType(type);
  int size;
  
  index--;       // account for "this" node
  
  // look for the right operand
  for (int i = 0; i < noperands; i++)
  {
    size = operands[i].n->treeSize;
    
    if (index < size)      // this is the right subtree
    {
      // set the pointer to return
      Node* pointer;
      
      if (index == 0)   // replace the tree
      {
        pointer = operands[i].n;
        operands[i].n = tree;
      }
      else      // forward the pointer of the replaced tree
      {
        pointer = operands[i].n->setNode(index, tree);
      }
      
      // update the treeSize
      treeSize = 1;
      
      for (int j = 0; j < noperands; j++)
      {
        treeSize += operands[j].n->treeSize;
      }
      
      return pointer;
    }
    else      // else try the next operand
    {
      index -= operands[i].n->treeSize;
    }
  }
  
  return NULL;
}

// ============================================================================
// Evaluates this node for the given sensor inputs
// @param data The data to evaluate with
// @return The value of the subtree rooted at this node
// ============================================================================

float Node::eval(void* data)
{
  Color* color = (Color*)data;
  
  switch (type)
  {
    // functions
    case NODE_TYPE_ADD:
      return operands[0].n->eval(color) + operands[1].n->eval(color);
    
    case NODE_TYPE_SUB:
      return operands[0].n->eval(color) - operands[1].n->eval(color);
    
    case NODE_TYPE_MUL:
      return operands[0].n->eval(color) * operands[1].n->eval(color);
    
    case NODE_TYPE_DIV:
    {
      float one = operands[0].n->eval(color);
      float two = operands[1].n->eval(color);
      return (two == 0 ? 0.000001 : one / two);
    }
    case NODE_TYPE_RAND:
    {
      int min = operands[0].n->eval(color);
      int max = operands[1].n->eval(color);
      
      // check if min > max -> swap them to get valid bounds
      if (min > max)
      {
        int temp = min;
        min = max;
        max = temp;
      }
      
      return (random() % (1 + max - min)) + min;
    }
    
    // inputs
    case NODE_TYPE_CONST:        return operands[0].f;
    case NODE_TYPE_RED:          return color->r;
    case NODE_TYPE_GREEN:        return color->g;
    case NODE_TYPE_BLUE:         return color->b;
    case NODE_TYPE_HUE:          return color->hue;
    case NODE_TYPE_HSV_SAT:      return color->HSVSaturation;
    case NODE_TYPE_HSL_SAT:      return color->HSLSaturation;
    case NODE_TYPE_VALUE:        return color->value;
    case NODE_TYPE_LIGHTNESS:    return color->lightness;
    case NODE_TYPE_LUMINOSITY:   return color->luminosity;
    
    // others
    case NODE_TYPE_NONE:         return 0;
  }
  
  return 0;
}

// ============================================================================
// Prints the tree in a tabbed heirarchy to stdout
// ============================================================================

void Node::print()
{
  print(stdout, 0);
}

// ============================================================================
// Prints the tree in a tabbed heirarchy
// @param file The file to print to
// ============================================================================

void Node::print(FILE* file)
{
  print(file, 0);
}

// ============================================================================
// Prints the tree in a tabbed heirarchy. Utility for print(file)
// @param file The file to print to
// @param level The indentation/tree level
// ============================================================================

void Node::print(FILE* file, int level)
{
  if (level > 0)    // using this check just to make the compiler happy
  {
    char spaces[51];       // 25 level capability
    memset(&spaces[0], ' ', level * 2);
    spaces[level * 2] = '\0';
    
    fprintf(file, "%s", spaces);
  }
  
  switch (type)
  {
    case NODE_TYPE_NONE:       fprintf(file, "NONE");           break;
    case NODE_TYPE_ADD:        fprintf(file, "ADD");            break;
    case NODE_TYPE_SUB:        fprintf(file, "SUB");            break;
    case NODE_TYPE_MUL:        fprintf(file, "MUL");            break;
    case NODE_TYPE_DIV:        fprintf(file, "DIV");            break;
    case NODE_TYPE_RAND:       fprintf(file, "RAND");           break;

    case NODE_TYPE_CONST:      fprintf(file, "CONST %f", operands[0].f); break;
    case NODE_TYPE_RED:        fprintf(file, "RED");            break;
    case NODE_TYPE_GREEN:      fprintf(file, "GREEN");          break;
    case NODE_TYPE_BLUE:       fprintf(file, "BLUE");           break;
    case NODE_TYPE_HUE:        fprintf(file, "HUE");            break;
    case NODE_TYPE_HSV_SAT:    fprintf(file, "HSV_SAT");        break;
    case NODE_TYPE_HSL_SAT:    fprintf(file, "HSL_SAT");        break;
    case NODE_TYPE_VALUE:      fprintf(file, "VALUE");          break;
    case NODE_TYPE_LIGHTNESS:  fprintf(file, "LIGHTNESS");      break;
    case NODE_TYPE_LUMINOSITY: fprintf(file, "LUMINOSITY");     break;
  }
  
  fprintf(file, "\n");
  fflush(file);
  
  for (int i = 0; i < numOperandsForType(type); i++)
  {
    operands[i].n->print(file, level + 1);
  }
}

// ============================================================================
// ============================================================================
//                                OTHER FUNCTIONS
// ============================================================================
// ============================================================================
// Returns the number of operands nodes of the given type have
// @param type The node type
// @return The number of operands nodes of the given type have
// ============================================================================

static int numOperandsForType(NodeType type)
{
  switch (type)
  {
    case NODE_TYPE_ADD:
    case NODE_TYPE_SUB:
    case NODE_TYPE_MUL:
    case NODE_TYPE_DIV:
    case NODE_TYPE_RAND:         return 2;
    
    case NODE_TYPE_CONST:
    case NODE_TYPE_RED:
    case NODE_TYPE_GREEN:
    case NODE_TYPE_BLUE:
    case NODE_TYPE_HUE:
    case NODE_TYPE_HSV_SAT:
    case NODE_TYPE_HSL_SAT:
    case NODE_TYPE_VALUE:
    case NODE_TYPE_LIGHTNESS:
    case NODE_TYPE_LUMINOSITY:
    case NODE_TYPE_NONE:         return 0;
  }
  
  return 0;
}

// ============================================================================


