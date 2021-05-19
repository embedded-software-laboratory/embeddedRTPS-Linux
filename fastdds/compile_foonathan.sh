#!/bin/bash

## Find the script location
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
echo $SCRIPT_DIR

## Change to the scripts location
cd $SCRIPT_DIR/foonathan_memory_vendor

ls -alh

## Check if build folder exists, otherwise create it
if [ -d "build" ]; then
  rm -rf build
fi
mkdir build

## Initiate the CMake compile and installation of foonathan memory
cd build
echo $SCRIPT_DIR
cmake -DCMAKE_INSTALL_PREFIX=$SCRIPT_DIR/foonathan_memory_vendor/build/ ..
make install
