#!/usr/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=/sphenix/u/hjheng/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# get the current directory
basedir=$(pwd)

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
    mkdir -p ./production_logs
    mkdir -p /sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/dst

    echo "Run Fun4All_MvtxProduction.C"
    # pipe printout to log file
    gl1str=""
    if [ $3 -eq 1 ]; then
        gl1str="wGL1"
    else
        gl1str="woGL1"
    fi
    # pipe printout, including error messages, to log file
    root -l -q -b Fun4All_MvtxProduction.C\($4,$2,$3\) &> ./production_logs/MvtxProduction-Run$2-$gl1str.log
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

# run various checks
root -l -q -b localQAdump.C\(\"/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/qahists/$2/qahist-nEvent$4-skip$7.root\",$2\)
root -l -q -b PlotTrkrHit.C\(\"/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/qahists/$2/ntp-nEvent$4-skip$7.root\",$2\)
root -l -q -b MvtxRawEvtHeaderCheck.C\(\"/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/qahists/$2/ntp-nEvent$4-skip$7.root\",$2\)

# publish the plots to /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning
cp -r ./plots/Run$2 /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/
cd /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/
cp -r /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/res ./Run$2
cp /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/index.php ./Run$2
for dir in $(find ./Run$2 -mindepth 1 -maxdepth 1 -type d); do
    cp -r /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/res $dir
    cp /sphenix/WWW/user/hjheng/Run2024/AuAu-Commissioning/index.php $dir
done


