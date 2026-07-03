#!/usr/bin/env bash

source $ROOTSYS/../root-v5-34-9-gcc64-slc5/amsvar.sh
export PGTRACK=1
unset G4AMS
export AMSWD=..
gmake lib
