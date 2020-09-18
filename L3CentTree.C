#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TTree.h>
#include <string>
#include "Cent_plotTurnOn.h"

void L3CentTree(){

  int centI=300;
  int Event, Run;
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";

  RecoReader recoInfo(reco, false);
  TFile* l3t = new TFile("MBL3.root", "update");
  TTree* t1 =(TTree*) l3t->Get("l3pAnalyzer/L3Track");

  TBranch *hibin = t1->Branch("hiBin", &centI, "hiBin/I");
  t1->SetBranchAddress("Event",&Event);
  t1->SetBranchAddress("Run", &Run);
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track", l3t);

  int count2 = 0;
  int startE = 848000;
  const auto nEntries = recoInfo.getEntries();
  long long count = 0 ;
  Long64_t nentries = t1->GetEntries();

  std::cout << "Iterating: " << nentries << " events" << std::endl;
	for(Long64_t j = 0 ; j <nentries; j++){
          t1->GetEntry(j);
          for (int iEntry=startE; iEntry <857000; ++iEntry){
          //  if ((iEntry%1000)==0){std::cout << "Processing event " << iEntry << " / " << nEntries << std::endl;}
            recoInfo.setEntry(iEntry, false, true);
            std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();

	    if( evtInfo.first == Run && evtInfo.second == Event ){

	      std::cout<< "Found hibin"<<", in Event(L3/reco): " << Event<<" / " << evtInfo.second  << std::endl;
	    

              const auto bObj = recoInfo.getCentrality();
              centI = (int) bObj;
	      hibin->Fill();
	      count2++;
	      if ((count2%1000)==0){std::cout << "count: " << count2 << std::endl;}
	      std::cout << iEntry << std::endl;

            break;
            }
	    centI= 300;

          }

    }
    std::cout << " Done Events: " << count2 << std::endl;
    l3t->cd();
    t1->Write();
    r1.Delete();
    l3t->Close();

}
