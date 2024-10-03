void localQAdump(TString fname = "./qahists/53705/qahist-nEvent10000-skip0.root", int runnumber = 53705)
{
    TString outdir = TString::Format("./plots/QAdump/Run%d", runnumber);
    gSystem->Exec(Form("mkdir -p %s", outdir.Data()));

    TKey *key;
    TFile *f = TFile::Open(fname.Data(), "READ");
    if (!f || f->IsZombie())
    {
        cout << "Unable to open " << fname << " for reading..." << endl;
        return;
    }

    TCanvas *c = new TCanvas("c", "c", 800, 700);
    
    TIter next((TList *)f->GetListOfKeys());
    while (key = (TKey *)next())
    {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (cl->InheritsFrom("TH1F") || cl->InheritsFrom("TH1D"))
        {
            gPad->SetRightMargin(0.05);
            TH1 *h = (TH1 *)key->ReadObj();
            cout << "Histo found: " << h->GetName() << " - " << h->GetTitle() << endl;
            c->cd();
            h->Draw("hist");
            c->SaveAs(Form("%s/%s.png", outdir.Data(), h->GetName()));
            c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h->GetName()));
        }
        else if (cl->InheritsFrom("TH2"))
        {
            gPad->SetRightMargin(0.17);
            TH2 *h = (TH2 *)key->ReadObj();
            cout << "Histo found: " << h->GetName() << " - " << h->GetTitle() << endl;
            c->cd();
            h->Draw("colz");
            c->SaveAs(Form("%s/%s.png", outdir.Data(), h->GetName()));
            c->SaveAs(Form("%s/%s.pdf", outdir.Data(), h->GetName()));
        }
    }

    f->Close();
    delete c;
    delete f;
}