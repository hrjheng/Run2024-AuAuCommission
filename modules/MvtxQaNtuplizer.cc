//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in MvtxQaNtuplizer.h.
//
// MvtxQaNtuplizer(const std::string &name = "MvtxQaNtuplizer")
// everything is keyed to MvtxQaNtuplizer, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// MvtxQaNtuplizer::~MvtxQaNtuplizer()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int MvtxQaNtuplizer::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int MvtxQaNtuplizer::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int MvtxQaNtuplizer::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int MvtxQaNtuplizer::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int MvtxQaNtuplizer::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int MvtxQaNtuplizer::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int MvtxQaNtuplizer::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void MvtxQaNtuplizer::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "MvtxQaNtuplizer.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <ffaobjects/EventHeader.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

#include <boost/format.hpp>
#include <boost/math/special_functions/sign.hpp>

namespace
{
template <class T> void CleanVec(std::vector<T> &v)
{
    std::vector<T>().swap(v);
    v.shrink_to_fit();
}
} // namespace

//____________________________________________________________________________..
MvtxQaNtuplizer::MvtxQaNtuplizer(const std::string &name)
    : SubsysReco(name)
{
    std::cout << "MvtxQaNtuplizer::MvtxQaNtuplizer(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
MvtxQaNtuplizer::~MvtxQaNtuplizer() { std::cout << "MvtxQaNtuplizer::~MvtxQaNtuplizer() Calling dtor" << std::endl; }

//____________________________________________________________________________..
int MvtxQaNtuplizer::Init(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::Init(PHCompositeNode *topNode) Initializing" << std::endl;

    PHTFileServer::get().open(outFileName, "RECREATE");
    outTree = new TTree("ntuple", "ntuple");
    outTree->OptimizeBaskets();
    outTree->SetAutoSave(-5e6);

    outTree->Branch("event", &event, "event/I");
    outTree->Branch("strobe_BCOs", &strobe_BCOs);
    outTree->Branch("L1_BCOs", &L1_BCOs);
    outTree->Branch("numberL1s", &numberL1s, "numberL1s/I");
    if (getMvtxRawEvtHeader)
    {
        outTree->Branch("nFeeIDs", &nFeeIDs, "nFeeIDs/I");
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_feeid", &MvtxRawEvtHeader_feeidinfo_feeid);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_layer", &MvtxRawEvtHeader_feeidinfo_layer);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_stave", &MvtxRawEvtHeader_feeidinfo_stave);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_gbtid", &MvtxRawEvtHeader_feeidinfo_gbtid);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_endpoint1", &MvtxRawEvtHeader_feeidinfo_endpoint1);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_endpoint2", &MvtxRawEvtHeader_feeidinfo_endpoint2);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_detField", &MvtxRawEvtHeader_feeidinfo_detField);
        outTree->Branch("MvtxRawEvtHeader_feeidinfo_bco", &MvtxRawEvtHeader_feeidinfo_bco);
        outTree->Branch("MvtxRawEvtHeader_MvtxLvL1BCO", &MvtxRawEvtHeader_MvtxLvL1BCO);
    }
    if (getTrkrHit)
    {
        outTree->Branch("TrkrHit_layer", &TrkrHit_layer);
        outTree->Branch("TrkrHit_stave", &TrkrHit_stave);
        outTree->Branch("TrkrHit_chip", &TrkrHit_chip);
        outTree->Branch("TrkrHit_globalX", &TrkrHit_globalX);
        outTree->Branch("TrkrHit_globalY", &TrkrHit_globalY);
        outTree->Branch("TrkrHit_globalZ", &TrkrHit_globalZ);
    }
    if (getTrkrCluster)
    {
        outTree->Branch("clusZSize", &clusZSize);
        outTree->Branch("clusPhiSize", &clusPhiSize);
        outTree->Branch("clusSize", &clusSize);
    }
    outTree->Branch("chip_occupancy", &chip_occupancy, "chip_occupancy/F");
    outTree->Branch("chip_hits", &chip_hits, "chip_hits/I");

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::InitRun(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::process_event(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;

    PHNodeIterator dstiter(topNode);

    PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(dstiter.findFirst("PHCompositeNode", "DST"));
    if (!dstNode)
    {
        std::cout << __FILE__ << "::" << __func__ << " - DST Node missing, doing nothing." << std::endl;
        exit(1);
    }

    mvtxRawEvtHeader = findNode::getClass<MvtxRawEvtHeaderv2>(dstNode, "MVTXRAWEVTHEADER");
    if (!mvtxRawEvtHeader)
    {
        std::cout << __FILE__ << "::" << __func__ << " - MvtxRawEvtHeaderv2 missing, doing nothing." << std::endl;
        exit(1);
    }

    trkrHitSetContainer = findNode::getClass<TrkrHitSetContainerv1>(dstNode, "TRKR_HITSET");
    if (!trkrHitSetContainer)
    {
        std::cout << __FILE__ << "::" << __func__ << " - TRKR_HITSET  missing, doing nothing." << std::endl;
        exit(1);
    }

    actsGeom = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
    if (!actsGeom)
    {
        std::cout << __FILE__ << "::" << __func__ << " - ActsGeometry missing, doing nothing." << std::endl;
        exit(1);
    }

    geantGeom = findNode::getClass<PHG4CylinderGeomContainer>(topNode, "CYLINDERGEOM_MVTX");
    if (!geantGeom)
    {
        std::cout << __FILE__ << "::" << __func__ << " - CYLINDERGEOM_MVTX missing, doing nothing." << std::endl;
        exit(1);
    }

    trktClusterContainer = findNode::getClass<TrkrClusterContainer>(dstNode, "TRKR_CLUSTER");
    if (!trktClusterContainer)
    {
        std::cout << __FILE__ << "::" << __func__ << " - TRKR_CLUSTER missing, doing nothing." << std::endl;
        exit(1);
    }

    actsGeom = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
    if (!actsGeom)
    {
        std::cout << __FILE__ << "::" << __func__ << " - ActsGeometry missing, doing nothing." << std::endl;
        exit(1);
    }

    mvtx_event_header = findNode::getClass<MvtxEventInfov2>(topNode, "MVTXEVENTHEADER");
    if (!mvtx_event_header)
    {
        std::cout << __FILE__ << "::" << __func__ << " - MVTXEVENTHEADER missing, doing nothing." << std::endl;
        exit(1);
    }

    event = f4aCounter;
    
    nFeeIDs = 0;
    chip_occupancy = 0.;
    chip_hits = 0.;

    if (getMvtxRawEvtHeader)
        MvtxRawEvtHeaderInfo(topNode);

    if (getMvtxEvtHeader)
        MvtxEvtHeaderInfo(topNode);

    if (getTrkrHit)
        TrkrHitInfo(topNode);

    if (getTrkrCluster)
        TrkrClusterInfo(topNode);

    outTree->Fill();

    ++f4aCounter;

    Cleanup();

    return Fun4AllReturnCodes::EVENT_OK;
}
//____________________________________________________________________________..
void MvtxQaNtuplizer::MvtxRawEvtHeaderInfo(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::MvtxRawEvtHeaderInfo(PHCompositeNode *topNode) Getting MvtxRawEvtHeader Info" << std::endl;

    uint64_t nFeeIDInfo = mvtxRawEvtHeader->get_nFeeIdInfo();
    nFeeIDs = nFeeIDInfo;

    for (uint64_t i = 0; i < nFeeIDInfo; ++i)
    {
        auto FeeIdInfo = mvtxRawEvtHeader->get_feeIdInfo(i);
        auto feeid = FeeIdInfo->get_feeId();
        auto _linkId = MvtxRawDefs::decode_feeid(feeid);
        std::pair<uint8_t, uint8_t> flxendpoint = MvtxRawDefs::get_flx_endpoint(_linkId.layer, _linkId.stave);

        MvtxRawEvtHeader_feeidinfo_feeid.push_back(FeeIdInfo->get_feeId());
        MvtxRawEvtHeader_feeidinfo_layer.push_back(_linkId.layer);
        MvtxRawEvtHeader_feeidinfo_stave.push_back(_linkId.stave);
        MvtxRawEvtHeader_feeidinfo_gbtid.push_back(_linkId.gbtid);
        MvtxRawEvtHeader_feeidinfo_endpoint1.push_back(flxendpoint.first);
        MvtxRawEvtHeader_feeidinfo_endpoint2.push_back(flxendpoint.second);
        MvtxRawEvtHeader_feeidinfo_detField.push_back(FeeIdInfo->get_detField());
        MvtxRawEvtHeader_feeidinfo_bco.push_back(FeeIdInfo->get_bco());
    }

    auto mvtxl1trgset = mvtxRawEvtHeader->getMvtxLvL1BCO();
    for (auto iter = mvtxl1trgset.begin(); iter != mvtxl1trgset.end(); ++iter)
    {
        MvtxRawEvtHeader_MvtxLvL1BCO.push_back(*iter);
    }

    std::cout << "nFeeIDInfo: " << nFeeIDInfo << " MvtxRawEvtHeader_feeidinfo_feeid.size(): " << MvtxRawEvtHeader_feeidinfo_feeid.size() << " MvtxRawEvtHeader_feeidinfo_detField.size(): " << MvtxRawEvtHeader_feeidinfo_detField.size() << " MvtxRawEvtHeader_feeidinfo_bco.size(): " << MvtxRawEvtHeader_feeidinfo_bco.size()
              << " MvtxRawEvtHeader_MvtxLvL1BCO.size(): " << MvtxRawEvtHeader_MvtxLvL1BCO.size() << std::endl;
}
//____________________________________________________________________________..
void MvtxQaNtuplizer::MvtxEvtHeaderInfo(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::MvtxEvtHeaderInfo(PHCompositeNode *topNode) Getting MvtxEvtHeader Info" << std::endl;

    numberL1s = mvtx_event_header->get_number_L1s();

    std::set<uint64_t> strobeList = mvtx_event_header->get_strobe_BCOs();
    for (auto iterStrobe = strobeList.begin(); iterStrobe != strobeList.end(); ++iterStrobe)
    {
        strobe_BCOs.push_back(*iterStrobe);
        std::set<uint64_t> l1List = mvtx_event_header->get_L1_BCO_from_strobe_BCO(*iterStrobe);
        for (auto iterL1 = l1List.begin(); iterL1 != l1List.end(); ++iterL1)
        {
            L1_BCOs.push_back(*iterL1);
        }
    }
}
//____________________________________________________________________________..
void MvtxQaNtuplizer::TrkrHitInfo(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::TrkrHitInfo(PHCompositeNode *topNode) Getting TrkrHit Info" << std::endl;

    TrkrHitSetContainer::ConstRange hitsetrange = trkrHitSetContainer->getHitSets(TrkrDefs::TrkrId::mvtxId);

    for (TrkrHitSetContainer::ConstIterator hitsetitr = hitsetrange.first; hitsetitr != hitsetrange.second; ++hitsetitr)
    {
        TrkrHitSet *hitset = hitsetitr->second;
        auto hitsetkey = hitset->getHitSetKey();

        auto surface = actsGeom->maps().getSiliconSurface(hitsetkey);
        auto layergeom = dynamic_cast<CylinderGeom_Mvtx *>(geantGeom->GetLayerGeom(TrkrDefs::getLayer(hitsetkey)));
        TVector2 LocalUse;

        chip_hits = hitsetitr->second->size();
        chip_occupancy = (float)chip_hits / (512 * 1024);

        TrkrHitSet::ConstRange hit_range = hitsetitr->second->getHits();
        for (TrkrHitSet::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; ++hit_iter)
        {
            TrkrDefs::hitkey hitKey = hit_iter->first;

            TVector3 local_coords = layergeom->get_local_coords_from_pixel(MvtxDefs::getRow(hitKey), MvtxDefs::getCol(hitKey));
            localX.push_back(local_coords.x());
            localY.push_back(local_coords.z());

            LocalUse.SetX(local_coords.x());
            LocalUse.SetY(local_coords.z());
            TVector3 ClusterWorld = layergeom->get_world_from_local_coords(surface, actsGeom, LocalUse);
            TrkrHit_layer.push_back(TrkrDefs::getLayer(hitsetkey));
            TrkrHit_stave.push_back(MvtxDefs::getStaveId(hitsetkey));
            TrkrHit_chip.push_back(MvtxDefs::getChipId(hitsetkey));
            TrkrHit_globalX.push_back(ClusterWorld.X());
            TrkrHit_globalY.push_back(ClusterWorld.Y());
            TrkrHit_globalZ.push_back(ClusterWorld.Z());

            std::pair layer_stave = std::make_pair(TrkrDefs::getLayer(hitsetkey), MvtxDefs::getStaveId(hitsetkey));
            if (map_stave_th1fchipoccup.find(layer_stave) == map_stave_th1fchipoccup.end())
            {
                map_stave_th1fchipoccup[layer_stave] = new TH1F(Form("hM_chipoccup_L%d_%d", TrkrDefs::getLayer(hitsetkey), MvtxDefs::getStaveId(hitsetkey)), Form("hM_chipoccup_L%d_%d", TrkrDefs::getLayer(hitsetkey), MvtxDefs::getStaveId(hitsetkey)), 60, 0, 0.6);
                map_stave_th1fchipoccup[layer_stave]->Fill(chip_occupancy * 100);
            }
            else
            {
                map_stave_th1fchipoccup[layer_stave]->Fill(chip_occupancy * 100);
            }
        }
    }
}
//____________________________________________________________________________..
void MvtxQaNtuplizer::TrkrClusterInfo(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::TrkrClusterInfo(PHCompositeNode *topNode) Getting TrkrCluster Info" << std::endl;

    for (const auto &hitsetkey : trktClusterContainer->getHitSetKeys(TrkrDefs::TrkrId::mvtxId))
    {
        auto range = trktClusterContainer->getClusters(hitsetkey);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            auto ckey = iter->first;
            auto cluster = trktClusterContainer->findCluster(ckey);
            clusZSize.push_back(cluster->getZSize());
            clusPhiSize.push_back(cluster->getPhiSize());
            clusSize.push_back(cluster->getAdc());
        }
    }
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::EndRun(const int runnumber)
{
    std::cout << "MvtxQaNtuplizer::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::End(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::End(PHCompositeNode *topNode) This is the End..." << std::endl;

    PHTFileServer::get().cd(outFileName);
    for (auto iter = map_stave_th1fchipoccup.begin(); iter != map_stave_th1fchipoccup.end(); ++iter)
    {
        iter->second->Write();
    }

    outTree->Write("", TObject::kOverwrite);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int MvtxQaNtuplizer::Reset(PHCompositeNode *topNode)
{
    std::cout << "MvtxQaNtuplizer::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void MvtxQaNtuplizer::Print(const std::string &what) const { std::cout << "MvtxQaNtuplizer::Print(const std::string &what) const Printing info for " << what << std::endl; }
//____________________________________________________________________________..
void MvtxQaNtuplizer::Cleanup()
{
    CleanVec(MvtxRawEvtHeader_feeidinfo_feeid);
    CleanVec(MvtxRawEvtHeader_feeidinfo_layer);
    CleanVec(MvtxRawEvtHeader_feeidinfo_stave);
    CleanVec(MvtxRawEvtHeader_feeidinfo_gbtid);
    CleanVec(MvtxRawEvtHeader_feeidinfo_endpoint1);
    CleanVec(MvtxRawEvtHeader_feeidinfo_endpoint2);
    CleanVec(MvtxRawEvtHeader_feeidinfo_detField);
    CleanVec(MvtxRawEvtHeader_feeidinfo_bco);
    CleanVec(MvtxRawEvtHeader_MvtxLvL1BCO);
    CleanVec(TrkrHit_layer);
    CleanVec(TrkrHit_stave);
    CleanVec(TrkrHit_chip);
    CleanVec(localX);
    CleanVec(localY);
    CleanVec(TrkrHit_globalX);
    CleanVec(TrkrHit_globalY);
    CleanVec(TrkrHit_globalZ);
    CleanVec(clusZSize);
    CleanVec(clusPhiSize);
    CleanVec(clusSize);
    CleanVec(strobe_BCOs);
    CleanVec(L1_BCOs);
}
