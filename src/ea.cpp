#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <math.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include "ea.h"
#include "color.h"
#include "member.h"
#include "config.h"


using std::cout;
using std::endl;
using std::vector;
using std::find;


// =============================================================================
//                                  CONSTANTS
// =============================================================================


#define MAX_FITNESS       10000000


// =============================================================================
//                            FUNCTION DECLARATIONS
// =============================================================================
// see function definitions for documentation

// generic functions
static void writeLogEntry(FILE* file, Member** members, int size);
static void writeMylogEntry(FILE* file);
static void deleteMember(Member* dude);
static bool evaluateMember(Member* dude, Config* config);
static int tournament(Member** members, int start, int end, int nparticipants);
static uint64_t* getFitnessProportionalChances(Member** members, int start, int end);
static Member* kidFromParents(Member* one, Member* two, PopulationConfig* config);
static float getParsimonyPenalty(Member* members, PopulationConfig* config);

// takes a population and population config so they are population agnostic
static int initializeGrow                 (Member** members, PopulationConfig* config);
static bool terminationNevals             (Member** members, int size);
static bool terminationNoChange           (Member** members, int size);
static bool terminationInvalid            (Member** members, int size);
static int parentSelectionFitness         (Member** members, int size, PopulationConfig* config);
static int parentSelectionOverSelection   (Member** members, int size, PopulationConfig* config);
static int parentSelectionInvalid         (Member** members, int size, PopulationConfig* config);
static int survivorSelectionTruncate      (Member** members, int size, PopulationConfig* config);
static int survivorSelectionTournament    (Member** members, int size, PopulationConfig* config);
static int survivorSelectionInvalid       (Member** members, int size, PopulationConfig* config);


// =============================================================================
//                              FUNCTION TABLES
// =============================================================================


bool (*termination[TERMINATION_NUM_TYPES])(Member** members, int size) = {
  terminationNevals,
  terminationNoChange,
  terminationInvalid
};

int (*parentSelection[PARENT_NUM_TYPES])(Member** members, int size, PopulationConfig* config) = {
  parentSelectionFitness,
  parentSelectionOverSelection,
  parentSelectionInvalid
};

int (*survivorSelection[SURVIVOR_NUM_TYPES])(Member** members, int size, PopulationConfig* config) = {
  survivorSelectionTruncate,
  survivorSelectionTournament,
  survivorSelectionInvalid
};


// =============================================================================
//                               GLOBAL VARIABLES
// =============================================================================


static Config* mainConfig = NULL;     // the main config object
static Member** population = NULL;    // the EA population
static Member* bestMember = NULL;     // the best dude found during the current run

static int populationSize = 0;        // current size of the EA population
static int totalEvals = 0;            // total number of evals so far

static float highestFitness = -9999999999999;   // the highest fitness ever


// =============================================================================
// =============================================================================
//                                  FUNCTIONS
// =============================================================================
// =============================================================================
// Executes the EA/GP
// @param inconfig The config to use
// =============================================================================

void executeEA(Config* inconfig)
{
  mainConfig = inconfig;
  
  // ---------------------------------------------
  // Prepare the global files we need
  // ---------------------------------------------
  FILE* logfile = fopen(mainConfig->logFilename, "w");
  FILE* mylogfile = fopen(mainConfig->mylogFilename, "w");

  if (logfile == NULL)
  {
    printf("ERROR: Could not open log file. Aborting...\n");
    exit(1);
  }
  if (mylogfile == NULL)
  {
    printf("ERROR: Could not open mylog file. Aborting...\n");
    exit(1);
  }
  
  fprintf(logfile, "Result Log\n\n");
  fprintf(mylogfile, "My Log\n");
  mainConfig->print(logfile);
  
  // ---------------------------------------------
  // Do the search
  // ---------------------------------------------
  for (int run = 0; run < mainConfig->nruns; run++)
  {
    // do a run
    printf("====================================================================\n");
    printf("                          Run %d\n", run + 1);
    printf("====================================================================\n");
    fprintf(logfile, "\nRun %d\n", run + 1);
    fprintf(mylogfile, "\nRun %d\n", run + 1);
    
    totalEvals = 0;
    
    // ---------------------------------------------
    // prepare the population
    printf("Initializing population...\n");
    population = (Member**)(new Member*[mainConfig->mainPopulation->maxPopulation]);
    populationSize = initializeGrow(population, mainConfig->mainPopulation);
    
    // ---------------------------------------------
    // run the evolution
    // ---------------------------------------------
    int generation = 0;
    int type;
    
    while (!termination[mainConfig->terminationType](population, populationSize))
    {
//      printf("maxEvals=%d  currentEvals=%d\n", mainConfig->nevals, 
      printf("------------------------ generation %d -----------------------\n", generation);
      
      type = mainConfig->mainPopulation->parentSelectionType;
      populationSize = parentSelection[type](population, populationSize, mainConfig->mainPopulation);
      
      type = mainConfig->mainPopulation->survivorSelectionType;
      populationSize = survivorSelection[type](population, populationSize, mainConfig->mainPopulation);
      
      // ---------------------------------------------
      // check for the best member (for mylog purposes)
      for (int i = 0; i < populationSize; i++)
      {
        if ((bestMember == NULL) || (population[i]->fitness > bestMember->fitness))
        {
          Member* m = bestMember;
          
          // need to change bestMember before calling deleteMember()
          bestMember = population[i];
          deleteMember(m);
        }
      }
      
      // -------------------------------------
      // make the log entries
      writeLogEntry(logfile, population, populationSize);
      writeMylogEntry(mylogfile);
      
      generation++;
    }
    
    // ---------------------------------------------
    // cleanup
    // ---------------------------------------------
    bestMember = NULL;    // TODO: why set to NULL then delete?
    deleteMember(bestMember);
    
    for (int i = 0; i < populationSize; i++)
    {
      delete population[i];
    }
    
    delete population;
  }
  
  // ---------------------------------------------
  // clean up
  // ---------------------------------------------
  fclose(logfile);
  fclose(mylogfile);
}

// =============================================================================
//                                   LOGGING
// =============================================================================
// Writes an entry to the given log file
// @param file The log file
// @param members The population who's state is to be logged
// @param size The size of the population
// =============================================================================

static void writeLogEntry(FILE* file, Member** members, int size)
{
  float avg = 0;
  float best = -1;
  bool setBest = false;
  
  for (int i = 0; i < size; i++)
  {
    avg += members[i]->fitness;
    
    if ((members[i]->fitness > best) || (!setBest))
    {
      best = members[i]->fitness;
      setBest = true;
    }
  }
  
  avg /= size;
  
  fprintf(file, "%d\t%f\t%f\n", totalEvals, avg, best);
}

// =============================================================================
// Writes an entry in the given mylog file
// @param file The mylog file
// =============================================================================

static void writeMylogEntry(FILE* file)
{
  fprintf(file, "%d\t%f\n", totalEvals, bestMember->fitness);
}

// =============================================================================
// Deletes the given member if not used in any other tracking variables. Must
// set the variable storing member to NULL before calling this so this function's
// usage checks don't think it is being used where you are trying to delete it.
// @param member The member to delete
// =============================================================================

static void deleteMember(Member* member)
{
  // check if this member is being used still in the population array/population
  for (int i = 0; i < populationSize; i++)
  {
    if (population[i] == member)
    {
      return;
    }
  }
  
  // check if this member is the best member
  if (bestMember == member)
  {
    return;
  }
  
  delete member;
}

// =============================================================================
// =============================================================================
// =============================================================================
//                                  EA STUFF
// =============================================================================
// =============================================================================
// =============================================================================
//                             GENERAL FUNCTIONS
// =============================================================================
// Evaluates a member
// @param member The member to evaluate
// @return True if the membet was evaluated (we had evals left)
// =============================================================================

static bool evaluateMember(Member* member, Config* config)
{
  if (terminationNevals(population, populationSize))
  {
    member->fitness = -10000000;
    return false;      // don't evaluate
  }
  
  // ---------------------------------------------
  // compute fitness
  float fitness = 0;
  Color* colorBuffer = (Color*)malloc(sizeof(Color) * config->evalSize);
  ColorPair* list = (ColorPair*)malloc(sizeof(ColorPair) * config->evalSize);
  
  for (int ply = 0; ply < config->evalPly; ply++)
  {
    // initialize
    for (int i = 0; i < config->evalSize; i++)
    {
      Color* color = &colorBuffer[i];
      initializeColorWith(color, rand() % 256, rand() % 256, rand() % 256);
      list[i].color = color;
      list[i].value = member->evaluate(color);
    }
    
    // evaluate the member
    qsort(list, config->evalSize, sizeof(list[0]), colorPairComparator);
    fitness += sortError(list, config->evalSize);
  }
  
  fitness /= 1000;       // get into manageable numbers
  fitness /= config->evalSize;
  
  // we want low error, so subtract from the max so we can "maximize fitness"
  fitness = MAX_FITNESS - fitness;
  
  // add parismony pressure
  fitness += getParsimonyPenalty(member, mainConfig->mainPopulation);
  
  member->addFitness(fitness);
  member->computeFitness();
  totalEvals++;
  
  // ---------------------------------------------
  // check for a highest fitness
  if (fitness > highestFitness)
  {
    // track the highest fitness member and store it as the solution
    printf("eval %4d got highest fitness of %3.2f beating old highest fitness of %3.2f\n",
           totalEvals - 1, fitness, highestFitness);
    highestFitness = fitness;
    
    // log the member's evaluation tree
    FILE* file = fopen(mainConfig->solutionFilename, "w");
    
    if (file == NULL)
    {
      printf("ERROR: could not open solution file with error %d. Aborting...\n", errno);
      exit(1);
    }
    
    member->root->print(file);
    member->root->print(stdout);
    fclose(file);
  }
  
  return true;
}

// =============================================================================
// Runs a tournament on the given member range and returns the index of the winner
// @param members The array of members to run the tournament on
// @param start The first index of the range of valid participants; range=[start,end)
// @param end The index AFTER the index of the last valid participant
// @param nparticipants The size of the tournament (k)
// @return The index in members of the winner of the tournament
// =============================================================================

static int tournament(Member** members, int start, int end, int nparticipants)
{
  bool valid;
  int index;
  int winner = -1000000;
  int range = start - end;
  float bestFitness = -10000000;
  vector<int> contestants = vector<int>(nparticipants, 0);
  
  // select the contestants
  for (int i = 0; i < nparticipants; i++)
  {
    // choose a contestant not already in the tournament
    do
    {
      valid = true;
      index = (random() % range) + start;
      
      // make sure we haven't already selected this member
      for (int j = 0; j < i; j++)
      {
        if (contestants[j] == index)
        {
          valid = false;
          break;
        }
      }
    }
    while (!valid);
    
    // add the member as a contestant
    contestants[i] = index;
    
    // see if this member is the best in the tournament
    if ((members[index]->fitness > bestFitness))
    {
      bestFitness = members[index]->fitness;
      winner = index;
    }
  }
  
  return winner;
}

// =============================================================================
// Returns a dynamically allocated array of chances for each member to be chosen
// given a random value.
// @param members The population to get the chances for
// @param start The first index 
// @param end The index AFTER the last index to get chances for
// @return The chance array
// =============================================================================

static uint64_t* getFitnessProportionalChances(Member** members, int start, int end)
{
  int size = end - start;
  uint64_t* chances = (uint64_t*)malloc(sizeof(uint64_t) * size);
  int count = 0;   // number of members with the min fitness
  float sum = 0;
  float min = 10000;
  
  // find the minimum fitness and the sum of all (fitness - min) values
  for (int i = start; i < end; i++)
  {
    sum += members[i]->fitness;
    
    if (members[i]->fitness < min)
    {
      min = members[i]->fitness;
      count = 0;
    }
    
    if (members[i]->fitness == min)
    {
      count++;
    }
  }
  
  sum += count;       // give each member with the min fitness a single chance
  sum -= min * size;
  
  // populate the chances array
  float value = 0;
  
  for (int i = 0; i < size; i++)
  {
    // make sure every member gets a chance
    value += (members[i + start]->fitness == min ? 1 : members[i + start]->fitness - min);
    chances[i] = (uint64_t)value;
  }
  
  return chances;
}

// =============================================================================
// Creates a kid from the two given members, based off of one
// @param one The first parent; the base for the kid
// @param two The second parent
// @param config The population config to use
// @return The new kid, NULL if could not evaluate the kid (not enough evals)
// =============================================================================

static Member* kidFromParents(Member* one, Member* two, PopulationConfig* config)
{
  Member* kid = one->recombine(two);
  
  if (random() < INT_MAX * config->mutationChance)
  {
    kid->mutate();
  }
  
  // only return the kid of we successfully evaluated it
  if (!evaluateMember(kid, mainConfig))
  {
    deleteMember(kid);
    return NULL;
  }
  
  return kid;
}

// =============================================================================
// Returns the parsimony penalty for the given member
// @param member The member
// @param config The population config for the member's population
// @return The fitness modifier due to parsimony pressure
// =============================================================================

static float getParsimonyPenalty(Member* member, PopulationConfig* config)
{
  return member->root->treeSize * config->parsimonyPressure;
}

// =============================================================================
//                               INITIALIZATION
// =============================================================================
// Creates the population via the ramped half and half grow method
// @param members The population
// @param config The population config for the given population
// @return The new population size
// =============================================================================

static int initializeGrow(Member** members, PopulationConfig* config)
{
  // create the members
  for (int i = 0; i < config->populationSize; i++)
  {
    members[i] = new Member(config->treeHeight, NODE_TYPE_ALL_NODES);
    
    // evaluate the member
    if (!evaluateMember(members[i], mainConfig))
    {
      printf("ERROR: not enough evals for initial population\n");
      return i;
    }
  }
  
  // null out the rest for easier debugging
  for (int i = config->populationSize; i < config->maxPopulation; i++)
  {
    members[i] = NULL;
  }
  
  return config->populationSize;
}

// =============================================================================
//                                TERMINATION
// =============================================================================
// Returns true if totalEvals >= config->nevals
// @param members The population (unused)
// @param size The size of population (unused)
// @return true if totalEvals >= config->evals
// =============================================================================

static bool terminationNevals(Member** members, int size)
{
  // avoid the "usused args" warnings
  if (false)
  {
    members = &members[0];
    size = size + 1;
  }
  
  return totalEvals >= mainConfig->nevals;
}

// =============================================================================
// Returns true if there has been no change in best fitness for ngenerations
// @param members The population
// @param size The size of population
// @return True if there has been no change in best fitness for ngenerations.
// Returns false if size is 0.
// =============================================================================

static bool terminationNoChange(Member** members, int size)
{
  static float bestFitness = -100000;
  static int   noChangeCount = 0;     // number of generations with this fitness
  
  if (size <= 0)
  {
    return false;
  }
  
  float fitness = members[0]->fitness;
  
  if (fitness == bestFitness)
  {
    noChangeCount++;
    
    if (noChangeCount == mainConfig->ngenerations)
    {
      bestFitness = -100000;    // reset the fitness to reset the count
      noChangeCount = 0;
      return true;
    }
    
    return false;
  }
  
  // reset the count if different
  noChangeCount = 1;
  bestFitness = fitness;
  
  return false;
}

// =============================================================================
// Calling this function is an error. Attempts to exit program after printing
// an error message. Returns true if fails to exit.
// @param members The population (unused)
// @param size The size of population (unused)
// @return True if fails to exit. Exits before returning.
// =============================================================================

static bool terminationInvalid(Member** members, int size)
{
  // here to prevent unused arg warnings
  if (false)
  {
    members = &members[8];
    size = size + 2;
  }
  
  printf("ERROR: Invalid termination option\n");
  exit(1);
  
  return true;
}

// =============================================================================
//                              PARENT SELECTION
// =============================================================================
// Selects parents via fitness proportional selection, and makes kids
// @param members The population
// @param size The size of population
// @param config The population config for members
// @pre config->numKids + size <= capacity of members
// @return The new population size after making kids
// =============================================================================

static int parentSelectionFitness(Member** members, int size, PopulationConfig* config)
{
  // ----------------------------------
  // get chance array
  uint64_t* chances = getFitnessProportionalChances(members, 0, size);
  uint64_t maxValue = chances[size - 1];
  
  // ----------------------------------
  // loop
  for (int i = 0; i < config->numKids; i++)
  {
    // pick parent 1
    uint64_t index1 = random() % maxValue;
    
    for (int j = 0; j < size; j++)
    {
      if (chances[j] >= index1)
      {
        index1 = j;
        break;
      }
    }
    
    // pick parent 2
    uint64_t index2 = random() % maxValue;
    
    for (int j = 0; j < size; j++)
    {
      if (chances[j] >= index2)
      {
        index2 = j;
        break;
      }
    }
    
    // make kid
    members[size + i] = kidFromParents(members[index1], members[index2], config);
    
    if (members[size + i] == NULL)
    {
      free(chances);   // cleanup
      return size + i;
    }
  }
  
  // be a decent human being
  free(chances);
  
  return size + config->numKids;
}

// =============================================================================
// Selects parents via over selection, and makes kids
// @param members The population
// @param size The size of population
// @param config The population config for members
// @return The new population size after making kids
// =============================================================================

static int parentSelectionOverSelection(Member** members, int size, PopulationConfig* config)
{
  float multiplier = 1 - (config->overSelectionPercent / 100);
  int limit = (int)round(size * multiplier);    // find the 80% boundary
  
  // make sure we can be successful
  if (limit == 0)
  {
    printf("ERROR: lower section is 0. Increase population size or decrease"\
           " overSelectionPercent.\n");
    exit(1);
  }
  else if (limit == size)
  {
    printf("ERROR: upper section is 0. Increase population size or increase"\
           " overSelectionPercent.\n");
    exit(1);
  }
  
  // ----------------------------------------------
  // sort the population by fitness
  qsort(members, size, sizeof(Member*), memberCompareFitness);
  
  // ----------------------------------------------
  // get the chances for each member to be chosen
  uint64_t* chances1 = getFitnessProportionalChances(members, 0, limit);
  uint64_t* chances2 = getFitnessProportionalChances(members, limit, size);
  int chances1size = limit;
  int chances2size = size - limit;
  uint64_t chances1max = chances1[chances1size - 1];
  uint64_t chances2max = chances2[chances2size - 1];
  int parents[2];    // indices of the parents in the members array
  
  // ----------------------------------------------
  // choose the parents and make the kids
  for (int i = 0; i < config->numKids; i++)
  {
    // ----------------------------------------------
    // get the indices of the parents via fitness proportional selection
    for (int k = 0; k < 2; k++)
    {
      if (random() % 100 < 20)   // take from the lower 80% (chances1)
      {
        uint64_t index = random() % chances1max;
        
        for (int j = 0; j < chances1size; j++)
        {
          if (chances1[j] >= index)
          {
            parents[k] = j;
            break;
          }
        }
      }
      else                       // take from the upper 20% (chances2)
      {
        uint64_t index = random() % chances2max;
        
        for (int j = 0; j < chances2size; j++)
        {
          if (chances2[j] >= index)
          {
            parents[k] = j + chances1size;
            break;
          }
        }
      }
    }
    
    // ----------------------------------------------
    // make the kid
    members[size + i] = kidFromParents(members[parents[0]], members[parents[1]], config);
    
    // if we failed to make a new kid, stop now because out of evals
    if (members[size + i] == NULL)
    {
      return size + i;
    }
  }
  
  free(chances1);
  free(chances2);
  return size + config->numKids;
}

// =============================================================================
// Calling this function is an error. Attempts to exit the program
// @param members The population (unused)
// @param size The size of population (unused)
// @param config The population config for members (unused)
// @return -10000000
// =============================================================================

static int parentSelectionInvalid(Member** members, int size, PopulationConfig* config)
{
  printf("ERROR: Invalid parent selection option\n");
  exit(1);
  
  // here to prevent unused args warnings
  if (false)
  {
    members = &members[0];
    size = size + 2;
    int x = config->populationSize;
    x = x + 2;
  }
  
  return -10000000;
}

// =============================================================================
//                              SURVIVOR SELECTION
// =============================================================================
// Selects the mu best members to survive
// @param members The population
// @param size The size of population
// @param config The population config for members
// @return The new size of the population
// =============================================================================

static int survivorSelectionTruncate(Member** members, int size, PopulationConfig* config)
{
  qsort(members, size, sizeof(Member*), memberCompareFitness);
  
  for (int i = config->populationSize; i < size; i++)
  {
    members[i] = NULL;
    deleteMember(members[i]);
  }
  
  return (size < config->populationSize ? size : config->populationSize);
}

// =============================================================================
// Selects survivors based on a tournament
// @param members The population
// @param size The size of population
// @param config The population config for members
// @return The new size of the population
// =============================================================================

static int survivorSelectionTournament(Member** members, int size, PopulationConfig* config)
{
  // check if we have enough people to run all the tournaments
  if (size < config->populationSize + config->survivork)
  {
    printf("ERROR: only %d members; need at least %d + %d to run all tournaments... Quitting\n",
           size, config->populationSize, config->survivork);
    exit(1);
  }
  
  // select survivors
  // invariant: after each iteration, the first i+1 survivors are at indices [0, i]
  for (int i = 0; i < config->populationSize; i++)
  {
    int winner = tournament(members, i, size, config->survivork);
    
    // move the winner to the i'th position to let him survive
    Member* temp = members[winner];
    members[winner] = members[i];
    members[i] = temp;
  }
  
  // truncate
  for (int i = config->populationSize; i < size; i++)
  {
    members[i] = NULL;
    deleteMember(members[i]);
  }
  
  return config->populationSize;
}

// =============================================================================
// Calling this function is an error. Attempts to exit the program
// @param members The population (unused)
// @param size The size of population (unused)
// @param config The population config for members (unused)
// @return -10000000;
// =============================================================================

static int survivorSelectionInvalid(Member** members, int size, PopulationConfig* config)
{
  printf("ERROR: Invalid survivor selection option\n");
  exit(1);
  
  // here to prevent unused arg warnings
  if (false)
  {
    members = &members[0];
    size = size - 2;
    int x = config->populationSize;
    x = x + 2;
  }
  
  return -10000000;
}

// =============================================================================
// =============================================================================
// =============================================================================
//                                  TESTING
// =============================================================================
// =============================================================================
// =============================================================================

void printPopulation(Member** members, int size);

// =============================================================================
// Runs function unit tests
// @param inconfig The config to tset with
// =============================================================================

void EATest(Config* inconfig)
{
  mainConfig = inconfig;
  
  // --------------------------------------
  // create members; CANNOT STATICALLY MAKE DUDES
  populationSize = mainConfig->mainPopulation->populationSize;
  population = new Member*[populationSize]();
  
  for (int i = 0; i < populationSize; i++)
  {
    population[i] = new Member(5, NODE_TYPE_ALL_NODES);
  }
  
  // --------------------------------------
  // Do some tests
  
  
  
  // --------------------------------------
  // clean up
  for (int i = 0; i < populationSize; i++)
  {
    delete population[i];
  }
  
  cout << "done running" << endl;
}

// =============================================================================
// Prints the population members and their fitnesses
// @param members The population
// @param size The size of population
// =============================================================================

void printPopulation(Member** members, int size)
{
  printf("======================================\n");
  printf("population size=%d\n", size);
  for (int i = 0; i < size; i++)
  {
    if (members[i] == NULL)
    {
      printf("population[%d]=NULL\n", i);
    }
    else
    {
      printf("population[%d]=%p %f\n", i, (void*)members[i], members[i]->fitness);
    }
  }
}

// =============================================================================

