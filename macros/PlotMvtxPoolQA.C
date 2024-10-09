#include "Utils.h"

const int nPackats = 12; // 12 FEEs per endpoint
const int nFelix = 12;   // 6 FELIX x 2 endpoints

void PlotMvtxPoolQA(TString fname = "./qahists/53539/qahist-streamprod-nEvent10000.root", int runnumber = 53539)
{
    TFile *f = TFile::Open(fname.Data(), "READ");
    if (!f || f->IsZombie())
    {
        cout << "Unable to open " << fname << " for reading..." << endl;
        return;
    }

    TString outdir = TString::Format("./plots/QAdump/Run%d", runnumber);
    gSystem->Exec(Form("mkdir -p %s", outdir.Data()));

    TH1 *h_MvtxPoolQA_RefGL1BCO = GetObjectFromFile<TH1>(f, "h_MvtxPoolQA_RefGL1BCO");
    TH1 *h_MvtxPoolQA_TagBCOAllFelixs = GetObjectFromFile<TH1>(f, "h_MvtxPoolQA_TagBCOAllFelixs");
    TH1 *h_MvtxPoolQA_TagBCOAllFelixsAllFees = GetObjectFromFile<TH1>(f, "h_MvtxPoolQA_TagBCOAllFelixsAllFees");
    TH1 *h_MvtxPoolQA_TagStBcoFEEs = GetObjectFromFile<TH1>(f, "h_MvtxPoolQA_TagStBcoFEEs");

    string str_gl1ll1bcodiff = "h_MvtxPoolQA_GL1LL1BCODiff_packet";
    string str_tagbcoallfees = "h_MvtxPoolQA_TagBCOAllFees_Felix";
    string str_tagbco_felix = "h_MvtxPoolQA_TagBCO_felix"; // number of tagged FELIX endpoints; "tagged" means the absolute value of the difference between the reference BCO and the gtm BCO is less than 3; https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/framework/fun4allraw/Fun4AllStreamingInputManager.cc#L910-L948
    string str_tagstbco_felix = "h_MvtxPoolQA_TagStBco_felix";

    vector<TH1 *> vec_h_MvtxPoolQA_GL1LL1BCODiff_packet;
    vector<TH1 *> vec_h_MvtxPoolQA_TagBCOAllFees_Felix;
    vector<TH1 *> vec_h_MvtxPoolQA_TagBCO_felix;
    vector<TH1 *> vec_h_MvtxPoolQA_TagStBco_felix;

    for (int i = 0; i < nPackats; i++)
    {
        vec_h_MvtxPoolQA_GL1LL1BCODiff_packet.push_back(GetObjectFromFile<TH1>(f, str_gl1ll1bcodiff + to_string(i)));
    }

    for (int i = 0; i < nFelix; i++)
    {
        vec_h_MvtxPoolQA_TagBCOAllFees_Felix.push_back(GetObjectFromFile<TH1>(f, str_tagbcoallfees + to_string(i)));
        vec_h_MvtxPoolQA_TagBCO_felix.push_back(GetObjectFromFile<TH1>(f, str_tagbco_felix + to_string(i)));
        vec_h_MvtxPoolQA_TagStBco_felix.push_back(GetObjectFromFile<TH1>(f, str_tagstbco_felix + to_string(i)));
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    c->cd();
    h_MvtxPoolQA_RefGL1BCO->GetXaxis()->SetRangeUser(0, 100);
    h_MvtxPoolQA_RefGL1BCO->GetYaxis()->SetTitle("Counts");
    h_MvtxPoolQA_RefGL1BCO->Draw("hist");
    c->SaveAs(Form("%s/%s.png", outdir.Data(), h_MvtxPoolQA_RefGL1BCO->GetName()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h_MvtxPoolQA_RefGL1BCO->GetName()));

    c->Clear();
    c->cd();
    h_MvtxPoolQA_TagBCOAllFelixs->GetXaxis()->SetRangeUser(0, 100);
    h_MvtxPoolQA_TagBCOAllFelixs->GetYaxis()->SetTitle("Counts");
    h_MvtxPoolQA_TagBCOAllFelixs->Draw("hist");
    c->SaveAs(Form("%s/%s.png", outdir.Data(), h_MvtxPoolQA_TagBCOAllFelixs->GetName()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h_MvtxPoolQA_TagBCOAllFelixs->GetName()));

    c->Clear();
    c->cd();
    h_MvtxPoolQA_TagBCOAllFelixsAllFees->GetXaxis()->SetRangeUser(0, 100);
    h_MvtxPoolQA_TagBCOAllFelixsAllFees->GetYaxis()->SetTitle("Counts");
    h_MvtxPoolQA_TagBCOAllFelixsAllFees->Draw("hist");
    c->SaveAs(Form("%s/%s.png", outdir.Data(), h_MvtxPoolQA_TagBCOAllFelixsAllFees->GetName()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h_MvtxPoolQA_TagBCOAllFelixsAllFees->GetName()));

    c->Clear();
    c->cd();
    gPad->SetRightMargin(0.1);
    TGaxis::SetMaxDigits(3);
    h_MvtxPoolQA_TagBCOAllFelixsAllFees->GetYaxis()->SetTitle("Counts");
    h_MvtxPoolQA_TagStBcoFEEs->Draw("hist");
    c->SaveAs(Form("%s/%s.png", outdir.Data(), h_MvtxPoolQA_TagStBcoFEEs->GetName()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h_MvtxPoolQA_TagStBcoFEEs->GetName()));

    c->Clear();
    c->cd();
    c->SetLogy();
    gPad->SetRightMargin(0.1);
    TGaxis::SetMaxDigits(3);
    TLegend *leg = new TLegend(0.67, 0.5, 0.87, 0.9);
    float max = 0;
    for (int i = 0; i < nPackats; i++)
    {
        if (vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i]->GetMaximum() > max)
            max = vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i]->GetMaximum();
        if (i == 0)
        {
            vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i]->GetYaxis()->SetTitle("Counts");
            vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i]->Draw("hist PLC");
        }
        else
            vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i]->Draw("hist PLC same");
        
        leg->AddEntry(vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[i], Form("Packet %d", i), "l");
    }
    vec_h_MvtxPoolQA_GL1LL1BCODiff_packet[0]->GetYaxis()->SetRangeUser(0.5, max * 5);
    leg->SetTextSize(0.035);
    leg->SetFillColor(0);
    leg->SetFillStyle(0);
    leg->Draw();
    c->SaveAs(Form("%s/%s.png", outdir.Data(), str_gl1ll1bcodiff.c_str()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), str_gl1ll1bcodiff.c_str()));

    c->Clear();
    c->cd();
    c->SetLogy(0);
    gPad->SetRightMargin(0.1);
    TGaxis::SetMaxDigits(3);
    max = 0;
    for (int i = 0; i < nFelix; i++)
    {
        if (vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->GetMaximum() > max)
            max = vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->GetMaximum();
        if (i == 0)
        {
            vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->GetXaxis()->SetRangeUser(0, 100);
            vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->GetYaxis()->SetTitle("Counts");
            vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->Draw("hist PLC");
        }
        else
            vec_h_MvtxPoolQA_TagBCOAllFees_Felix[i]->Draw("hist PLC same");
    }
    vec_h_MvtxPoolQA_TagBCOAllFees_Felix[0]->GetYaxis()->SetRangeUser(0, max * 1.05);
    leg->Draw();
    c->SaveAs(Form("%s/%s.png", outdir.Data(), str_tagbcoallfees.c_str()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), str_tagbcoallfees.c_str()));

    c->Clear();
    c->cd();
    c->SetLogy(0);
    gPad->SetRightMargin(0.1);
    TGaxis::SetMaxDigits(3);
    max = 0;
    for (int i = 0; i < nFelix; i++)
    {
        if (vec_h_MvtxPoolQA_TagBCO_felix[i]->GetMaximum() > max)
            max = vec_h_MvtxPoolQA_TagBCO_felix[i]->GetMaximum();
        if (i == 0)
        {
            vec_h_MvtxPoolQA_TagBCO_felix[i]->GetXaxis()->SetRangeUser(0, 100);
            vec_h_MvtxPoolQA_TagBCO_felix[i]->GetYaxis()->SetTitle("Counts");
            vec_h_MvtxPoolQA_TagBCO_felix[i]->Draw("hist PLC");
        }
        else
            vec_h_MvtxPoolQA_TagBCO_felix[i]->Draw("hist PLC same");
    }
    vec_h_MvtxPoolQA_TagBCO_felix[0]->GetYaxis()->SetRangeUser(0, max * 1.05);
    leg->Draw();
    c->SaveAs(Form("%s/%s.png", outdir.Data(), str_tagbco_felix.c_str()));
    c->SaveAs(Form("%s/%s.pdf", outdir.Data(), str_tagbco_felix.c_str()));
}
