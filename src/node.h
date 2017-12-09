#ifndef NODE_H
#define NODE_H


#include "color.h"


// ==========================================================================
//                               NODE_TYPES
// ==========================================================================


#define NODE_TYPE_NUM_TYPES    15
#define NODE_TYPE_ALL_NODES    ((1 << NODE_TYPE_NUM_TYPES) - 1)


typedef enum
{
  // functions
  NODE_TYPE_NONE         = 0,
  NODE_TYPE_ADD          = 1 << 0,
  NODE_TYPE_SUB          = 1 << 1,
  NODE_TYPE_MUL          = 1 << 2,
  NODE_TYPE_DIV          = 1 << 3,
  NODE_TYPE_RAND         = 1 << 4,
  
  // inputs
  NODE_TYPE_CONST        = 1 << 5,
  NODE_TYPE_RED          = 1 << 6,
  NODE_TYPE_GREEN        = 1 << 7,
  NODE_TYPE_BLUE         = 1 << 8,
  NODE_TYPE_HUE          = 1 << 9,
  NODE_TYPE_HSV_SAT      = 1 << 10,
  NODE_TYPE_HSL_SAT      = 1 << 11,
  NODE_TYPE_VALUE        = 1 << 12,
  NODE_TYPE_LIGHTNESS    = 1 << 13,
  NODE_TYPE_LUMINOSITY   = 1 << 14,
} NodeType;


// ==========================================================================
//                                 OPERAND
// ==========================================================================
// forward declare Node so Operand can use it
class Node;

typedef union
{
  Node* n;
  float f;    // support for constants
} Operand;

// ==========================================================================
//                                  NODE
// ==========================================================================


#define NODE_MAX_OPERANDS   2


class Node
{
  public:
    int        validNodes;       // mask of the valid node types
    int        maxHeight;        // the max height of this node's subtree
    int        treeSize;         // how many nodes in this node's subtree
    NodeType   type;             // what operation this node represents
    Operand    operands[NODE_MAX_OPERANDS];   // operation operands
    
    // ========================================================================
    // Constructs a default node
    // ========================================================================
    
    Node();
    
    // ========================================================================
    // Constructs a Node using the ramped half and half grow method.
    // @param inHeight The max height the tree is allowed to grow to
    // @param nodes The valid nodes mask
    // ========================================================================
    
    Node(int inHeight, int nodes);
    
    // ========================================================================
    // Destructs a Node
    // ========================================================================
    
    ~Node();
    
    // ========================================================================
    // Creates a tree with this Node as the root using the ramped half and half
    // grow method. Deletes old nodes to accomodate the new nodes.
    // @param inHeight The max height the tree is allowed to grow to
    // @param grow True if the grow method should be used. False if the full
    // method should be used.
    // ========================================================================
    
    void createSubtree(int inHeight, bool grow);
    
    // ========================================================================
    // Mutates the tree rooted at the index'th node in the preorder traversal of
    // the tree rooted at this node.
    // @param index The index of the root of the tree to mutate
    // ========================================================================
    
    void mutateSubtree(int index);
    
    // ========================================================================
    // Recombines the tree rooted at this node with the tree rooted at other to
    // produce a third tree which is returned. This tree serves as the base tree
    // with the replacement tree being provided by other.
    // @param other The other tree to recombine with
    // @return The new tree generated
    // ========================================================================
    
    Node* recombine(Node* other);
    
    // ========================================================================
    // Returns a deep copy of this tree
    // @return A deep copy of this tree
    // ========================================================================
    
    Node* copy();
    
    // ========================================================================
    // Returns the index'th node of the tree rooted at this node
    // @param index The index of the node to get in the preorder traversal of
    // this tree
    // @return The index'th node of the tree rooted at this node in the
    // preorder traversal. Returns NULL if index out of bounds.
    // ========================================================================
    
    Node* getNode(int index);
    
    // ========================================================================
    // Sets the node at the given index to be the given tree (no copy)
    // @param index The index of the node to replace
    // @param tree The replacement tree/node
    // @return A pointer to the node that was replaced (and its tree)
    // ========================================================================
    
    Node* setNode(int index, Node* tree);
    
    // ========================================================================
    // Evaluates this node for the given sensor inputs
    // @param data The data to evaluate with
    // @return The value of the subtree rooted at this node
    // ========================================================================
    
    float eval(void* data);
    
    // ========================================================================
    // Prints the tree in a tabbed heirarchy to stdout
    // ========================================================================
    
    void print();
    
    // ========================================================================
    // Prints the tree in a tabbed heirarchy
    // @param file The file to print to
    // ========================================================================
    
    void print(FILE* file);
  
  private:
    
    // ========================================================================
    // Prints the tree in a tabbed heirarchy. Utility for print(file)
    // @param file The file to print to
    // @param level The indentation/tree level
    // ========================================================================
    
    void print(FILE* file, int level);
};

// ==========================================================================

#endif
