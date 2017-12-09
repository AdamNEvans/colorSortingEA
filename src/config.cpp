// ============================================================================
// File:         Config.cpp
// Author:       Adam Evans
// Class:        CS 5401 section A
// Description:  Reads configuration files
// ============================================================================

#include <iostream>
#include <map>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "config.h"
#include "util.h"


using namespace std;


// ============================================================================
//                                 CONSTANTS
// ============================================================================

const char TERMINATION_STRINGS[TERMINATION_NUM_TYPES][20] = {
  "nevals",
  "no change",
  "invalid"
};

// ============================================================================
//                             Big 3 (but only 2)
// ============================================================================
// Constructs a default Config object
// ============================================================================

Config::Config()
{
  clear();
}

// ============================================================================
// Creates a Config with the information from the given file
// @param filename The name of the configuration file to read
// ============================================================================

Config::Config(const char* filename)
{
  FILE* file = fopen(filename, "r");
  map<string, map<string, string> >* params = readKeyValuePairs(file);
  
  clear();
  setVariables(*params);

  delete params;
  fclose(file);
}

// ============================================================================
// Destructs a Config object
// ============================================================================

Config::~Config()
{
  if (solutionFilename != NULL)  free(solutionFilename);
  if (logFilename != NULL)       free(logFilename);
  if (mainPopulation != NULL)    delete mainPopulation;
}

// ============================================================================
//                                 FUNCTIONS
// ============================================================================
// Sets all fields to 0 or NULL. DOES NOT FREE ALLOCATED MEMORY. Use only
// during initialization
// ============================================================================

void Config::clear()
{
  solutionFilename  = NULL;
  logFilename       = NULL;
  mainPopulation    = NULL;
  nruns             = 0;
  nevals            = 0;
  seed              = 0;
  evalPly           = 0;
  evalSize          = 0;
  terminationType   = TERMINATION_INVALID;
  ngenerations      = 0;
}

// ============================================================================
// Prints this config object
// @param file The file to print to
// ============================================================================

void Config::print(FILE* file)
{
  fprintf(file, "solutionFilename:  '%s'\n", solutionFilename);
  fprintf(file, "logFilename:       '%s'\n", logFilename);
  fprintf(file, "nruns:              %d\n",  nruns);
  fprintf(file, "nevals:             %d\n",  nevals);
  fprintf(file, "seed:               %lu\n", seed);
  fprintf(file, "evalPly:            %d\n",  evalPly);
  fprintf(file, "evalSize:           %d\n",  evalSize);
  fprintf(file, "terminationType:    %s\n",  TERMINATION_STRINGS[terminationType]);
  fprintf(file, "ngenerations:       %d\n",  ngenerations);
  
  // Print sub-configs
  fprintf(file, "\nMain Population Config:\n");
  mainPopulation->print(file);
}

// ============================================================================
// Sets variables based on the key-value pairs in the map
// @param pairs The map containing the key-value pairs
// ============================================================================

void Config::setVariables(map<string, map<string, string> >& categories)
{
  map<string, string> pairs;
  
  if (categories.find("general") != categories.end())
  {
    pairs = categories["general"];
  }
  
  // ---------------------------------------------
  // filenames/strings
  if (keyExists(pairs, "solutionFilename"))
       solutionFilename = stringToCString(pairs["solutionFilename"]);
  if (keyExists(pairs, "logFilename"))
       logFilename = stringToCString(pairs["logFilename"]);
  
  // ---------------------------------------------
  // general EA variables
  if (keyExists(pairs, "nruns"))
       sscanf(pairs["nruns"].c_str(), "%d", &nruns);
  if (keyExists(pairs, "nevals"))
       sscanf(pairs["nevals"].c_str(), "%d", &nevals);
  if (keyExists(pairs, "evalPly"))
       sscanf(pairs["evalPly"].c_str(), "%d", &evalPly);
  if (keyExists(pairs, "evalSize"))
       sscanf(pairs["evalSize"].c_str(), "%d", &evalSize);
  if (keyExists(pairs, "terminationType"))
       sscanf(pairs["terminationType"].c_str(),  "%d", (int*)&terminationType);
  if (keyExists(pairs, "ngenerations"))
       sscanf(pairs["ngenerations"].c_str(),  "%d", &ngenerations);
  
  // ---------------------------------------------
  // read the seed
  if (keyExists(pairs, "seed"))
  {
    if (pairs["seed"].compare("true") == 0)
    {
      // get time in microseconds
      struct timeval time;
      gettimeofday(&time, NULL);
      seed = (1000000 * time.tv_sec) + time.tv_usec;
    }
    else
    {
      // use the hardcoded seed from the config file
      sscanf(pairs["seed"].c_str(), "%lu", &seed);
    }
  }
  
  // ---------------------------------------------
  // read in the gameConfig variables
  mainPopulation = new PopulationConfig();
  
  if (categories.find("population") != categories.end())
  {
    mainPopulation->setVariables(categories["population"]);
  }
}

// ============================================================================
//                           CONFIG READING FUNCTIONS
// ============================================================================
// Reads in a configuration file and parses it into keys and values
// @param file The file to parse
// @return A map filled with the key-value pairs by category
// ============================================================================

map<string, map<string, string> >* Config::readKeyValuePairs(FILE* file)
{
  const int LINE_LENGTH = 1000;
  char lineBuffer[LINE_LENGTH];
  char* line = &lineBuffer[0];
  bool status;
  map<string, map<string, string> >* categories = new map<string, map<string, string> >();
  map<string, string> category = map<string, string>();
  string categoryName = "general";
  map<string, map<string, string> >::iterator location;
  
  while ((status = nextLine(file, line, LINE_LENGTH)))
  {
    if (!status)
    {
      continue;
    }
    
    // ----------------------------------------------
    // read the key-value pair
    string s = string(line);
    int index = s.find(": ");
    string keyString = s.substr(0, index);
    string key = trimString(keyString);
    string valueString = s.substr(index+2, s.size() - (index+2));
    string value = trimString(valueString);
    
    // ----------------------------------------------
    // check for a new category being created
    if (key == "CATEGORY")
    {
      // save the old category
      location = categories->find(categoryName);
      
      if (location != categories->end())
      {
        location->second = category;
      }
      else
      {
        categories->emplace(categoryName, category);
      }
      
      // ----------------------------------------------
      // create the new category or use the existing one if it already exists
      categoryName = value;
      
      if (categories->find(categoryName) != categories->end())
      {
        category = (*categories)[categoryName];
      }
      else
      {
        category = map<string, string>();
      }
    }
    else
    {
      category.emplace(key, value);
    }
  }
  
  // ----------------------------------------------
  // save the final category
  location = categories->find(categoryName);
  
  if (location != categories->end())
  {
    location->second = category;
  }
  else
  {
    categories->emplace(categoryName, category);
  }
  
  return categories;
}

// ============================================================================
// Reads the next non-blank line of the file that does not start with '#' into
// line (lines starting with '#' are considered comments and are skipped). If
// the next line is longer than the size of line, this reads only the number of
// characters that can fit in line.
// @param file The file to read the next line from
// @param line The string buffer to store the next line in
// @param size The size of the string buffer
// @return true if line was filled with a non-empty, non-comment line from file
// ============================================================================

bool Config::nextLine(FILE* file, char* line, int size)
{
  // skip comment lines and blank lines
  do 
  {
    if ((fgets(line, size, file) == NULL) && (feof(file) != 0))
    {
      return false;
    }
    
    // remove trailing newline if it exists
    int length = strlen(line) - 1;
    
    if (line[length] == '\n')
    {
        line[length] = '\0';
    }
    
  } while ((line[0] == '#') || (strlen(line) == 0));
  
  return true;
}

// ============================================================================

















