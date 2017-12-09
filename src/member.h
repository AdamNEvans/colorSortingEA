#ifndef MEMBER_H
#define MEMBER_H


#include "node.h"


class Member
{
  public:
    float      fitness;         // the fitness of this member (their score)
    float      fitnessSum;      // the sum of fitnesses from ngames games
    int        nfitnessEvals;   // how many evals this member has participated
                                // in to arrive at fitnessSum. Used to get avg
                                // fitness.
    Node*      root;            // the root of the fitness function tree
    
    // =======================================================================
    // Constructs a default Member
    // =======================================================================
    
    Member();
    
    // =======================================================================
    // Constructs a Member and initializes the evaluation tree
    // with the given tree height
    // @param treeHeight Maximum evaluation tree height
    // @param validNodes A mask of valid nodes for the evaluation tree
    // =======================================================================
    
    Member(int treeHeight, int validNodes);
    
    // ===================================================================
    // Destructs a Member
    // =======================================================================
    
    virtual ~Member();
    
    // =======================================================================
    // Evaluate this member and returns the fitness value
    // @Return The value for the given data
    // =======================================================================
    
    virtual float evaluate(void* data);
    
    // =======================================================================
    // Mutates this member's evaluation tree
    // =======================================================================
    
    void mutate();
    
    // =======================================================================
    // Recombines this member with another to produce a third
    // @param other The other member to recombine with
    // @return The resulting Member
    // =======================================================================
    
    virtual Member* recombine(Member* other);
    
    // =======================================================================
    // Adds another game's fitness to this member
    // @param f The fitness to add
    // =======================================================================
    
    void addFitness(float f);
    
    // =======================================================================
    // Computes the true fitness from fitnessSum and ngames (the average
    // fitness over the ngames games played)
    // =======================================================================
    
    void computeFitness();
    
    // =======================================================================
};


// ===========================================================================
// Sorts Members by decreasing fitness. Used with qsort()
// @param inone Pointer to the first Member*
// @param intwo Pointer to the second Member*
// @return 1 if inone's fitness < intwo's fitness
//        -1 if inone's fitness > intwo's fitness
//         0 otherwise
// ===========================================================================

int memberCompareFitness(const void* inone, const void* intwo);

// ===========================================================================


#endif
