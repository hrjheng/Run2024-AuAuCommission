// This is a macro that plots the BCO QA information produced as a part of the
// Fun4AllStreamingInputManager into summary plots showing fraction of tagged BCOs as a function of packet number
// This macro is copied and modified from the "PlotRatios.C" macro under the TrackingAnalysis/BcoMatching directory originally made by Joe Osborn
// Reference: https://indico.bnl.gov/event/24506/contributions/95349/attachments/56447/96613/BCOMatching.pdf

const int nmvtxfelix = 12;
const int nmvtxfees = 12;

void MvtxBcoQADraw(std::string filename = "./qahists/53539/qahist-streamprod-nEvent10000.root", int runnumber = 53539)
{
    std::string plotdir = "./plots/Run" + std::to_string(runnumber) + "/StreamPoolQA";
    system(Form("mkdir -p %s", plotdir.c_str()));

    TFile *file = TFile::Open(filename.c_str());
    std::cout << "runnumber is " << runnumber << std::endl;
    ostringstream name;

    TH1 *mvtxrefgl1 = (TH1 *)file->Get("h_MvtxPoolQA_RefGL1BCO");
    TH1 *mvtxallpackets = (TH1 *)file->Get("h_MvtxPoolQA_TagBCOAllFelixs"); 
    TH1 *mvtxallpacketsallfees = (TH1 *)file->Get("h_MvtxPoolQA_TagBCOAllFelixsAllFees"); 
    // "tagged" means the absolute value of the difference between the reference BCO and the gtm BCO is less than 3; https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/offline/framework/fun4allraw/Fun4AllStreamingInputManager.cc#L910-L948

    const int nMvtxGL1BCOs = mvtxrefgl1->GetEntries();
    TH1 *mvtxTagFelix[nmvtxfelix][nmvtxfees];
    int nMvtxTagFelix[nmvtxfelix][nmvtxfees];
    float mvtxTagFelixFrac[nmvtxfelix][nmvtxfees];
    float mvtxTagFelixx[nmvtxfelix];
    float mvtxTagFelixxFrac[nmvtxfelix];
    float mvtxTagAllFelixFrac[nmvtxfelix];
    float mvtxFelixPacket[nmvtxfees];
    TGraph *mvtxFelixFracGr[nmvtxfelix];
    for (int i = 0; i < nmvtxfelix; i++)
    {
        name.str("");
        name << "h_MvtxPoolQA_TagBCO_felix" << i;
        mvtxTagFelixx[i] = ((TH1 *)file->Get(name.str().c_str()))->GetEntries(); // number of entries in each FELIX endpoint -> how many times the FELIX endpoint is tagged |ref BCO - gtm BCO| < 3
        mvtxTagFelixxFrac[i] = mvtxTagFelixx[i] / nMvtxGL1BCOs;

        name.str("");
        name << "h_MvtxPoolQA_TagBCOAllFees_Felix" << i;
        mvtxTagAllFelixFrac[i] = (((TH1 *)file->Get(name.str().c_str()))->GetEntries()) / nMvtxGL1BCOs; // number of entries where all 12 FEEs in a FELIX endpoint are tagged
    }

    TH1F *mvtxsummaryhisto = new TH1F("mvtxsummaryhisto", ";Felix.Endpoint;Fraction Tagged GL1 BCOs", 13, 0, 13);
    for (int i = 0; i < nmvtxfelix; i++)
    {
        mvtxsummaryhisto->SetBinContent(i + 1, mvtxTagFelixxFrac[i]);
    }
    mvtxsummaryhisto->SetBinContent(13, mvtxallpackets->GetEntries() / nMvtxGL1BCOs);

    TH1F *mvtxAllFelixSummary = new TH1F("mvtxfelixsummaryhisto", ";Felix.Endpoint; Fraction All FEEs Tagged GL1 BCOs", 13, 0, 13);
    mvtxAllFelixSummary->GetYaxis()->SetRangeUser(0.8, 1.2);
    for (int i = 0; i < nmvtxfelix; i++)
    {
        mvtxAllFelixSummary->SetBinContent(i + 1, mvtxTagAllFelixFrac[i]);
    }
    mvtxAllFelixSummary->SetBinContent(13, mvtxallpacketsallfees->GetEntries() / nMvtxGL1BCOs);

    TCanvas *mvtxfeesumcan = new TCanvas("mvtsfeesumcan", "mvtxfeesumcan", 700, 600);
    gStyle->SetOptStat(0);
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(1, "0.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(2, "0.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(3, "1.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(4, "1.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(5, "2.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(6, "2.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(7, "3.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(8, "3.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(9, "4.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(10, "4.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(11, "5.0");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(12, "5.1");
    mvtxAllFelixSummary->GetXaxis()->SetBinLabel(13, "All");
    mvtxAllFelixSummary->Draw("histtext");
    mvtxfeesumcan->SaveAs(Form("%s/mvtxfeesumcan_%d.png", plotdir.c_str(), runnumber));
    mvtxfeesumcan->SaveAs(Form("%s/mvtxfeesumcan_%d.pdf", plotdir.c_str(), runnumber));

    TCanvas *mvtxsumcan = new TCanvas("mvtxsumcan", "mvtxsumcan", 700, 600);
    gStyle->SetOptStat(0);
    mvtxsummaryhisto->GetYaxis()->SetRangeUser(0, 1.1);
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(1, "0.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(2, "0.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(3, "1.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(4, "1.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(5, "2.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(6, "2.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(7, "3.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(8, "3.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(9, "4.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(10, "4.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(11, "5.0");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(12, "5.1");
    mvtxsummaryhisto->GetXaxis()->SetBinLabel(13, "All");
    mvtxsummaryhisto->Draw("histtext");
    mvtxsumcan->SaveAs(Form("%s/mvtxsumcan_%d.png", plotdir.c_str(), runnumber));
    mvtxsumcan->SaveAs(Form("%s/mvtxsumcan_%d.pdf", plotdir.c_str(), runnumber));
}
