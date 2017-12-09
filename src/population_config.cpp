#include <iostream>
#include <map>
#include <string>
#include <stdio.h>
#include "population_config.h"
#include "util.h"


using std::cout;
using std::endl;
using std::map;
using std::string;


// ============================================================================
//                                 CONSTANTS
// ============================================================================

const char PARENT_STRINGS[PARENT_NUM_TYPES][20] = {
  "fitness",
  "over selection",
  "invalid"
};

const char SURVIVOR_STRINGS[SURVIVOR_NUM_TYPES][20] = {
  "truncation",
  "tournament",
  "invalid"
};

// ============================================================================
//                                 FUNCTIONS
// ============================================================================
// Sets the config variables based on the values in the given map
// @param pairs A map containing the values for the config fields
// ============================================================================

void PopulationConfig::setVariables(map<string, string>& pairs)
{
  if (keyExists(pairs, "parentSelectionType"))
    sscanf(pairs["parentSelectionType"].c_str(), "%d", (int*)&parentSelectionType);
  if (keyExists(pairs, "survivorSelectionType"))
    sscanf(pairs["survivorSelectionType"].c_str(), "%d", (int*)&survivorSelectionType);
  
  if (keyExists(pairs, "populationSize"))
    sscanf(pairs["populationSize"].c_str(), "%d", &populationSize);
  if (keyExists(pairs, "numKids"))
    sscanf(pairs["numKids"].c_str(), "%d", &numKids);
  if (keyExists(pairs, "survivork"))
    sscanf(pairs["survivork"].c_str(), "%d", &survivork);
  if (keyExists(pairs, "treeHeight"))
    sscanf(pairs["treeHeight"].c_str(), "%d", &treeHeight);
  if (keyExists(pairs, "mutationChance"))
    sscanf(pairs["mutationChance"].c_str(), "%f", &mutationChance);
  if (keyExists(pairs, "parsimonyPressure"))
    sscanf(pairs["parsimonyPressure"].c_str(), "%f", &parsimonyPressure);
  if (keyExists(pairs, "overSelectionPercent"))
    sscanf(pairs["overSelectionPercent"].c_str(), "%f", &overSelectionPercent);
  
  // calculate stuff based off of selection and survival options
  maxPopulation = populationSize + numKids;
}

// ============================================================================
// Prints this config to the given file
// @param file The file to print to
// ============================================================================

void PopulationConfig::print(FILE* file)
{
  fprintf(file, "parentSelectionType:    %s\n", PARENT_STRINGS[parentSelectionType]);
  fprintf(file, "survivorSelectionType:  %s\n", SURVIVOR_STRINGS[survivorSelectionType]);
  fprintf(file, "maxPopulation:          %d\n", maxPopulation);
  fprintf(file, "populationSize:         %d\n", populationSize);
  fprintf(file, "numKids:                %d\n", numKids);
  fprintf(file, "survivork:              %d\n", survivork);
  fprintf(file, "treeHeight:             %d\n", treeHeight);
  fprintf(file, "mutationChance:         %f\n", mutationChance);
  fprintf(file, "parsimonyPressure:      %f\n", parsimonyPressure);
  fprintf(file, "overSelectionPercent:   %f\n", overSelectionPercent);
}

// ============================================================================

