#include <iostream>
#include <map>
#include <utility>
#include <vector>

double npixels_per_stave = 9 * 512 * 1024;

void plot_staveHitOccup(TString inputfile = "./qahists/53548/ntp-nEvent10000-skip0.root", int runnumber = 53548)
{
    std::map<std::pair<int, int>, double> stave_occupancy;

    TFile *f = new TFile(inputfile, "READ");
    TTree *tree = (TTree *)f->Get("ntuple");
    Int_t event;
    vector<int> *layer = 0;
    vector<int> *stave = 0;
    vector<int> *chip = 0;
    vector<float> *trkrhitX = 0;
    vector<float> *trkrhitY = 0;
    vector<float> *trkrhitZ = 0;
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("TrkrHit_layer", &layer);
    tree->SetBranchAddress("TrkrHit_stave", &stave);
    tree->SetBranchAddress("TrkrHit_chip", &chip);
    tree->SetBranchAddress("TrkrHit_globalX", &trkrhitX);
    tree->SetBranchAddress("TrkrHit_globalY", &trkrhitY);
    tree->SetBranchAddress("TrkrHit_globalZ", &trkrhitZ);

    for (int i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        for (int j = 0; j < layer->size(); j++)
        {
            std::pair<int, int> layer_stave = std::make_pair(layer->at(j), stave->at(j));
            stave_occupancy[layer_stave] += 1;
        }
    }

    std::map<std::pair<int, int>, TH1F*> map_stave_th1fchipoccup;
    TH1F *h_stave_avgoccupancy = new TH1F("h_stave_avgoccupancy", "Stave Hit Occupancy", 48, 0, 48);
    TH2F *h_stave_occcupancy = new TH2F("h_stave_occcupancy", "Stave Hit Occupancy", 48, 0, 48, 61, 0, 0.61); // The last bin is for overflow
    int bin = 1;
    for (auto &it : stave_occupancy)
    {
        TH1F *h = (TH1F *) f->Get(Form("hM_chipoccup_L%d_%d", it.first.first, it.first.second));
        h->Scale(1.0 / h->Integral(-1,-1));
        // loop over all bins, including overflow, and fill the occupancy histogram
        for (int i = 1; i <= h->GetNbinsX()+1; i++)
        {
            h_stave_occcupancy->SetBinContent(bin, i, h->GetBinContent(i));
        }
        h_stave_occcupancy->GetXaxis()->SetBinLabel(bin, Form("L%d_%d", it.first.first, it.first.second));

        stave_occupancy[it.first] = (it.second / tree->GetEntriesFast()) / npixels_per_stave * 100;
        h_stave_avgoccupancy->GetXaxis()->SetBinLabel(bin, Form("L%d_%d", it.first.first, it.first.second));
        h_stave_avgoccupancy->SetBinContent(bin, stave_occupancy[it.first]);
        bin++;
    }

    TCanvas *c = new TCanvas("c", "c", 1300, 700);
    c->cd();
    h_stave_avgoccupancy->SetLineColor(kBlue - 2);
    h_stave_avgoccupancy->SetLineWidth(2);
    h_stave_avgoccupancy->SetMarkerColor(kBlue - 2);
    h_stave_avgoccupancy->SetMarkerStyle(20);
    h_stave_avgoccupancy->SetMarkerSize(1.5);
    h_stave_avgoccupancy->GetYaxis()->SetTitle("Average Occupancy [%]");
    h_stave_avgoccupancy->GetYaxis()->SetTitleOffset(1.3);
    h_stave_avgoccupancy->GetYaxis()->SetRangeUser(0, h_stave_avgoccupancy->GetMaximum() * 1.1);
    h_stave_avgoccupancy->LabelsOption("v");
    h_stave_avgoccupancy->Draw("P");
    gPad->Update();
    TLine *l_L0 = new TLine(12, 0, 12, gPad->GetUymax());
    l_L0->Draw();
    TLine *l_L1 = new TLine(12+16, 0, 12+16, gPad->GetUymax());
    l_L1->Draw();
    c->SaveAs(Form("./plots/staveHitOccupancy_Run%d.png", runnumber));
    c->SaveAs(Form("./plots/staveHitOccupancy_Run%d.pdf", runnumber));

    c->Clear();
    c->cd();
    c->SetLogz();
    gPad->SetRightMargin(0.15);
    h_stave_occcupancy->GetYaxis()->SetTitle("Occupancy [%]");
    h_stave_occcupancy->GetYaxis()->SetTitleOffset(1.3);
    h_stave_occcupancy->GetZaxis()->SetRangeUser(5E-4, 1);
    h_stave_occcupancy->LabelsOption("v");
    h_stave_occcupancy->Draw("COLZ");
    gPad->Update();
    TLine *l_L0_2D = new TLine(12, 0, 12, gPad->GetUymax());
    l_L0_2D->Draw();
    TLine *l_L1_2D = new TLine(12+16, 0, 12+16, gPad->GetUymax());
    l_L1_2D->Draw();
    c->SaveAs(Form("./plots/staveHitOccupancy2D_Run%d.png", runnumber));
    c->SaveAs(Form("./plots/staveHitOccupancy2D_Run%d.pdf", runnumber));
}