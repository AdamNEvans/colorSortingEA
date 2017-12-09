#include <map>
#include <string>
#include <stdio.h>


#ifndef POPULATION_CONFIG_H
#define POPULATION_CONFIG_H


// ============================================================================
// Possible values for GA options
// ============================================================================


typedef enum
{ 
  PARENT_FITNESS = 0,
  PARENT_OVER_SELECT,
  PARENT_INVALID,
  PARENT_NUM_TYPES
} ParentSelectionType;


typedef enum
{ 
  SURVIVOR_TRUNCATION = 0,
  SURVIVOR_TOURNAMENT,
  SURVIVOR_INVALID,
  SURVIVOR_NUM_TYPES
} SurvivorSelectionType;


// ============================================================================
// Holds all the option values for a single population
// ============================================================================


class PopulationConfig
{
  public:
    // GA options
    ParentSelectionType   parentSelectionType;
    SurvivorSelectionType survivorSelectionType;

    // option parameters
    int    maxPopulation;
    int    populationSize;
    int    numKids;
    int    survivork;
    int    treeHeight;
    float  mutationChance;
    float  parsimonyPressure;
    float  overSelectionPercent;
    
    // ========================================================================
    // Sets the config variables based on the values in the given map
    // @param pairs A map containing the values for the config fields
    // ========================================================================
    
    void setVariables(std::map<std::string, std::string>& pairs);
    
    // ========================================================================
    // Prints this config to the given file
    // @param file The file to print to
    // ========================================================================
    
    void print(FILE* file);
    
    // ========================================================================
};


#endif
