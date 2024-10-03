// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef MVTXQANTUPLIZER_H
#define MVTXQANTUPLIZER_H

#include <fun4all/SubsysReco.h>

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <set>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TTree.h>
#include <TVector2.h>
#include <TVector3.h>

#include <ffarawobjects/MvtxRawEvtHeaderv2.h>
#include <ffarawobjects/MvtxFeeIdInfov1.h>

#include <g4detectors/PHG4CylinderGeomContainer.h>
#include <mvtx/CylinderGeom_Mvtx.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/MvtxEventInfov2.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrClusterHitAssocv3.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainerv1.h>
#include <trackbase/TrkrHitv2.h>

class PHCompositeNode;
class MvtxFeeIdInfo;
class TClonesArray;

class MvtxQaNtuplizer : public SubsysReco
{
  public:
    MvtxQaNtuplizer(const std::string &name = "MvtxQaNtuplizer");

    ~MvtxQaNtuplizer() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
     */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
     */
    int InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
     */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;

    void OutputFileName(const std::string &file) { outFileName = file; }
    void GetMvtxRawEvtHeaderInfo(bool b) { getMvtxRawEvtHeader = b; }
    void GetTrkrHitInfo(bool b) { getTrkrHit = b; }
    void GetTrkrClusterInfo(bool b) { getTrkrCluster = b; }

  private:
    bool getMvtxRawEvtHeader = true;
    bool getTrkrHit = true;
    bool getTrkrCluster = false;

    void MvtxRawEvtHeaderInfo(PHCompositeNode *topNode);
    void TrkrHitInfo(PHCompositeNode *topNode);
    void TrkrClusterInfo(PHCompositeNode *topNode);
    
    int f4aCounter = 0;

    MvtxRawEvtHeaderv2 *mvtxRawEvtHeader = nullptr;
    TrkrHitSetContainerv1 *trkrHitSetContainer = nullptr;
    TrkrClusterContainer *trktClusterContainer = nullptr;
    ActsGeometry *actsGeom = nullptr;
    PHG4CylinderGeomContainer *geantGeom = nullptr;
    MvtxEventInfov2 *mvtx_event_header = nullptr;

    TTree *outTree = nullptr;
    std::string outFileName = "outputClusters.root";

    int event = 0;
    int nFeeIDs = 0;
    std::vector<uint16_t> MvtxRawEvtHeader_feeidinfo_feeid;
    std::vector<uint32_t> MvtxRawEvtHeader_feeidinfo_detField;
    std::vector<uint64_t> MvtxRawEvtHeader_feeidinfo_bco;
    std::vector<uint64_t> MvtxRawEvtHeader_MvtxLvL1BCO;
    std::vector<uint64_t> strobe_BCOs;
    std::vector<uint64_t> L1_BCOs;
    int numberL1s = 0;
    std::vector<int> TrkrHit_layer;
    std::vector<int> TrkrHit_stave;
    std::vector<int> TrkrHit_chip;
    std::vector<float> localX;
    std::vector<float> localY;
    std::vector<float> TrkrHit_globalX;
    std::vector<float> TrkrHit_globalY;
    std::vector<float> TrkrHit_globalZ;
    std::vector<float> clusZSize;
    std::vector<float> clusPhiSize;
    std::vector<unsigned int> clusSize;
    float chip_occupancy = 0.;
    int chip_hits = 0.;

    std::map<std::pair<int, int>, TH1F *> map_stave_th1fchipoccup;
    TH2F *h_stave_chipoccupancy = nullptr;
};

#endif // MVTXQANTUPLIZER_H
