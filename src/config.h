// ============================================================================
// File:         Config.h
// Author:       Adam Evans
// Class:        CS 5401 section A
// Description:  Reads configuration files
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "population_config.h"


#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_LINE_SIZE 500


// ============================================================================


typedef enum
{
  TERMINATION_NEVALS = 0,
  TERMINATION_NO_CHANGE,
  TERMINATION_INVALID,
  TERMINATION_NUM_TYPES
} TerminationType;


// ============================================================================
// Holds information found in a configuration file
// ============================================================================

class Config
{
  public:
    // general fields
    char*           solutionFilename;
    char*           logFilename;
    char*           mylogFilename;
    
    int             nruns;
    int             nevals;
    unsigned long   seed;
    int             evalPly;
    int             evalSize;
    
    TerminationType terminationType;
    int             ngenerations;
    
    // population config(s)
    PopulationConfig* mainPopulation;
    
    // ========================================================================
    // Constructs an default Config object
    // ========================================================================
    
    Config();
    
    // ========================================================================
    // Creates a Config with the information from the given file
    // @param filename The name of the configuration file to read
    // ========================================================================
    
    Config(const char* filename);
    
    // ========================================================================
    // Destructs the Config object
    // ========================================================================
    
    virtual ~Config();
    
    // ========================================================================
    // Prints a config object
    // @param file The file to print to
    // ========================================================================
    
    virtual void print(FILE* file);
    
    // ========================================================================
    
  protected:
    
    // ========================================================================
    // Sets variables based on the key-value pairs in the map
    // @param pairs The map containing the key-value pairs
    // ========================================================================
    
    virtual void setVariables(std::map<std::string, std::map<std::string, std::string> >& pairs);
    
    // ========================================================================
    
  private:
    
    // ========================================================================
    // Sets all fields to 0 or NULL. DOES NOT FREE ALLOCATED MEMORY. Use only
    // during initialization
    // ========================================================================
    
    void clear();
    
    // ========================================================================
    // Reads in a configuration file and parses it into keys and values
    // @param file The file to parse
    // @return A map filled with the key-value pairs by category
    // ========================================================================
    
    std::map<std::string, std::map<std::string, std::string> >* readKeyValuePairs(FILE* file);
    
    // ========================================================================
    // Reads the next non-blank line of the file that does not start with '#'
    // into line (lines starting with '#' are considered comments and are
    // skipped). If the next line is longer than the size of line, this reads
    // only the number of characters that can fit in line.
    // @param file The file to read the next line from
    // @param line The string buffer to store the next line in
    // @param size The size of the string buffer
    // @return true if line was filled with a non-empty, non-comment line from
    // file
    // ========================================================================
    
    bool nextLine(FILE* file, char* line, int size);
    
    // ========================================================================
};


#endif
