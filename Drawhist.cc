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

void Drawhist(){
  const int Max_mu_size = 10000;
  std::string fname = "MBL3.root";
  TFile* f = new TFile(fname.c_str());
  TTreeReader r1("L3Track");
  TTree* t1 = TTreeReader("L3Track").GetTree();
//  TTree* t2 = t1->CloneTree(0);
  TObjArray* blist = t1->GetListOfBranches();
  TCanvas* c1 = new TCanvas("c1", "EDProducer Pt Collection", 1000, 800);
  auto legend = new TLegend(160,10, 180,1000);
  long long nEntries = t1->GetEntries();
  Int_t Event;
  Int_t Run;
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);
  t1->SetBranchAddress("Event", &Event);
  t1->SetBranchAddress("Run", &Run);


  std::cout << "Set Br address: Evt, Run load Entries" << std::endl;
  for(Int_t brent=0; brent<13; brent++){    
    TBranch* br = (TBranch*) blist->At(brent+2);
     std::string pname = br->GetName(); 
    t1->SetBranchAddress(pname.c_str(), &TC);
    TH1D* hist = new TH1D("hist", pname.c_str(), 20,0,80); 
    legend->AddEntry(hist, pname.c_str(), "l");
    std::cout << "Reading Branch: " << pname << " with entries " << nEntries << std::endl; 

    for(long long j = 0 ; j <nEntries; j++){
      t1->GetEntry(j);
      int cEntries = TC->GetSize();
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
    hist->SetLineColor(brent+30);
    hist->SetMarkerStyle(brent+1);
    hist->Draw("CP SAME");
    hist->Clear();
    legend->Draw();
    }

  c1->SetLogy();
  c1->Print("Result.pdf"); 
  c1->Close();
  f->Close(); 
}
