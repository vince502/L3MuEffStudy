//#include "CMS_lumi.C"
//#include "tdrstyle.C"
#include "TROOT.h"
#include "TCanvas.h"
#include "TBranch.h"
#include "TClass.h"
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

void Drawhist(){
  const int Max_mu_size = 10000;
  std::string fname = "MBL3.root";
  TFile* f = new TFile(fname.c_str());
  TTree* t1 = TTreeReader("L3Track").GetTree();
  TObjArray* blist = t1->GetListOfBranches();
  t1->SetBranchStatus("*",0);
  TCanvas* c1 = new TCanvas("c1", "EDProducer Pt Collection", 1000, 800);
  //auto legend = new TLegend(160,10, 180,1000);
  long long nEntries = t1->GetEntries();
  int* Event;
  int* Run;
  t1->SetBranchAddress("Event", &Event);
  t1->SetBranchAddress("Run", &Run);
  std::cout << "Set Br address: Evt, Run load Entries" << std::endl;
  for(int brent=0; brent<13; brent++){
    
    TBranch* br = (TBranch*) blist->At(brent+2);
    std::string pname = t1->GetListOfBranches()->At(brent+2)->GetName(); 
    TObject* obj;;
    std::cout << pname << std::endl;
    t1->SetBranchStatus(pname.c_str(), 1);
    t1->SetBranchAddress( pname.c_str(), &obj, &br);
//    br->SetAddress(&obj);
    std::cout << "Set Br address: Evt, Run load Entries" << std::endl;
    TH1D* hist = new TH1D("hist", pname.c_str(), 20,0,80); 
//    legend->AddEntry(hist, pname.c_str(), "l");
    std::cout << "Reading Branch: " << pname << " with entries " << nEntries << std::endl; 

    for(long long j = 0 ; j <nEntries; j++){
      std::cout << "Tree: " << t1->GetName() << std::endl;
      t1->GetEntry(j);
      std::cout << "really;;;" << std::endl;;
      TObjArray* TC = (TObjArray*) obj;
      long long cEntries = TC->GetSize();

 //     std::cout << "Load Entries" << std::endl;
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
//    legend->Draw();
    }

  c1->SetLogy();
  c1->Print("Result.pdf"); 
  c1->Close();
 
}
