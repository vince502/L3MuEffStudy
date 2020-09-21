//#include "CMS_lumi.C"
//#include "tdrstyle.C"
#include "TROOT.h"
#include "TCanvas.h"
#include "TBranch.h"
//#include "TClass.h"
#include "TFile.h"
#include "TTree.h"
#include <iostream>
#include <map>
#include <string>
#include "TString.h"
#include "TLatex.h"
#include "TH1D.h"
#include "TObject.h"
#include "TObjArray.h"
#include <memory>

void DrawhistCent(){
  const int Max_mu_size = 10000;
  std::string fname = "MBL3.root";
  TFile* f = new TFile(fname.c_str());
  TTreeReader r1("L3Track");
  TTree* t1 = TTreeReader("L3Track").GetTree();
//  TTree* t2 = t1->CloneTree(0);
  TObjArray* blist = t1->GetListOfBranches();
  TCanvas* c1 = new TCanvas("c1", "EDProducer Pt Collection",1);
  auto legend = new TLegend(0.6,0.5,1.0,1.0);
  legend->SetBorderSize(0);
  long long nEntries = t1->GetEntries();
  Int_t Event;
  Int_t Run;
  Int_t centI;
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);
  t1->SetBranchAddress("Event", &Event);
  t1->SetBranchAddress("Run", &Run);
  t1->SetBranchAddress("hiBin", &centI);


  std::cout << "Set Br address: Evt, Run load Entries" << std::endl;
  for(Int_t brent=0; brent<13; brent++){    
    TBranch* br = (TBranch*) blist->At(brent+2);
     std::string pname = br->GetName(); 
    t1->SetBranchAddress(pname.c_str(), &TC);
    TH1D* hist = new TH1D("hist", pname.c_str(), 20,0,40); 
    hist->SetStats(0000);
    legend->AddEntry(hist, pname.c_str(), "pl");
    std::cout << "Reading Branch: " << pname << " with entries " << nEntries << std::endl; 

    for(long long j = 0 ; j <nEntries; j++){
      t1->GetEntry(j);
      int cent = centI/2;
      if(cent > 10 && cent < 60){int cEntries = TC->GetSize();
      if(cEntries>0){for(int k =0; k < cEntries; k++){
	/*if(k==0){std::cout << "Start filling" << std::endl;
        std::cout << pname << ": Entry Size -> " << cEntries << "at Event"   << std::endl;
	}*/
        TLorentzVector* v1 = (TLorentzVector*) TC->At(k);
        double pt = v1->Pt();
        double eta = v1->Eta();
  //      double phi = v1->Phi();
        hist->Fill(pt);
      }}
      }
    }
    hist->GetYaxis()->SetRange(0,100000);
    hist->GetXaxis()->SetTitle("Pt");
    hist->GetYaxis()->SetTitle("Counts");
//    hist->GetYaxis()->SetRangeUser(0,1e+5);

    hist->SetLineColor(brent+30);
    TAttMarker tm;
    tm.SetMarkerStyle(brent+50);
    hist->SetMarkerStyle(tm.GetMarkerStyle());
    hist->Draw("C P SAME");
    hist->Clear();
    legend->Draw();
    c1->Update();
    }

  c1->SetLogy();
  c1->Modified();
  c1->Print("Result_Cent10-60.pdf"); 
//  c1->Close();
//  f->Close(); 
}
