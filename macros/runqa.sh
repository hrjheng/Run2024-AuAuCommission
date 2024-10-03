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
# $5: from rawdata to dst (Fun4All_MvtxProduction.C)
# $6: from dst to hit/cluster/qa/ntuple (Fun4All_MvtxQA.C)
# $7 skip events

# check arguments
if [ $# -lt 7 ]; then
    echo "Usage: runqa.sh <type> <run number> <GL1> <nevents(strobes)> <run Fun4All_MvtxProduction.C> <run Fun4All_MvtxQA.C> <skip events(strobes)>"
    exit 1
fi

if [ $5 -eq 1 ]; then
    # make list
    mvtx_makelist.sh $1 $2

    echo "Run Fun4All_MvtxProduction.C"

    root -l -q -b Fun4All_MvtxProduction.C\($4,$2,$3\)
fi

if [ $6 -eq 1 ]; then
    echo "Run Fun4All_MvtxQA.C"

    if [ $3 -eq 1 ]; then
        sed -i 's/woGL1/wGL1/g' Fun4All_MvtxQA.C
    else
        sed -i 's/wGL1/woGL1/g' Fun4All_MvtxQA.C
    fi

    root -l -q -b Fun4All_MvtxQA.C\($4,$2,$7\)
fi
