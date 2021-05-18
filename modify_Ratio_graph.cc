#include <TEfficiency.h>
#include <TStyle.h>
#include <TFile.h>
#include <vector>
#include <string>
#include <TString.h>
#include <TTree.h>
#include <TROOT.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TLegend.h>
#include <TList.h>
#include "Cent_plotTurnOn.h"

void modify_Ratio_graph(std::string jobname = "L3_2021May_113X_Pt_0p5_100L2_MT_gyeonghwan_code_changed_PGvsONIAfull"){
  TFile* f1 = new TFile(Form("./outputratioL3_%s.root",jobname.c_str()), "open");
  TFile* f2 = new TFile(Form("./Ratioplots_%s.root",jobname.c_str()), "recreate");
  f1->cd();
  TList* l1 = gDirectory->GetListOfKeys();
  for( auto keys : *l1){
    std::string ename = keys->GetName();
    std::cout << ename << std::endl;
    gStyle->SetOptStat(kFALSE);
    gStyle->SetOptTitle(0);
    TH1D* ho = (TH1D*) f1->Get(Form("%s/%s_efficiency",ename.c_str(),ename.c_str()));
    TH1D* hf = (TH1D*) f1->Get(Form("%s/%s_online_fake_ratio",ename.c_str(),ename.c_str()));
    TH1D* hoo = (TH1D*) f1->Get(Form("%s/%s_online_matched_reco",ename.c_str(),ename.c_str()));
    TH1D* hff = (TH1D*) f1->Get(Form("%s/%s_reco_notmatched_online",ename.c_str(),ename.c_str()));
    TH1D* hrec = (TH1D*) f1->Get(Form("%s/%s_reco",ename.c_str(),ename.c_str()));
    TH1D* hrof = (TH1D*) f1->Get(Form("%s/%s_online",ename.c_str(),ename.c_str()));
    
    auto onE = hoo->GetEntries();
    auto onfE = hff->GetEntries();
    auto reE = hrec->GetEntries();

    TCanvas* c1 = new TCanvas(Form("%s_canvas",ename.c_str()),Form("Efficiency & Ratio plot %s",ename.c_str()),1300,1000);
    TCanvas* c2 = new TCanvas(Form("%s_online_canvas",ename.c_str()),Form("Efficiency plot %s",ename.c_str()),1000,1000);
    TCanvas* c3 = new TCanvas(Form("%s_online_fake_canvas",ename.c_str()),Form("Fake Ratio plot %s",ename.c_str()),1000,1000);
    c1->cd();
    //gPad->SetLogy();
    
    ho->SetLineColor(4);

    ho->SetLineStyle(1);

    ho->SetMarkerStyle(105);

    ho->SetTitle("");
    ho->Draw();
    c1->Update();
    Float_t rightmax = 1.1*hf->GetMaximum();
    Float_t scale; 
    scale = (rightmax !=0 ) ? gPad->GetUymax()/rightmax : gPad->GetUymax()/1;
    std::cout << scale << std::endl;
    hf->SetLineColor(2);
    hf->SetLineStyle(2);
    hf->SetMarkerStyle(55);
    hf->GetXaxis()->SetTitle("centrality");
    hf->GetYaxis()->SetTitle("Online ratio");
    hf->SetTitle("");
    hf->Scale(scale);
    hf->Draw("same");
    TGaxis*axis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmax(),gPad->GetUymax(),0,rightmax,510,"+L");
    axis->SetLabelSize(ho->GetYaxis()->GetLabelSize());
    axis->Draw();


    c1->Update();

    TLatex *OE = new TLatex(); 
    OE->SetTextSize(0.02);
    TLatex *OFE = new TLatex(); 
    OFE->SetTextSize(0.02);
    TLatex *RE = new TLatex();
    RE->SetTextSize(0.02);
    OE->SetNDC();
    OFE->SetNDC();
    RE->SetNDC();
    OE->DrawLatex(0.7,0.76,Form("Online = %i",(int)onE));
    OFE->DrawLatex(0.7,0.73,Form("Fake = %i",(int)onfE));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reE));
    TLatex *TT = new TLatex();
    TT->SetTextSize(0.02);
    TT->SetNDC();
    TT->DrawLatex(0.1,0.93,Form("Efficiency & Ratio Plot %s",ename.c_str()));
    //hf->SetStats(0);
    //ho->SetStats(0);
    auto legend = new TLegend(0.7, 0.8,0.9,0.9);
    legend->AddEntry(ho,"Online Matched RECO","lep");
    legend->AddEntry(hf,Form("Online FAKE"), "lep");
    legend->Draw();

    gPad->Update();
    c1->Modified();
    c1->Draw();
    const std::string plotDir = "plot/"+jobname+"/"+ename;
    makeDir(plotDir+ "/pdf/two/");
    makeDir(plotDir+ "/pdf/online/");
    makeDir(plotDir+ "/pdf/fake/");
    c1->SaveAs((plotDir + "/pdf/two/" + "two_"+jobname + "_" + ename +".pdf").c_str());
    c1->Close();
/////////////////////////////////////////////////////////////////////////////////////////////////
    f2->cd();
    c1->Write();
    c2->cd();
    ho->Draw();
    auto legend2 = new TLegend(0.7, 0.8,0.9,0.9);
    legend2->AddEntry(ho,"Online Matched RECO","lep");
    legend2->Draw();
    OE->DrawLatex(0.7,0.76,Form("Online = %i",(int)onE));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reE));
    TT->DrawLatex(0.1,0.93,Form("Ratio Plot %s",ename.c_str()));
    ho->SetStats(0);
    gPad->Update();
    c2->Modified();
    c2->SaveAs((plotDir+"/pdf/online/online_"+jobname+ "_" + ename+".pdf").c_str());

    c3->cd();
    hf->Draw();
    auto legend3 = new TLegend(0.7, 0.8,0.9,0.9);
    legend3->AddEntry(hf,"Online FAKE","lep");
    legend3->Draw();
    OFE->DrawLatex(0.7,0.73,Form("Fake = %i",(int)onfE));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reE));
    TT->DrawLatex(0.1,0.93,Form("Ratio Plot %s",ename.c_str()));
    hf->SetStats(0);
    c3->Modified();
    c3->SaveAs((plotDir+"/pdf/fake/fake_"+jobname+ "_" + ename+".pdf").c_str());
  }
  f1->Close();
  f2->Close();
}
