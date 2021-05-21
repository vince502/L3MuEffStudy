#include <TEfficiency.h>
#include <TStyle.h>
#include <TFile.h>
#include <vector>
#include <string>
#include <TString.h>
#include <TTree.h>
#include <TROOT.h>
#include <TH2D.h>
#include <TLatex.h>
#include <TMarker.h>
#include <TLegend.h>
#include <TList.h>
#include "Cent_plotTurnOn.h"
#include "util.h"

void accDrawer(TLine lin){

/*  lin.SetLineColor(kRed);

  lin.SetLineWidth(3);
  lin.DrawLine(1.2, -2.4, 1.2, -2.2);
  lin.DrawLine(1.2, 2.2, 1.2, 2.4);
  lin.DrawLine(1.2, -2.2, 2.1, -1.55);
  lin.DrawLine(1.2, 2.2, 2.1, 1.55);
  lin.DrawLine(2.1, 1.55, 2.1, 1.4);
  lin.DrawLine(2.1, -1.55, 2.1, -1.4);
  lin.DrawLine(3.3, -1.10, 2.1, -1.4);
  lin.DrawLine(3.3, 1.10, 2.1, 1.4);
  lin.DrawLine(3.3, 1.10, 3.4, 0.3);
  lin.DrawLine(3.3, -1.10, 3.4, -0.3);
  lin.DrawLine(3.4, 0.30, 3.4, -0.3);

*/
};

void modify_2DRatio_graph(std::string jobname = "L3_2021May_113X_Pt_3_100_L2_PtEta_MT_PGvsONIA_Pt_3_100"){
  TFile* f1 = new TFile(Form("./outputratioL3_%s.root",jobname.c_str()), "open");
  TFile* f2 = new TFile(Form("./2DEffplots_%s.root",jobname.c_str()), "recreate");

  f1->cd();
  TList* l1 = gDirectory->GetListOfKeys();
  for( auto keys : *l1){
    TH1::SetDefaultSumw2;
    std::string ename = keys->GetName();
    int index_key;
    for( auto pdkey : ProdList){
      if (ename.find(pdkey.second.first.c_str())!= std::string::npos) index_key = pdkey.first;
    }
    gStyle->SetOptStat(kFALSE);
    gStyle->SetOptTitle(0);
    TH2D* hol = (TH2D*) f1->Get(Form("%s/%s_efficiency_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hfl = (TH2D*) f1->Get(Form("%s/%s_online_fake_ratio_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hoh = (TH2D*) f1->Get(Form("%s/%s_efficiency_70-100" , ename.c_str(),ename.c_str()));
    TH2D* hfh = (TH2D*) f1->Get(Form("%s/%s_online_fake_ratio_70-100" , ename.c_str(),ename.c_str()));
    TH2D* hool = (TH2D*) f1->Get(Form("%s/%s_online_matched_reco_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hffl = (TH2D*) f1->Get(Form("%s/%s_reco_notmatched_online_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hrecl = (TH2D*) f1->Get(Form("%s/%s_reco_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hrofl = (TH2D*) f1->Get(Form("%s/%s_online_0-10" , ename.c_str(),ename.c_str()));
    TH2D* hooh = (TH2D*) f1->Get(Form("%s/%s_online_matched_reco_70-100" , ename.c_str(),ename.c_str()));
    TH2D* hffh = (TH2D*) f1->Get(Form("%s/%s_reco_notmatched_online_70-100" , ename.c_str(),ename.c_str()));
    TH2D* hrech = (TH2D*) f1->Get(Form("%s/%s_reco_70-100" , ename.c_str(),ename.c_str()));
    TH2D* hrofh = (TH2D*) f1->Get(Form("%s/%s_online_70-100" , ename.c_str(),ename.c_str()));
    
    auto onEl = hool->GetEntries();
    auto onfEl = hffl->GetEntries();
    auto reEl = hrecl->GetEntries();
    auto onEh = hooh->GetEntries();
    auto onfEh = hffh->GetEntries();
    auto reEh = hrech->GetEntries();
    double Xbin = (double) hol->GetNbinsX();
    double Ybin = (double) hol->GetNbinsY();

//    TCanvas* c1 = new TCanvas(Form("%s_canvas",ename.c_str()),Form("Efficiency & Ratio plot %s",ename.c_str()),1500,1000);
    TCanvas* c2 = new TCanvas(Form("%s_online_canvas_0-10",ename.c_str()),Form("Efficiency plot %s_0-10",ename.c_str()),1500,1000);
    TCanvas* c3 = new TCanvas(Form("%s_online_fake_canvas_0-10",ename.c_str()),Form("Fake Ratio plot %s_0-10",ename.c_str()),1500,1000);
    TCanvas* c4 = new TCanvas(Form("%s_online_canvas_70-100",ename.c_str()),Form("Efficiency plot %s_70-100",ename.c_str()),1500,1000);
    TCanvas* c5 = new TCanvas(Form("%s_online_fake_canvas_70-100",ename.c_str()),Form("Fake Ratio plot %s_70-100",ename.c_str()),1500,1000);
/////////////////////////////////////////////////////////////////////////////////////////////////
    gStyle->SetPalette(1);
    TLatex *OE = new TLatex(); 
    OE->SetTextSize(0.02);
    TLatex *OFE = new TLatex(); 
    OFE->SetTextSize(0.02);
    TLatex *RE = new TLatex();
    RE->SetTextSize(0.02);
    OE->SetNDC();
    OFE->SetNDC();
    RE->SetNDC();
    TLatex *TT = new TLatex();
    TT->SetTextSize(0.02);
    TT->SetNDC();
    const std::string plotDir =Form("plot/%s/%d_%s", jobname.c_str(),index_key,ename.c_str());
    makeDir(plotDir+ "/pdf/two/");
    makeDir(plotDir+ "/pdf/online/");
    makeDir(plotDir+ "/pdf/fake/");
    TLine l;
//    c1->Write();
/////////////////////////////////////////////////////////////////////////////////////////////////
    c2->cd();
    hol->GetXaxis()->SetTitle("Pt");
    hol->GetYaxis()->SetTitle("Eta");
    hol->Draw("colz");
    auto legend2 = new TLegend(0.7, 0.8,0.9,0.9);
    legend2->AddEntry(hol,"Online Matched RECO","");
    legend2->Draw();
    accDrawer(l);
    OE->DrawLatex(0.7,0.76,Form("Online = %i",(int)onEl));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reEl));
    TT->DrawLatex(0.1,0.93,Form("Efficiency Plot %s 0-10",ename.c_str()));
    hol->SetStats(0);
    gPad->Update();
    c2->Draw();
    c2->Modified();
    c2->SaveAs((plotDir+Form("/pdf/online/%d_online_", index_key)+jobname+ "_" + ename+"_0-10.pdf").c_str());
    c2->SaveAs((plotDir+Form("/pdf/online/%d_online_", index_key)+jobname+ "_" + ename+"_0-10.png").c_str());
    c2->Clear();

    c2->SetCanvasSize(2200,1000);
    c2->Divide(2,1);
    TT->DrawLatex(0.1,0.93,Form("Efficiency Plot %s 0-10",ename.c_str()));
    TH1D* projholptnum = hool->ProjectionX("prjholptnum",0,-1,"e");
    TH1D* projholptden = hrecl->ProjectionX("prjholptden",0,-1,"e");
    TH1D* projholetanum = hool->ProjectionY("prjholetanum",0,-1,"e");
    TH1D* projholetaden = hrecl->ProjectionY("prjholetaden",0,-1,"e");
    TH1D* projholpt = (TH1D*) projholptnum->Clone("projholpt"); 
    TH1D* projholeta = (TH1D*) projholetanum->Clone("projholeta"); 
    projholpt->Divide(projholptden);
    projholeta->Divide(projholetaden);
    projholpt->Scale(1.0);
    projholeta->Scale(1.0);
    projholpt->GetYaxis()->SetTitle("Eff");
    projholpt->GetXaxis()->SetTitle("Pt");
    projholeta->GetXaxis()->SetTitle("Eta");
    c2->cd(1);
    projholpt->Draw("e");
    c2->cd(2);
    projholeta->Draw("e");
    c2->SaveAs((plotDir+Form("/pdf/online/%d_online_",index_key)+jobname+"_"+ename+"_0-10_pteta.pdf").c_str());
    c2->Clear();

/////////////////////////////////////////////////////////////////////////////////////////////////
    c3->cd();
    hfl->GetXaxis()->SetTitle("Pt");
    hfl->GetYaxis()->SetTitle("Eta");
    hfl->Draw("colz");
    auto legend3 = new TLegend(0.7, 0.8,0.9,0.9);
    legend3->AddEntry(hfl,"Online FAKE","");
    legend3->Draw();
    accDrawer(l);
    OFE->DrawLatex(0.7,0.73,Form("Fake = %i",(int)onfEl));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reEl));
    TT->DrawLatex(0.1,0.93,Form("Ratio Plot %s 0-10",ename.c_str()));
    hfl->SetStats(0);
    c3->Draw();

    c3->Modified();
    c3->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+ "_" + ename+"_0-10.pdf").c_str());
    c3->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+ "_" + ename+"_0-10.png").c_str());
    c3->Clear();

    c3->SetCanvasSize(2200,1000);
    c3->Divide(2,1);
    TT->DrawLatex(0.1,0.93,Form("Ratio Plot %s 0-10",ename.c_str()));
    TH1D* projhflptnum = hffl->ProjectionX("prjhflptnum",0,-1,"e");
    TH1D* projhflptden = hrofl->ProjectionX("prjhflptden",0,-1,"e");
    TH1D* projhfletanum = hffl->ProjectionY("prjhfletanum",0,-1,"e");
    TH1D* projhfletaden = hrofl->ProjectionY("prjhfletaden",0,-1,"e");
    TH1D* projhflpt = (TH1D*) projhflptnum->Clone("projhflpt"); 
    TH1D* projhfleta = (TH1D*) projhfletanum->Clone("projhfleta"); 
    projhflpt->Divide(projhflptden);
    projhfleta->Divide(projhfletaden);
    projhflpt->Scale(1.0);
    projhfleta->Scale(1.0);
    projhflpt->GetYaxis()->SetTitle("Eff");
    projhflpt->GetXaxis()->SetTitle("Pt");
    projhfleta->GetXaxis()->SetTitle("Eta");
    c3->cd(1);
    projhflpt->Draw("e");
    c3->cd(2);
    projhfleta->Draw("e");
    c3->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+"_"+ename+"_0-10_pteta.pdf").c_str());
    c3->Clear();

/////////////////////////////////////////////////////////////////////////////////////////////////
    c4->cd();
    hoh->GetXaxis()->SetTitle("Pt");
    hoh->GetYaxis()->SetTitle("Eta");
    hoh->Draw("colz");
    auto legend4 = new TLegend(0.7, 0.8,0.9,0.9);
    legend4->AddEntry(hoh,"Online Matched RECO","");
    legend4->Draw();
    accDrawer(l);
    OE->DrawLatex(0.7,0.76,Form("Online = %i",(int)onEh));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reEh));
    TT->DrawLatex(0.1,0.93,Form("Efficiency Plot %s 70-100",ename.c_str()));
    hoh->SetStats(0);
    gPad->Update();
    c4->Draw();

    c4->Modified();
    c4->SaveAs((plotDir+Form("/pdf/online/%d_online_",index_key)+jobname+ "_" + ename+"_70-100.pdf").c_str());
    c4->SaveAs((plotDir+Form("/pdf/online/%d_online_",index_key)+jobname+ "_" + ename+"_70-100.png").c_str());
    c4->Clear();

    c4->SetCanvasSize(2200,1000);
    c4->Divide(2,1);
    TT->DrawLatex(0.1,0.93,Form("Efficiency Plot %s 70-100",ename.c_str()));
    TH1D* projhohptnum = hooh->ProjectionX("prjhohptnum",0,-1,"e");
    TH1D* projhohptden = hrech->ProjectionX("prjhohptden",0,-1,"e");
    TH1D* projhohetanum = hooh->ProjectionY("prjhohetanum",0,-1,"e");
    TH1D* projhohetaden = hrech->ProjectionY("prjhohetaden",0,-1,"e");
    TH1D* projhohpt = (TH1D*) projhohptnum->Clone("projhohpt"); 
    TH1D* projhoheta = (TH1D*) projhohetanum->Clone("projhoheta"); 
    projhohpt->Divide(projhohptden);
    projhoheta->Divide(projhohetaden);
    projhohpt->Scale(1.0);
    projhoheta->Scale(1.0);
    projhohpt->GetYaxis()->SetTitle("Eff");
    projhohpt->GetXaxis()->SetTitle("Pt");
    projhoheta->GetXaxis()->SetTitle("Eta");
    c4->cd(1);
    projhohpt->Draw("e");
    c4->cd(2);
    projhoheta->Draw("e");
    c4->SaveAs((plotDir+Form("/pdf/online/%d_online_",index_key)+jobname+"_"+ename+"_70-100_pteta.pdf").c_str());
    c4->Clear();

/////////////////////////////////////////////////////////////////////////////////////////////////
    c5->cd();
    hfh->GetXaxis()->SetTitle("Pt");
    hfh->GetYaxis()->SetTitle("Eta");
    hfh->Draw("colz");
    auto legend5 = new TLegend(0.7, 0.8,0.9,0.9);
    legend5->AddEntry(hfh,"Online FAKE","");
    legend5->Draw();
    OFE->DrawLatex(0.7,0.73,Form("Fake = %i",(int)onfEh));
    RE->DrawLatex(0.7,0.7,Form("Reco = %i",(int)reEh));
    TT->DrawLatex(0.1,0.93,Form("Ratio Plot %s 70-100",ename.c_str()));
    hfh->SetStats(0);
    c5->Draw();
    c5->Modified();
    c5->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+ "_" + ename+"_70-100.pdf").c_str());
    c5->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+ "_" + ename+"_70-100.png").c_str());
    c5->Clear();

    c5->SetCanvasSize(2200,1000);
    c5->Divide(2,1);
    TH1D* projhfhptnum = hffh->ProjectionX("prjhfhptnum",0,-1,"e");
    TH1D* projhfhptden = hrofh->ProjectionX("prjhfhptden",0,-1,"e");
    TH1D* projhfhetanum = hffh->ProjectionY("prjhfhetanum",0,-1,"e");
    TH1D* projhfhetaden = hrofh->ProjectionY("prjhfhetaden",0,-1,"e");
    TH1D* projhfhpt = (TH1D*) projhfhptnum->Clone("projhfhpt"); 
    TH1D* projhfheta = (TH1D*) projhfhetanum->Clone("projhfheta"); 
    projhfhpt->Divide(projhfhptden);
    projhfheta->Divide(projhfhetaden);
    projhfhpt->Scale(1.0);
    projhfheta->Scale(1.0);
    projhfhpt->GetYaxis()->SetTitle("Eff");
    projhfhpt->GetXaxis()->SetTitle("Pt");
    projhfheta->GetXaxis()->SetTitle("Eta");
    c5->cd(1);
    projhfhpt->Draw("e");
    c5->cd(2);
    projhfheta->Draw("e");
    c5->SaveAs((plotDir+Form("/pdf/fake/%d_fake_",index_key)+jobname+"_"+ename+"_70-100_pteta.pdf").c_str());
    c5->Clear();
  }
  f1->Close();
  f2->Close();
}


