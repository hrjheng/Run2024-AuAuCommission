#!/usr/bin/bash

# creates file lists for the MVTX from known locations in lustre
# run number is the input argument

mkdir -p ./filelists

if [ $# -eq 0 ]; then
  echo "Creates needed lists of input files for the MVTX for a given run"
  echo "Usage: mvtx_makelist.sh <type> <run number>"
  exit 1
fi

if [ $# -eq 1 ]; then
  echo "No type or runnumber supplied"
  exit 0
fi

type=$1
runnumber=$(printf "%08d" $2)

for i in {0..5}; do
  ls -1 /sphenix/lustre01/sphnxpro/physics/MVTX/${type}/${type}_mvtx${i}-${runnumber}-* >./filelists/mvtx_${i}_Run${runnumber}.list
  if [ ! -s filelists/mvtx_${i}_Run${runnumber}.list ]; then
    echo mvtx_${i}_Run${runnumber}.list empty, removing it
    rm filelists/mvtx_${i}_Run${runnumber}.list
  fi

done

/bin/ls -1 /sphenix/lustre01/sphnxpro/physics/GL1/${type}/GL1_*-${runnumber}-* >./filelists/gl1daq_Run${runnumber}.list
if [ ! -s filelists/gl1daq_Run${runnumber}.list ]; then
  echo gl1daq_Run${runnumber}.list empty, removing it
  rm filelists/gl1daq_Run${runnumber}.list
fi
