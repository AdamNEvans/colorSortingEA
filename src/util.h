// ============================================================================
// File:         util.h
// Author:       Adam Evans
// Class:        CS 5401 section A
// Description:  Allows sampling from the normal distribution N(0, 1)
// ============================================================================


#include <map>
#include <string>


#ifndef UTIL_H
#define UTIL_H


// ============================================================================
// Prepares the normal distribution to be sampled
// @param seed The seed to seed the random number generator with
// ============================================================================

void initializeNormalDistribution(unsigned long seed);

// ============================================================================
// Samples the normal distribution with a mean of 0 and std dev of 1
// initializeNormalDistribution(seed) must be called before this function can
// be called.
// @return The value sampled
// ============================================================================

double normal();

// ============================================================================
// Returns a char* string that is equivalent, but not dependent upon the given
// string object
// @param in The string to convert
// @return The C string
// ============================================================================

char* stringToCString(std::string& in);

// ============================================================================
// Returns true if the given key exists in the given map
// @param dict The map
// @param key The key
// @return True if the given key exists in the given map
// ============================================================================

bool keyExists(std::map<std::string, std::string>& dict, std::string key);

// ============================================================================
// Removes whitespace from the ends of the string and returns the result
// @param in The string to trim
// @return The trimmed string
// ============================================================================

std::string trimString(std::string& in);

// ============================================================================


#endif
