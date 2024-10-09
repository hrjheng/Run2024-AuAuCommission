#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "Utils.h"

void MvtxRawEvtHeaderCheck(TString inputfile = "./qahists/53539/ntp-nEvent10000-skip0.root", int runnumber = 53539)
{
    std::string plotdir_rawevtheader = "./plots/Run" + std::to_string(runnumber) + "/RawEvtHeader";
    system(Form("mkdir -p %s", plotdir_rawevtheader.c_str()));

    TH2F *h_MvtxRawEvtHeader_nBCOconf = new TH2F("h_MvtxRawEvtHeader_nBCOconf", ";Number of L1 BCOs;Number of unique FEE BCOs;Percent [%]", 4, -0.5, 3.5, 4, -0.5, 3.5);
    vector<vector<uint64_t>> vec_uniquefeebco;
    vector<uint64_t> vec_flat_uniquefeebco;
    vector<int> vec_nFeeIDs;
    vector<uint64_t> vec_uniqueGL1bco;
    vector<uint64_t> vec_uniqueL1bco;
    map<int, uint64_t> map_eventidx_GL1bco;
    map<uint64_t, int> map_FeeBco_nFeeIDs;
    map<uint64_t, bool> map_FeeBco_matched2GL1Bco;
    map<uint64_t, bool> map_GL1Bco_matched2FeeBco;
    map<uint64_t, std::pair<uint64_t, int>> map_GL1BCO_pairMatchedFeeBcoNFees;

    TFile *f = new TFile(inputfile, "READ");
    TTree *tree = (TTree *)f->Get("ntuple");
    Int_t event;
    uint64_t GL1Packet_BCO = 0;
    int nFeeIDs = 0;
    int nUniqueFeeBcos = 0;
    int nUniqueL1Bcos = 0;
    vector<uint64_t> *L1_BCOs = 0;
    vector<uint16_t> *MvtxRawEvtHeader_feeidinfo_feeid = 0;
    vector<uint32_t> *MvtxRawEvtHeader_feeidinfo_layer = 0;
    vector<uint32_t> *MvtxRawEvtHeader_feeidinfo_stave = 0;
    vector<uint32_t> *MvtxRawEvtHeader_feeidinfo_gbtid = 0;
    vector<uint8_t> *MvtxRawEvtHeader_feeidinfo_endpoint1 = 0;
    vector<uint8_t> *MvtxRawEvtHeader_feeidinfo_endpoint2 = 0;
    vector<uint64_t> *MvtxRawEvtHeader_feeidinfo_bco = 0;
    vector<uint64_t> *MvtxRawEvtHeader_MvtxLvL1BCO = 0;
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("L1_BCOs", &L1_BCOs);
    tree->SetBranchAddress("GL1Packet_BCO", &GL1Packet_BCO);
    tree->SetBranchAddress("nFeeIDs", &nFeeIDs);
    tree->SetBranchAddress("nUniqueFeeBcos", &nUniqueFeeBcos);
    tree->SetBranchAddress("nUniqueL1Bcos", &nUniqueL1Bcos);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_feeid", &MvtxRawEvtHeader_feeidinfo_feeid);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_layer", &MvtxRawEvtHeader_feeidinfo_layer);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_stave", &MvtxRawEvtHeader_feeidinfo_stave);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_gbtid", &MvtxRawEvtHeader_feeidinfo_gbtid);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_endpoint1", &MvtxRawEvtHeader_feeidinfo_endpoint1);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_endpoint2", &MvtxRawEvtHeader_feeidinfo_endpoint2);
    tree->SetBranchAddress("MvtxRawEvtHeader_feeidinfo_bco", &MvtxRawEvtHeader_feeidinfo_bco);
    tree->SetBranchAddress("MvtxRawEvtHeader_MvtxLvL1BCO", &MvtxRawEvtHeader_MvtxLvL1BCO);

    std::set<uint64_t> set_bcodiff_2bcoPerFee;
    std::set<uint64_t> set_bcodiff_1L12Fee;
    std::set<uint64_t> set_bcodiff_2L1bcoPerFee;
    std::set<uint64_t> set_bcodiff_2L11Fee;
    std::vector<int> vec_eventidx_0L10Fee;
    std::vector<int> vec_eventidx_2L1;

    for (int i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);

        vec_uniqueGL1bco.push_back(GL1Packet_BCO);
        map_eventidx_GL1bco[event] = GL1Packet_BCO;
        for (auto iter = L1_BCOs->begin(); iter != L1_BCOs->end(); ++iter)
        {
            vec_uniqueL1bco.push_back(*iter);
        }
        map_GL1Bco_matched2FeeBco[GL1Packet_BCO] = false;

        std::set<uint64_t> bcoset;
        bcoset.insert(MvtxRawEvtHeader_feeidinfo_bco->begin(), MvtxRawEvtHeader_feeidinfo_bco->end());
        std::vector<uint64_t> vfeebco;
        for (auto iter = bcoset.begin(); iter != bcoset.end(); ++iter)
        {
            vec_flat_uniquefeebco.push_back(*iter);
            vfeebco.push_back(*iter);

            int nFeeIDInfo = 0;
            for (unsigned int j = 0; j < MvtxRawEvtHeader_feeidinfo_bco->size(); j++)
            {
                if (MvtxRawEvtHeader_feeidinfo_bco->at(j) == *iter)
                {
                    nFeeIDInfo++;
                }
            }
            vec_nFeeIDs.push_back(nFeeIDInfo);
            map_FeeBco_nFeeIDs[*iter] = nFeeIDInfo;
            map_FeeBco_matched2GL1Bco[*iter] = false;
        }
        vec_uniquefeebco.push_back(vfeebco);

        if (MvtxRawEvtHeader_MvtxLvL1BCO->size() <= 2 && bcoset.size() <= 2)
        {
            h_MvtxRawEvtHeader_nBCOconf->Fill(MvtxRawEvtHeader_MvtxLvL1BCO->size(), bcoset.size());

            // 0 L1 BCO, 0 FEE BCO
            if (MvtxRawEvtHeader_MvtxLvL1BCO->size() == 0 && bcoset.size() == 0)
            {
                vec_eventidx_0L10Fee.push_back(event);
            }

            // 2 L1 BCO
            if (MvtxRawEvtHeader_MvtxLvL1BCO->size() == 2)
            {
                vec_eventidx_2L1.push_back(event);
                // 2 L1 BCO, 1 FEE BCO
                if (bcoset.size() == 1)
                {
                    set_bcodiff_2L11Fee.insert(MvtxRawEvtHeader_MvtxLvL1BCO->at(1) - MvtxRawEvtHeader_MvtxLvL1BCO->at(0));
                }

                // 2 L1 BCO, 2 FEE BCO
                // if (bcoset.size() == 2)
                // {
                //     // print out the 4 differences
                //     for (auto iter = bcoset.begin(); iter != bcoset.end(); ++iter)
                //     {
                //         for (auto iter2 = MvtxRawEvtHeader_MvtxLvL1BCO->begin(); iter2 != MvtxRawEvtHeader_MvtxLvL1BCO->end(); ++iter2)
                //         {
                //             std::cout << "Event (strobe) index:" << event << "  L1 BCO: " << *iter2 << ", FEE BCO: " << *iter << ", diff=" << diff(*iter, *iter2) << std::endl;
                //         }
                //     }
                // }
            }

            // 1 L1 BCO, 2 FEE BCO
            if (MvtxRawEvtHeader_MvtxLvL1BCO->size() == 1 && bcoset.size() == 2)
            {
                for (auto iter = bcoset.begin(); iter != bcoset.end(); ++iter)
                {
                    // std::cout << "Event (strobe) index:" << event << "  L1 BCO: " << MvtxRawEvtHeader_MvtxLvL1BCO->at(0) << ", FEE BCO: " << *iter << ", diff=" << diff(*iter, MvtxRawEvtHeader_MvtxLvL1BCO->at(0)) << std::endl;
                    set_bcodiff_1L12Fee.insert(diff(*iter, MvtxRawEvtHeader_MvtxLvL1BCO->at(0)));
                }
            }
        }
        else
        {
            if (MvtxRawEvtHeader_MvtxLvL1BCO->size() > 2 && bcoset.size() <= 2)
            {
                h_MvtxRawEvtHeader_nBCOconf->Fill(3, bcoset.size());
            }
            else if (bcoset.size() > 2 && MvtxRawEvtHeader_MvtxLvL1BCO->size() <= 2)
            {
                h_MvtxRawEvtHeader_nBCOconf->Fill(MvtxRawEvtHeader_MvtxLvL1BCO->size(), 3);
            }
            else if (bcoset.size() > 2 && MvtxRawEvtHeader_MvtxLvL1BCO->size() > 2)
            {
                h_MvtxRawEvtHeader_nBCOconf->Fill(3, 3);
            }
        }

        if (bcoset.size() >= 2)
        {
            if (bcoset.size() == 2)
            {
                set_bcodiff_2bcoPerFee.insert(*bcoset.rbegin() - *bcoset.begin());
            }

            std::cout << "Event (strobe) index: " << i << " GL1 BCO: " << GL1Packet_BCO << " (>= 2 unique FEE BCOs) FEE BCO set: ";
            for (auto iter = bcoset.begin(); iter != bcoset.end(); ++iter)
            {
                std::cout << *iter << " (diff to GL1 BCO = " << diff(*iter, GL1Packet_BCO) << "), ";
            }
            std::cout << std::endl;
        }

        if (MvtxRawEvtHeader_MvtxLvL1BCO->size() == 2)
        {
            set_bcodiff_2L1bcoPerFee.insert(MvtxRawEvtHeader_MvtxLvL1BCO->at(1) - MvtxRawEvtHeader_MvtxLvL1BCO->at(0));
        }
    }

    std::cout << "Unique BCO differences for strobes with 2 BCOs per FEE: ";
    for (auto iter = set_bcodiff_2bcoPerFee.begin(); iter != set_bcodiff_2bcoPerFee.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    std::cout << "Unique BCO differences from 1 L1 BCO and 2 FEE BCOs: ";
    for (auto iter = set_bcodiff_1L12Fee.begin(); iter != set_bcodiff_1L12Fee.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    std::cout << "Unique BCO differences from 2 L1 BCOs: ";
    for (auto iter = set_bcodiff_2L1bcoPerFee.begin(); iter != set_bcodiff_2L1bcoPerFee.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    std::cout << "Unique BCO differences from 2 L1 BCOs and 1 FEE: ";
    for (auto iter = set_bcodiff_2L11Fee.begin(); iter != set_bcodiff_2L11Fee.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;

    /*
    std::cout << "List of events with 0 L1 BCO and 0 FEE BCO: ";
    for (auto iter = vec_eventidx_0L10Fee.begin(); iter != vec_eventidx_0L10Fee.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    std::cout << "List of events with 2 L1 BCOs: ";
    for (auto iter = vec_eventidx_2L1.begin(); iter != vec_eventidx_2L1.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    */

    // Compare the vec_eventidx_0L10Fee and vec_eventidx_2L1. If the difference between the elements from the two vectors is 1, then the event is matched.
    // First, add 1 to each element in vec_eventidx_2L1, use transform() function
    std::transform(vec_eventidx_2L1.begin(), vec_eventidx_2L1.end(), vec_eventidx_2L1.begin(), [](int i) { return i + 1; });
    // Then, make both vectors to sets
    std::set<int> set_eventidx_0L10Fee(vec_eventidx_0L10Fee.begin(), vec_eventidx_0L10Fee.end());
    std::set<int> set_eventidx_2L1(vec_eventidx_2L1.begin(), vec_eventidx_2L1.end());
    // Get the same elements from the two sets and get the unma
    std::vector<int> vec_eventidx_matched;
    std::set_intersection(set_eventidx_0L10Fee.begin(), set_eventidx_0L10Fee.end(), set_eventidx_2L1.begin(), set_eventidx_2L1.end(), std::back_inserter(vec_eventidx_matched));
    // Remove the matched elements from 2gl1 vector
    vec_eventidx_2L1.erase(std::remove_if(vec_eventidx_2L1.begin(), vec_eventidx_2L1.end(), [&vec_eventidx_matched](int i) { return std::find(vec_eventidx_matched.begin(), vec_eventidx_matched.end(), i) != vec_eventidx_matched.end(); }), vec_eventidx_2L1.end());
    // Subtract 1 from each element in vec_eventidx_2L1
    std::transform(vec_eventidx_2L1.begin(), vec_eventidx_2L1.end(), vec_eventidx_2L1.begin(), [](int i) { return i - 1; });
    /*
    std::cout << " List of matched events: ";
    for (auto iter = vec_eventidx_matched.begin(); iter != vec_eventidx_matched.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    std::cout << " List of unmatched events: ";
    for (auto iter = vec_eventidx_2L1.begin(); iter != vec_eventidx_2L1.end(); ++iter)
    {
        std::cout << *iter << ", ";
    }
    std::cout << std::endl;
    */
    std::cout << "Number of 0L1-0Fee events right after 2L1 events: " << vec_eventidx_matched.size() << std::endl;
    std::cout << "Number of standalone/isolated 0L1-0Fee events: " << vec_eventidx_2L1.size() << std::endl;
    std::cout << "Fraction of matched events (how many 0L1-0Fee events occur after 2L1 events): " << (double)vec_eventidx_matched.size() / (double)(vec_eventidx_matched.size() + vec_eventidx_2L1.size()) * 100. << "%" << std::endl;

    // Compare the L1 BCOs with the GL1 BCOs
    std::set<uint64_t> set_uniqueGL1bco(vec_uniqueGL1bco.begin(), vec_uniqueGL1bco.end());
    std::vector<uint64_t> vec_matched_GL1bco_L1, vec_unmatched_GL1bco_L1;
    auto contains = [&](uint64_t target) { return std::binary_search(vec_uniqueL1bco.begin(), vec_uniqueL1bco.end(), target); };

    for (uint64_t b : vec_uniqueGL1bco)
    {
        if (contains(b + 2))
        {
            vec_matched_GL1bco_L1.push_back(b);
        }
        else
        {
            vec_unmatched_GL1bco_L1.push_back(b);
        }
    }

    // Compare the fee bcos with the GL1 BCOs
    uint64_t range = 100;
    std::vector<uint64_t> vec_matched_Gl1bco_Fee, vec_unmatched_Gl1bco_Fee;
    for (size_t i = 0; i < vec_uniqueGL1bco.size(); ++i)
    {
        uint64_t target = vec_uniqueGL1bco[i];

        for (uint64_t b_fee : vec_flat_uniquefeebco)
        {
            if (diff(target, b_fee) <= range)
            {
                map_FeeBco_matched2GL1Bco[b_fee] = true;
                map_GL1Bco_matched2FeeBco[target] = true;
                map_GL1BCO_pairMatchedFeeBcoNFees[target] = std::make_pair(b_fee, map_FeeBco_nFeeIDs[b_fee]);
                // remove the matched bcos from the vector
                vec_flat_uniquefeebco.erase(std::remove(vec_flat_uniquefeebco.begin(), vec_flat_uniquefeebco.end(), b_fee), vec_flat_uniquefeebco.end());
                break;
            }
        }
    }

    // print out the results of matchings
    std::cout << "Number of matched GL1/L1 BCOs: " << vec_matched_GL1bco_L1.size() << std::endl;
    std::cout << "Number of unmatched GL1/L1 BCOs: " << vec_unmatched_GL1bco_L1.size() << std::endl;
    std::cout << "Fraction of matched GL1/L1 BCOs: " << (double)vec_matched_GL1bco_L1.size() / (double)(vec_matched_GL1bco_L1.size() + vec_unmatched_GL1bco_L1.size()) * 100. << "%" << std::endl;
    // find the event index of the unmatched GL1 BCOs
    std::vector<int> vec_eventidx_unmatched_GL1bco;
    for (uint64_t b : vec_unmatched_GL1bco_L1)
    {
        auto iter = std::find_if(map_eventidx_GL1bco.begin(), map_eventidx_GL1bco.end(), [b](const std::pair<const int, uint64_t> &p) { return p.second == b; });
        if (iter != map_eventidx_GL1bco.end())
        {
            vec_eventidx_unmatched_GL1bco.push_back(iter->first);
        }
    }
    std::cout << "List of event indices of unmatched GL1 BCOs: ";
    for (int idx : vec_eventidx_unmatched_GL1bco)
    {
        std::cout << idx << ", ";
    }
    std::cout << std::endl;

    // print out map_GL1BCO_pairMatchedFeeBcoNFees
    int nMatched_GL1_Fee = 0;
    vector<uint64_t> vec_GL1Bco_matched2FeeBco;
    vector<int> vec_nFeeIDs_GL1Matched2FeeBco;
    for (auto iter = map_GL1BCO_pairMatchedFeeBcoNFees.begin(); iter != map_GL1BCO_pairMatchedFeeBcoNFees.end(); ++iter)
    {
        nMatched_GL1_Fee++;
        vec_GL1Bco_matched2FeeBco.push_back(iter->first);
        vec_nFeeIDs_GL1Matched2FeeBco.push_back(iter->second.second);
        // std::cout << "GL1 BCO: " << iter->first << ", FEE BCO: " << iter->second.first << ", nFeeIDs: " << iter->second.second << std::endl;
    }
    std::cout << "Number of matched GL1/FEE BCOs: " << nMatched_GL1_Fee << std::endl;

    // print map_GL1Bco_matched2FeeBco if the value is false, and find the corresponding event index using map_eventidx_GL1bco
    std::vector<int> vec_eventidx_unmatched_GL1bco_Fee;
    for (auto iter = map_GL1Bco_matched2FeeBco.begin(); iter != map_GL1Bco_matched2FeeBco.end(); ++iter)
    {
        if (!iter->second)
        {
            auto iter2 = std::find_if(map_eventidx_GL1bco.begin(), map_eventidx_GL1bco.end(), [iter](const std::pair<const int, uint64_t> &p) { return p.second == iter->first; });
            if (iter2 != map_eventidx_GL1bco.end())
            {
                vec_eventidx_unmatched_GL1bco_Fee.push_back(iter2->first);
            }
        }
    }
    std::cout << "Number of unmatched GL1/FEE BCOs: " << vec_eventidx_unmatched_GL1bco_Fee.size() << std::endl;
    std::cout << "List of event indices of unmatched GL1/FEE BCOs: ";
    for (int idx : vec_eventidx_unmatched_GL1bco_Fee)
    {
        std::cout << idx << ", ";
    }
    std::cout << std::endl;

    // std::cout << "Number of matched GL1/FEE BCOs: " << vec_matched_Gl1bco_Fee.size() << std::endl;
    // std::cout << "Number of unmatched GL1/FEE BCOs: " << vec_unmatched_Gl1bco_Fee.size() << std::endl;
    // std::cout << "Fraction of matched GL1/FEE BCOs: " << (double)vec_matched_Gl1bco_Fee.size() / (double)(vec_matched_Gl1bco_Fee.size() + vec_unmatched_Gl1bco_Fee.size()) * 100. << "%" << std::endl;
    // // find the event index of the unmatched GL1 BCOs
    // std::vector<int> vec_eventidx_unmatched_Gl1bco_Fee;
    // for (uint64_t b : vec_unmatched_Gl1bco_Fee)
    // {
    //     auto iter = std::find_if(map_eventidx_GL1bco.begin(), map_eventidx_GL1bco.end(), [b](const std::pair<const int, uint64_t> &p) { return p.second == b; });
    //     if (iter != map_eventidx_GL1bco.end())
    //     {
    //         vec_eventidx_unmatched_Gl1bco_Fee.push_back(iter->first);
    //     }
    // }
    // std::cout << "List of event indices of unmatched GL1/FEE BCOs: ";
    // for (int idx : vec_eventidx_unmatched_Gl1bco_Fee)
    // {
    //     std::cout << idx << ", ";
    // }
    // std::cout << std::endl;

    // print out the first 20 elements of vec_uniqueGL1bco and vec_uniqueFeecbo
    for (int i = 495; i < 495 + 10; i++)
    {
        std::cout << i << " GL1 bco = " << vec_uniqueGL1bco[i] << std::endl;
        for (uint64_t b : vec_uniquefeebco[i])
        {
            std::cout << "   FEE bco = " << b << ", diff = " << diff(vec_uniqueGL1bco[i], b) << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    gStyle->SetPaintTextFormat("g %%");
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    gPad->SetRightMargin(0.18);
    h_MvtxRawEvtHeader_nBCOconf->Scale(100.0 / h_MvtxRawEvtHeader_nBCOconf->Integral(-1, -1, -1, -1));
    h_MvtxRawEvtHeader_nBCOconf->GetXaxis()->SetBinLabel(1, "0");
    h_MvtxRawEvtHeader_nBCOconf->GetXaxis()->SetBinLabel(2, "1");
    h_MvtxRawEvtHeader_nBCOconf->GetXaxis()->SetBinLabel(3, "2");
    h_MvtxRawEvtHeader_nBCOconf->GetXaxis()->SetBinLabel(4, "#geq2");
    h_MvtxRawEvtHeader_nBCOconf->GetYaxis()->SetBinLabel(1, "0");
    h_MvtxRawEvtHeader_nBCOconf->GetYaxis()->SetBinLabel(2, "1");
    h_MvtxRawEvtHeader_nBCOconf->GetYaxis()->SetBinLabel(3, "2");
    h_MvtxRawEvtHeader_nBCOconf->GetYaxis()->SetBinLabel(4, "#geq2");
    h_MvtxRawEvtHeader_nBCOconf->GetXaxis()->SetLabelSize(0.08);
    h_MvtxRawEvtHeader_nBCOconf->GetYaxis()->SetLabelSize(0.08);
    h_MvtxRawEvtHeader_nBCOconf->GetZaxis()->SetRangeUser(0, 100);
    h_MvtxRawEvtHeader_nBCOconf->GetZaxis()->SetTitleOffset(1.25);
    h_MvtxRawEvtHeader_nBCOconf->SetMarkerSize(2);
    h_MvtxRawEvtHeader_nBCOconf->Draw("colztext45");
    static TExec *ex1 = new TExec("ex1", "Pal_blue();");
    ex1->Draw();
    h_MvtxRawEvtHeader_nBCOconf->Draw("colztext45same");
    c->RedrawAxis();
    c->SaveAs(Form("%s/MvtxRawEvtHeader_nBCOconf_Run%d.png", plotdir_rawevtheader.c_str(), runnumber));
    c->SaveAs(Form("%s/MvtxRawEvtHeader_nBCOconf_Run%d.pdf", plotdir_rawevtheader.c_str(), runnumber));

    c->Clear();
    c->cd();
    gPad->SetRightMargin(0.11);
    std::vector<Double_t> vec_nFeeIDs_double(vec_nFeeIDs_GL1Matched2FeeBco.begin(), vec_nFeeIDs_GL1Matched2FeeBco.end());
    std::vector<Double_t> vec_GL1bco_double(vec_GL1Bco_matched2FeeBco.begin(), vec_GL1Bco_matched2FeeBco.end());
    TGraph *gr_nFeeIDs = new TGraph(vec_nFeeIDs_double.size(), &vec_GL1bco_double[0], &vec_nFeeIDs_double[0]);
    gr_nFeeIDs->SetMarkerStyle(20);
    gr_nFeeIDs->SetMarkerSize(0.2);
    gr_nFeeIDs->GetXaxis()->SetTitle("GL1 BCO");
    gr_nFeeIDs->GetYaxis()->SetTitle("FEE Counts");
    gr_nFeeIDs->GetYaxis()->SetTitleOffset(1.4);
    gr_nFeeIDs->GetYaxis()->SetRangeUser(0, 160);
    gr_nFeeIDs->GetXaxis()->SetNdivisions(505);
    // gr_nFeeIDs->GetXaxis()->SetLabelSize(0.03);
    gr_nFeeIDs->Draw("ap");
    c->SaveAs(Form("%s/MvtxRawEvtHeader_nFeeIDs_Run%d.png", plotdir_rawevtheader.c_str(), runnumber));
    c->SaveAs(Form("%s/MvtxRawEvtHeader_nFeeIDs_Run%d.pdf", plotdir_rawevtheader.c_str(), runnumber));

    f->Close();
}