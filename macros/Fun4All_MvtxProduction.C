#include <GlobalVariables.C>
#include <QA.C>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllStreamingInputManager.h>
#include <fun4allraw/InputManagerType.h>
#include <fun4allraw/SingleGl1PoolInput.h>
#include <fun4allraw/SingleMvtxPoolInput.h>

#include <phool/recoConsts.h>

#include <ffarawmodules/StreamingCheck.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawmodules.so)

bool isGood(const string &infile)
{
    ifstream intest;
    intest.open(infile);
    bool goodfile = false;
    if (intest.is_open())
    {
        if (intest.peek() != std::ifstream::traits_type::eof()) // is it non zero?
        {
            goodfile = true;
        }
        intest.close();
    }
    return goodfile;
}

void Fun4All_MvtxProduction(const int nEvents = 10000,   //
                            const int runnumber = 53705, //
                            const bool withGL1 = false   //
)
{
    bool verbose = false;
    Enable::QA = true;

    // std::string dstoutdir = "./dst/";
    std::string dstoutdir = "/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/dst/";
    // std::string QAhistdir = "./qahists/";
    std::string QAhistdir = "/sphenix/tg/tg01/hf/hjheng/MVTX/Run24-AuAu/qahists/";
    std::string outfilename = "dst_Run" + to_string(runnumber) + "_";
    outfilename += (withGL1) ? "wGL1" : "woGL1";
    outfilename += ".root";
    TString qaoutfile = QAhistdir + "/" + std::to_string(runnumber) + "/qahist-streamprod-nEvent" + std::to_string(nEvents);
    std::string qaOutfile = qaoutfile.Data();

    std::string mkdir = "mkdir -p " + dstoutdir;
    system(mkdir.c_str());
    mkdir = "mkdir -p " + QAhistdir + "/" + std::to_string(runnumber) + "/";
    system(mkdir.c_str());

    // format the string of runnumber to as %08d
    std::string runnumber_str = std::to_string(runnumber);
    runnumber_str = std::string(8 - runnumber_str.length(), '0') + runnumber_str;
    vector<std::string> mvtxlists = {
        "./filelists/mvtx_0_Run" + runnumber_str + ".list", //
        "./filelists/mvtx_1_Run" + runnumber_str + ".list", //
        "./filelists/mvtx_2_Run" + runnumber_str + ".list", //
        "./filelists/mvtx_3_Run" + runnumber_str + ".list", //
        "./filelists/mvtx_4_Run" + runnumber_str + ".list", //
        "./filelists/mvtx_5_Run" + runnumber_str + ".list", //
    };
    vector<std::string> gl1lists = {"./filelists/gl1daq_Run" + runnumber_str + ".list"};

    Fun4AllServer *se = Fun4AllServer::instance();
    se->Verbosity(1);

    recoConsts *rc = recoConsts::instance();

    Fun4AllStreamingInputManager *in = new Fun4AllStreamingInputManager("Comb");
    in->Verbosity(0);

    // mvtx
    int i = 0;
    for (auto iter : mvtxlists)
    {
        if (isGood(iter))
        {
            SingleMvtxPoolInput *mvtx_sngl = new SingleMvtxPoolInput("MVTX_" + to_string(i));
            if (verbose)
                mvtx_sngl->Verbosity(5);
            mvtx_sngl->SetBcoRange(100);
            mvtx_sngl->SetNegativeBco(500);
            mvtx_sngl->AddListFile(iter);
            in->registerStreamingInput(mvtx_sngl, InputManagerType::MVTX);
            i++;
        }
    }

    i = 0;
    if (withGL1)
    {
        for (auto iter : gl1lists)
        {
            if (isGood(iter))
            {
                SingleGl1PoolInput *gl1_sngl = new SingleGl1PoolInput("GL1_" + to_string(i));
                if (verbose)
                    gl1_sngl->Verbosity(3);
                gl1_sngl->AddListFile(iter);
                in->registerStreamingInput(gl1_sngl, InputManagerType::GL1);
                i++;
            }
        }
    }

    i = 0;

    se->registerInputManager(in);

    SyncReco *sync = new SyncReco();
    se->registerSubsystem(sync);

    HeadReco *head = new HeadReco();
    se->registerSubsystem(head);

    FlagHandler *flag = new FlagHandler();
    se->registerSubsystem(flag);

    Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", dstoutdir + outfilename);
    se->registerOutputManager(out);

    if (nEvents < 0)
    {
        return;
    }
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

    cout << "all done" << endl;
    gSystem->Exit(0);
}
