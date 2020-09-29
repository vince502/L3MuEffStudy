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

void modifygraph(){
  TFile *file = new TFile("outputEffMuCent.root");
  TList *l1 = gDirectory->GetListOfKeys();
  for( auto keys : *l1 ){
    std::string ename = keys->GetName();
    std::cout << ename << std::endl;
    //TDirectory* subd = (TDirectory*) file->Get(ename.c_str());
    //subd->cd();
    //TList* ll = subd->GetListOfKeys();
    TEfficiency *eff = (TEfficiency*) file->Get(Form("%s/%s",ename.c_str(), ename.c_str()));
    eff->SetName(ename.c_str());
    eff->SetTitle(ename.c_str());
    TCanvas* ctemp = new TCanvas("ctemp", "ctemp", 1000 ,1000);
    ctemp->cd();

    eff->Draw(); gPad->Update();
    TGraphAsymmErrors* g1 = eff->GetPaintedGraph();
    ctemp->Clear();
    
    TLegend* leg = new TLegend(0.15,0.8, 0.3,0.85);
    int ent = eff->Sizeof();
 //   leg->AddEntry((TObject*)0, Form("Entries  %i",ent)  ,"");
    g1->SetMarkerStyle(8);
    g1->SetMarkerColor(2);
    g1->GetXaxis()->SetTitle("Centrality");
    g1->GetYaxis()->SetTitle("Efficiency");
    g1->Draw("AP same"); 
//    leg->Draw("same"); 
    gPad->Update();

    ctemp->Modified();
    ctemp->SaveAs(Form("pdfs/%s.pdf", ename.c_str()));
    ctemp->Close();
  }
  file->Close();
}
