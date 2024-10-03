#include <GlobalVariables.C>
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

void Fun4All_MvtxProduction(const int nEvents = 10000,    //
                            const int run_number = 53705, //
                            const bool withGL1 = false    //
)
{
    bool verbose = false;
    std::string dstoutdir = "./dst/";

    vector<std::string> mvtxlists = {
        "./filelists/mvtx_0.list", //
        "./filelists/mvtx_1.list", //
        "./filelists/mvtx_2.list", //
        "./filelists/mvtx_3.list", //
        "./filelists/mvtx_4.list", //
        "./filelists/mvtx_5.list"  //
    };
    vector<std::string> gl1lists = {"./filelists/gl1daq.list"};

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

    std::string outfilename = "dst_Run" + to_string(run_number) + "_";
    outfilename += (withGL1) ? "wGL1" : "woGL1";
    outfilename += ".root";

    string mkdir = "mkdir -p " + dstoutdir;
    system(mkdir.c_str());

    Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out", dstoutdir + outfilename);
    se->registerOutputManager(out);

    if (nEvents < 0)
    {
        return;
    }
    se->run(nEvents);

    se->End();
    se->PrintTimer();
    delete se;
    cout << "all done" << endl;
    gSystem->Exit(0);
}
