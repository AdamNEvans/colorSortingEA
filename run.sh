#!/bin/bash

SOLUTION_DIR="solutions"
LOGS_DIR="logs"

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
if [ ! -d "$SOLUTION_DIR" ]; then
  mkdir "$SOLUTION_DIR"
fi

if [ ! -d "$LOGS_DIR" ]; then
  mkdir "$LOGS_DIR"
fi

# ==============================================================================
# run
./main "$configFile"
