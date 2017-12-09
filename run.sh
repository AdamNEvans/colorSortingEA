#!/bin/bash

TEMP_DIR="temp"
SOLUTION_DIR="solutions"

# ==============================================================================
# read command line args
configFile="configurations/default.cfg"

if (( $# == 1 )); then
  configFile=$1
elif (( $# > 1 )); then
  echo "Usage: ./run.sh [<configFile>]"
  exit 1
fi

# ==============================================================================
# check for the required directories existing and create them if necessary
if [ ! -d "$TEMP_DIR" ]; then
  mkdir "$TEMP_DIR"
fi

if [ ! -d "$SOLUTION_DIR" ]; then
  mkdir "$SOLUTION_DIR"
fi

# ==============================================================================
# run
./main "$configFile"
