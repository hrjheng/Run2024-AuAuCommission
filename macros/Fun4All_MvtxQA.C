/*
 * This macro shows a minimum working example of running the tracking
 * hit unpackers with some basic seeding algorithms to try to put together
 * tracks. There are some analysis modules run at the end which package
 * hits, clusters, and clusters on tracks into trees for analysis.
 */

#include <G4_ActsGeom.C>
#include <G4_Global.C>
#include <G4_Magnet.C>
#include <G4_Mbd.C>
#include <GlobalVariables.C>
#include <QA.C>
#include <Trkr_Clustering.C>
#include <Trkr_Reco.C>
#include <Trkr_RecoInit.C>
#include <fun4all/Fun4AllUtils.h>

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

#include <mvtxqantuplizer/MvtxQaNtuplizer.h>
#include <mvtxrawhitqa/MvtxRawHitQA.h>
#include <trackingqa/MvtxClusterQA.h>

#include <cstdlib>
#include <stdio.h>

// #include "localQAdump.C"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libtrackingqa.so)
R__LOAD_LIBRARY(libmvtxrawhitqa.so)

R__LOAD_LIBRARY(libMvtxQaNtuplizer.so)

void Fun4All_MvtxQA(const int nEvents = 100,     //
                    const int runnumber = 53705, //
                    const int skip = 0           //
)
{
    bool hitntp = true;
    Enable::QA = true;

    // std::string QAhistdir = "./qahists/";
    std::string QAhistdir = "/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/qahists/";
    std::string mkdir = "mkdir -p " + QAhistdir + "/" + std::to_string(runnumber) + "/";
    system(mkdir.c_str());

    TString qaoutfile = QAhistdir + "/" + std::to_string(runnumber) + "/qahist-nEvent" + std::to_string(nEvents) + "-skip" + std::to_string(skip);
    TString ntpoutfile = QAhistdir + "/" + std::to_string(runnumber) + "/ntp-nEvent" + std::to_string(nEvents) + "-skip" + std::to_string(skip);
    std::string qaOutfile = qaoutfile.Data();
    std::string ntpOutfile = ntpoutfile.Data();

    auto se = Fun4AllServer::instance();
    se->Verbosity(1);

    auto rc = recoConsts::instance();
    rc->set_IntFlag("RUNNUMBER", runnumber);

    Enable::CDB = true;
    rc->set_StringFlag("CDB_GLOBALTAG", "2024p007");
    rc->set_uint64Flag("TIMESTAMP", runnumber);
    rc->set_IntFlag("RUNNUMBER", runnumber);
    std::string geofile = CDBInterface::instance()->getUrl("Tracking_Geometry");

    ACTSGEOM::ActsGeomInit();

    Fun4AllRunNodeInputManager *ingeo = new Fun4AllRunNodeInputManager("GeoIn");
    ingeo->AddFile(geofile);
    se->registerInputManager(ingeo);

    G4MAGNET::magfield_rescale = 1;
    TrackingInit();

    auto hitsin = new Fun4AllDstInputManager("InputManager");
    // hitsin->fileopen(Form("./dst/dst_Run%d_wGL1.root", runnumber));
    hitsin->fileopen(Form("/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/dst/dst_Run%d_wGL1.root", runnumber));

    se->registerInputManager(hitsin);

    Mvtx_HitUnpacking();
    Mvtx_Clustering();

    if (Enable::QA)
    {
        se->registerSubsystem(new MvtxRawHitQA);
        se->registerSubsystem(new MvtxClusterQA);
    }

    if (hitntp)
    {
        MvtxQaNtuplizer *mvtxqantuplizer = new MvtxQaNtuplizer();
        std::string clusterFileName = ntpOutfile + ".root";
        mvtxqantuplizer->OutputFileName(clusterFileName);
        mvtxqantuplizer->GetMvtxRawEvtHeaderInfo(true);
        mvtxqantuplizer->GetTrkrHitInfo(true);
        mvtxqantuplizer->GetTrkrClusterInfo(false);
        se->registerSubsystem(mvtxqantuplizer);
    }

    se->skip(skip);
    se->run(nEvents);
    se->End();

    if (Enable::QA)
    {
        TString qaname = qaOutfile + ".root";
        std::string qaOutputFileName(qaname.Data());
        QAHistManagerDef::saveQARootFile(qaOutputFileName);
    }

    se->PrintTimer();
    delete se;

    // localQAdump(theOutfile + ".root", runnumber);

    std::cout << "Finished" << std::endl;

    gSystem->Exit(0);
}
