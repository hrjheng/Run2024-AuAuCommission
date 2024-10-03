#!/usr/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=/sphenix/u/hjheng/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# arguments
# $1: type
# $2: run number
# $3: run with GL1 or not
# $4: number of events to process
# $5: run QA or not
# $6: run hit ntuplizer or not
# $7 skip events

# check arguments
if [ $# -lt 7 ]; then
    echo "Usage: runqa.sh <type> <run number> <GL1> <nevents(strobes)> <runQA> <runHitNtuplizer> <skip events(strobes)>"
    exit 1
fi

# make list 
mvtx_makelist.sh $1 $2

# run production
# root -l -q -b Fun4All_MvtxProduction.C\($4,$2,$3\)

# run hit unpacking, clustering, QA, and hit ntuplizer
# for example, if run with GL1, change the input file name from "*_woGL1.root" to "*_wGL1.root"
if [ $3 -eq 1 ]; then
    sed -i 's/woGL1/wGL1/g' Fun4All_MvtxQA.C
else
    sed -i 's/wGL1/woGL1/g' Fun4All_MvtxQA.C
fi

if [ $5 -eq 1 ]; then
    sed -i 's/Enable::QA = false;/Enable::QA = true;/g' Fun4All_MvtxQA.C
else
    sed -i 's/Enable::QA = true;/Enable::QA = false;/g' Fun4All_MvtxQA.C
fi

if [ $6 -eq 1 ]; then
    sed -i 's/bool hitntp = false;/bool hitntp = true;/g' Fun4All_MvtxQA.C
else
    sed -i 's/bool hitntp = true;/bool hitntp = false;/g' Fun4All_MvtxQA.C
fi



root -l -q -b Fun4All_MvtxQA.C\($4,$2,$7\)