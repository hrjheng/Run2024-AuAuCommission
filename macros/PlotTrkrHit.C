#include <iostream>
#include <map>
#include <utility>
#include <vector>

double npixels_per_stave = 9 * 512 * 1024;

void Draw1DHist(TH1F *h, TString plotinfo, TString plotname)
{
    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    gPad->SetRightMargin(0.05);
    gPad->SetTopMargin(0.08);
    h->GetYaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetRangeUser(0, h->GetMaximum() * 1.2);
    h->Draw("hist");
    TLatex *tex = new TLatex(0.85, 0.85, plotinfo);
    tex->SetTextAlign(31);
    tex->SetNDC();
    tex->SetTextSize(0.04);
    tex->Draw();
    c->SaveAs(plotname + ".png");
    c->SaveAs(plotname + ".pdf");
}

void drawHitMap(TH2F *h, vector<TString> plotinfo, TString plotname)
{
    // int f = 0;
    // gStyle->SetPalette(kDarkBodyRadiator);
    // gStyle->SetPalette(kSunset);
    // gStyle->SetCanvasColor(1);
    TCanvas *c = new TCanvas("c", "c", 1100, 600);
    gPad->SetTopMargin(0.2);
    gPad->SetRightMargin(0.12);
    gPad->SetLeftMargin(0.12);
    c->cd();
    h->GetYaxis()->SetTitleOffset(1.);
    // h->GetZaxis()->SetRangeUser(0, h->GetMaximum(nevt_seg/2));
    h->Draw("colz");
    gPad->Update(); // make sure it's really drawn
    // h->SetAxisColor(f,"xyz");
    // h->SetLabelColor(f,"xyz");
    // h->GetXaxis()->SetTitleColor(f);
    // h->GetYaxis()->SetTitleColor(f);
    // gPad->Update(); // make sure it's re-drawn

    // Add the plotinfo to the plot
    float rmargin = gPad->GetRightMargin();
    for (int i = 0; i < plotinfo.size(); i++)
    {
        TLatex *tex = new TLatex(rmargin, 0.9 - i * 0.05, plotinfo[i]);
        tex->SetTextAlign(11);
        tex->SetNDC();
        tex->SetTextSize(0.04);
        tex->Draw();
    }
    c->SaveAs(plotname + ".pdf");
    c->SaveAs(plotname + ".png");
    delete c;
}

void PlotTrkrHit(TString inputfile = "./qahists/53548/ntp-nEvent10000-skip0.root", int runnumber = 53548)
{
    std::string plotdir_hitmap = "./plots/Run" + std::to_string(runnumber) + "/Hitmap";
    system(Form("mkdir -p %s", plotdir_hitmap.c_str()));
    std::string plotdir_hit = "./plots/Run" + std::to_string(runnumber) + "/TrkrHit";
    system(Form("mkdir -p %s", plotdir_hit.c_str()));

    std::map<std::pair<int, int>, double> stave_occupancy;
    TH1F *hM_NTrkrHitsStave = new TH1F("hM_NTrkrHitsStave", ";Number of hits in single stave; Fraction [%]", 25, 0, 50000);
    TH1F *hM_TrkrHitPhi_L0 = new TH1F("hM_TrkrHitPhi_L0", ";TrkrHit #phi [rad]; Entries", 128, -3.2, 3.2);
    TH1F *hM_TrkrHitPhi_L1 = new TH1F("hM_TrkrHitPhi_L1", ";TrkrHit #phi [rad]; Entries", 128, -3.2, 3.2);
    TH1F *hM_TrkrHitPhi_L2 = new TH1F("hM_TrkrHitPhi_L2", ";TrkrHit #phi [rad]; Entries", 128, -3.2, 3.2);
    TH1F *hM_TrkrHitPhi_Incl = new TH1F("hM_TrkrHitPhi_Incl", ";TrkrHit #phi [rad]; Entries", 128, -3.2, 3.2);
    // TH2F *hM_HitMap_L0_integrate = new TH2F("hM_HitMap_L0_integrate", ";TrkrHit Z [cm];TrkrHit #phi [rad];", 3000, -15, 15, 3200, -3.2, 3.2);

    TFile *f = new TFile(inputfile, "READ");
    TTree *tree = (TTree *)f->Get("ntuple");
    Int_t event;
    uint64_t GL1Packet_BCO = 0;
    vector<int> *layer = 0;
    vector<int> *stave = 0;
    vector<int> *chip = 0;
    vector<float> *trkrhitX = 0;
    vector<float> *trkrhitY = 0;
    vector<float> *trkrhitZ = 0;
    tree->SetBranchAddress("event", &event);
    tree->SetBranchAddress("GL1Packet_BCO", &GL1Packet_BCO);
    tree->SetBranchAddress("TrkrHit_layer", &layer);
    tree->SetBranchAddress("TrkrHit_stave", &stave);
    tree->SetBranchAddress("TrkrHit_chip", &chip);
    tree->SetBranchAddress("TrkrHit_globalX", &trkrhitX);
    tree->SetBranchAddress("TrkrHit_globalY", &trkrhitY);
    tree->SetBranchAddress("TrkrHit_globalZ", &trkrhitZ);

    for (int i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);

        map<std::pair<int, int>, int> stave_occupancy_perevt;
        for (int j = 0; j < layer->size(); j++)
        {
            std::pair<int, int> layer_stave = std::make_pair(layer->at(j), stave->at(j));
            stave_occupancy[layer_stave] += 1;
            stave_occupancy_perevt[layer_stave] += 1;

            float trkrhitphi = atan2(trkrhitY->at(j), trkrhitX->at(j));
            hM_TrkrHitPhi_Incl->Fill(trkrhitphi);
            if (layer->at(j) == 0)
            {
                hM_TrkrHitPhi_L0->Fill(trkrhitphi);
            }
            else if (layer->at(j) == 1)
            {
                hM_TrkrHitPhi_L1->Fill(trkrhitphi);
            }
            else if (layer->at(j) == 2)
            {
                hM_TrkrHitPhi_L2->Fill(trkrhitphi);
            }

            // hM_HitMap_L0_integrate->Fill(trkrhitZ->at(j), trkrhitphi);
        }

        // high occupancy
        // bool highoccup = false;
        for (auto &it : stave_occupancy_perevt)
        {
            hM_NTrkrHitsStave->Fill(it.second);
            if (it.second > 10000)
            {
                TH2F *hitmap_layer0 = new TH2F(Form("hitmap_layer0)%lu", GL1Packet_BCO), ";TrkrHit Z [cm];TrkrHit #phi [rad];", 3000, -15, 15, 3200, -3.2, 3.2);
                TH2F *hitmap_layer1 = new TH2F(Form("hitmap_layer1_%lu", GL1Packet_BCO), ";TrkrHit Z [cm];TrkrHit #phi [rad];", 3000, -15, 15, 3200, -3.2, 3.2);
                TH2F *hitmap_layer2 = new TH2F(Form("hitmap_layer2_%lu", GL1Packet_BCO), ";TrkrHit Z [cm];TrkrHit #phi [rad];", 3000, -15, 15, 3200, -3.2, 3.2);
                std::cout << "Event index: " << event << " GL1 BCO: " << GL1Packet_BCO << " - High occupancy in L" << it.first.first << "_" << it.first.second << " = " << it.second << std::endl;

                for (int j = 0; j < layer->size(); j++)
                {
                    float trkrhitphi = atan2(trkrhitY->at(j), trkrhitX->at(j));
                    if (layer->at(j) == 0)
                    {
                        hitmap_layer0->Fill(trkrhitZ->at(j), trkrhitphi);
                    }
                    else if (layer->at(j) == 1)
                    {
                        hitmap_layer1->Fill(trkrhitZ->at(j), trkrhitphi);
                    }
                    else if (layer->at(j) == 2)
                    {
                        hitmap_layer2->Fill(trkrhitZ->at(j), trkrhitphi);
                    }
                }

                TString whatstave = TString::Format("High occupancy in L%d_%d (Number of hits = %d)", it.first.first, it.first.second, it.second);
                drawHitMap(hitmap_layer0, {TString::Format("GL1 BCO %lu, Layer 0", GL1Packet_BCO), whatstave}, Form("%s/hitmap_layer0_GL1BCO%lu", plotdir_hitmap.c_str(), GL1Packet_BCO));
                drawHitMap(hitmap_layer1, {TString::Format("GL1 BCO %lu, Layer 1", GL1Packet_BCO), whatstave}, Form("%s/hitmap_layer1_GL1BCO%lu", plotdir_hitmap.c_str(), GL1Packet_BCO));
                drawHitMap(hitmap_layer2, {TString::Format("GL1 BCO %lu, Layer 2", GL1Packet_BCO), whatstave}, Form("%s/hitmap_layer2_GL1BCO%lu", plotdir_hitmap.c_str(), GL1Packet_BCO));

                delete hitmap_layer0;
                delete hitmap_layer1;
                delete hitmap_layer2;
            }
        }
    }

    // drawHitMap(hM_HitMap_L0_integrate, {TString::Format("Integrated over first %lld strobes", tree->GetEntriesFast()), "Layer 0"}, Form("%s/hitmap_layer0_Integrated", plotdir_hitmap.c_str()));

    std::map<std::pair<int, int>, TH1F *> map_stave_th1fchipoccup;
    TH1F *h_stave_avgoccupancy = new TH1F("h_stave_avgoccupancy", "Stave Hit Occupancy", 48, 0, 48);
    TH2F *h_stave_occcupancy = new TH2F("h_stave_occcupancy", "Stave Hit Occupancy", 48, 0, 48, 61, 0, 0.61); // The last bin is for overflow
    int bin = 1;
    for (auto &it : stave_occupancy)
    {
        TH1F *h = (TH1F *)f->Get(Form("hM_chipoccup_L%d_%d", it.first.first, it.first.second));
        h->Scale(1.0 / h->Integral(-1, -1));
        for (int i = 1; i <= h->GetNbinsX() + 1; i++)
        {
            h_stave_occcupancy->SetBinContent(bin, i, h->GetBinContent(i));
        }
        h_stave_occcupancy->GetXaxis()->SetBinLabel(bin, Form("L%d_%d", it.first.first, it.first.second));

        stave_occupancy[it.first] = (it.second / tree->GetEntriesFast()) / npixels_per_stave * 100;
        h_stave_avgoccupancy->GetXaxis()->SetBinLabel(bin, Form("L%d_%d", it.first.first, it.first.second));
        h_stave_avgoccupancy->SetBinContent(bin, stave_occupancy[it.first]);
        bin++;
    }

    vector<TString> plotinfo = {TString::Format("Run %d, first 10000 strobes", runnumber)};
    TCanvas *c = new TCanvas("c", "c", 1300, 700);
    gPad->SetTopMargin(0.2);
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
    TLine *l_L1 = new TLine(12 + 16, 0, 12 + 16, gPad->GetUymax());
    l_L1->Draw();
    for (int i = 0; i < plotinfo.size(); i++)
    {
        TLatex *tex = new TLatex(gPad->GetLeftMargin(), 0.9 - i * 0.05, plotinfo[i]);
        tex->SetTextAlign(11);
        tex->SetNDC();
        tex->SetTextSize(0.045);
        tex->Draw();
    }
    c->SaveAs(Form("./plots/Run%d/staveHitOccupancy_Run%d.png", runnumber, runnumber));
    c->SaveAs(Form("./plots/Run%d/staveHitOccupancy_Run%d.pdf", runnumber, runnumber));

    c->Clear();
    c->cd();
    c->SetLogz();
    gPad->SetTopMargin(0.2);
    gPad->SetRightMargin(0.15);
    h_stave_occcupancy->GetYaxis()->SetTitle("Occupancy [%]");
    h_stave_occcupancy->GetYaxis()->SetTitleOffset(1.3);
    h_stave_occcupancy->GetZaxis()->SetRangeUser(5E-4, 1);
    h_stave_occcupancy->LabelsOption("v");
    h_stave_occcupancy->Draw("COLZ");
    gPad->Update();
    TLine *l_L0_2D = new TLine(12, 0, 12, gPad->GetUymax());
    l_L0_2D->Draw();
    TLine *l_L1_2D = new TLine(12 + 16, 0, 12 + 16, gPad->GetUymax());
    l_L1_2D->Draw();
    for (int i = 0; i < plotinfo.size(); i++)
    {
        TLatex *tex = new TLatex(gPad->GetLeftMargin(), 0.9 - i * 0.05, plotinfo[i]);
        tex->SetTextAlign(11);
        tex->SetNDC();
        tex->SetTextSize(0.045);
        tex->Draw();
    }
    c->SaveAs(Form("./plots/Run%d/staveHitOccupancy2D_Run%d.png", runnumber, runnumber));
    c->SaveAs(Form("./plots/Run%d/staveHitOccupancy2D_Run%d.pdf", runnumber, runnumber));

    Draw1DHist(hM_TrkrHitPhi_L0, "Layer 0", Form("%s/TrkrHitPhi_L0", plotdir_hit.c_str()));
    Draw1DHist(hM_TrkrHitPhi_L1, "Layer 1", Form("%s/TrkrHitPhi_L1", plotdir_hit.c_str()));
    Draw1DHist(hM_TrkrHitPhi_L2, "Layer 2", Form("%s/TrkrHitPhi_L2", plotdir_hit.c_str()));

    TCanvas *c_nhitsStave = new TCanvas("c_nhitsStave", "c_nhitsStave", 800, 700);
    c_nhitsStave->cd();
    gPad->SetRightMargin(0.1);
    // set the number of digits in x-axis labels to be 3
    TGaxis::SetMaxDigits(3);
    hM_NTrkrHitsStave->Scale(100.0 / hM_NTrkrHitsStave->Integral(-1, -1));
    hM_NTrkrHitsStave->GetYaxis()->SetRangeUser(0, hM_NTrkrHitsStave->GetMaximum() * 1.3);
    // include overflow bin
    hM_NTrkrHitsStave->GetXaxis()->SetRange(1, hM_NTrkrHitsStave->GetNbinsX() + 1);
    // hM_NTrkrHitsStave->GetXaxis()->SetBinLabel(hM_NTrkrHitsStave->GetNbinsX() + 1, ">=50000");
    hM_NTrkrHitsStave->Draw("histtext");
    for (int i = 0; i < plotinfo.size(); i++)
    {
        TLatex *tex = new TLatex(1 - gPad->GetRightMargin() - 0.05, 0.87 - i * 0.05, plotinfo[i]);
        tex->SetTextAlign(31);
        tex->SetNDC();
        tex->SetTextSize(0.045);
        tex->Draw();
    }
    c_nhitsStave->SaveAs(Form("./plots/Run%d/NTrkrHitsStave_Run%d.png", runnumber, runnumber));
    c_nhitsStave->SaveAs(Form("./plots/Run%d/NTrkrHitsStave_Run%d.pdf", runnumber, runnumber));
}