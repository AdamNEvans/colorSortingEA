#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "member.h"


// ============================================================================
//                                  Big 3
// ============================================================================
// Constructs a default Member
// ============================================================================

Member::Member()
{
  fitness = -100000000;
  fitnessSum = 0;
  nfitnessEvals = 0;
  root = NULL;
}

// ============================================================================
// Constructs a Member and initializes the evaluation tree
// with the given tree height
// @param treeHeight Maximum evaluation tree height
// @param validNodes A mask of valid nodes for the evaluation tree
// ============================================================================

Member::Member(int treeHeight, int validNodes)
{
  fitness = -100000000;
  fitnessSum = 0;
  nfitnessEvals = 0;
  root = new Node(treeHeight, validNodes);
}

// ============================================================================
// Destructs a Member
// ============================================================================

Member::~Member()
{
  if (root != NULL)
  {
    delete root;
  }
}

// ============================================================================
//                             Normal Functions
// ============================================================================
// Evaluates a given Member
// @return The value of the evaluation
// ============================================================================

float Member::evaluate(void* data)
{
  if (root == NULL)
  {
    printf("ERROR: root is NULL -> can't evaluate. Aborting\n");
    exit(1);
  }
  
  return root->eval(data);
}

// ============================================================================
//                         MUTATION AND RECOMBINATION
// ============================================================================
// Mutates this member's evaluation tree
// ============================================================================

void Member::mutate()
{
  if (root == NULL)
  {
    printf("ERROR: bad mutation - root is NULL. Quitting...\n");
    exit(1);
  }
  
  int index = random() % root->treeSize;   // randomly choose a node
  root->mutateSubtree(index);              // mutate it
}

// ============================================================================
// Recombines this member with another to produce a third
// @param other The other member to recombine with
// @return The resulting Member
// ============================================================================

Member* Member::recombine(Member* other)
{
  if (root == NULL)
  {
    printf("ERROR: bad recombination - root is NULL. Quitting...\n");
    exit(1);
  }
  
  Member* newMember = new Member();
  newMember->root = root->recombine(other->root);
  
  return newMember;
}

// ============================================================================
// Adds a game's fitness to this member
// @param f The fitness to add
// ============================================================================

void Member::addFitness(float f)
{
  fitnessSum += f;
  nfitnessEvals++;
}

// ============================================================================
// Computes the true fitness from fitnessSum and nfitnessEvals (the average
// fitness)
// ============================================================================

void Member::computeFitness()
{
  fitness = fitnessSum / nfitnessEvals;
  fitnessSum = 0;
  nfitnessEvals = 0;
}

// ============================================================================
//                                 SORTING
// ============================================================================
// Sorts Members by decreasing fitness. Used with qsort()
// @param inone Pointer to the first Member*
// @param intwo Pointer to the second Member*
// @return 1 if inone's fitness < intwo's fitness
//        -1 if inone's fitness > intwo's fitness
//         0 otherwise
// ============================================================================

int memberCompareFitness(const void* inone, const void* intwo)
{
  Member* one = *(Member**)inone;
  Member* two = *(Member**)intwo;
  
  if (one->fitness > two->fitness)  return -1;
  if (one->fitness < two->fitness)  return 1;
  return 0;
}

// ============================================================================
