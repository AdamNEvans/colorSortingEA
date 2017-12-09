#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "ea.h"


// ============================================================================
// Main function. If a configuration file is not given, then
// "configurations/default.cfg" is used
// @param argc Size of argv
// @param argv argv[0] = program name
//             argv[1] = config file (optional)
// ============================================================================

int main(int argc, char* argv[])
{
  // ---------------------------------------
  // command line argument validation
  // ---------------------------------------
  char defaultConfigFile[] = "configurations/default.cfg";
  char* configFilename = NULL;
  
  switch (argc)
  {
    case 1:    configFilename = &defaultConfigFile[0];   break;
    case 2:    configFilename = argv[1];                 break;
    default:
      printf("Usage: %s [configFile]\n", argv[0]);
      return 1;
  }
  
  // ---------------------------------------
  // load the config
  // ---------------------------------------
  Config config = Config(configFilename);
  config.print(stdout);
  
  srandom(config.seed);
  
  // ---------------------------------------
  // run the EA
  // ---------------------------------------
  
  executeEA(&config);
  
  // ---------------------------------------
  
  return 0;
}

// ============================================================================



