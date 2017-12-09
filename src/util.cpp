// ============================================================================
// File:         util.cpp
// Author:       Adam Evans
// Class:        CS 5401 section A
// Description:  Allows sampling from the normal distribution N(0, 1)
// ============================================================================

#include <iostream>
#include <random>
#include <string.h>
#include "util.h"

using namespace std;

static default_random_engine randomGenerator;
static normal_distribution<double> normalDist;

// ========================================================================
// Prepares the normal distribution to be sampled
// @param seed The seed to seed the random number generator with
// ============================================================================

void initializeNormalDistribution(unsigned long seed)
{
  randomGenerator = default_random_engine(seed);
  normalDist = normal_distribution<double>(0.0, 1.0);
}

// ========================================================================
// Samples the normal distribution with a mean of 0 and std dev of 1
// initializeNormalDistribution(seed) must be called before this function can
// be called.
// @return The value sampled
// ============================================================================

double normal()
{
  return normalDist(randomGenerator);
}

// ========================================================================
// Returns a char* string that is equivalent, but not tied to the given
// string object
// @param in The string to convert
// @return The C string
// ========================================================================

char* stringToCString(string& in)
{
  int length = in.length();
  char* output = (char*)malloc(sizeof(char) * (length + 1));
  memcpy(output, in.c_str(), length);
  output[length] = '\0';
  
  return output;
}

// ========================================================================
// Returns true if the given key exists in the given map
// @param dict The map
// @param key The key
// @return True if the given key exists in the given map
// ========================================================================

bool keyExists(map<string, string>& dict, string key)
{
  return dict.find(key) != dict.end();
}

// ============================================================================
// Removes whitespace from the ends of the string and returns the result
// @param in The string to trim
// @return The trimmed string
// ============================================================================

string trimString(string& in)
{
  int start = 0;
  
  while ((in[start] == ' ') || (in[start] == '\t') || (in[start] == '\n'))
  {
    start++;
  }
  
  int end = in.length() - 1;
  
  while ((in[end] == ' ') || (in[end] == '\t') || (in[end] == '\n'))
  {
    end--;
  }
  
  string out = in.substr(start, 1 + end - start);
  return out;
}

// ========================================================================




















