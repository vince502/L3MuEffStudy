#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TTree.h>
#include <string>
#include "../TriggerStudy/RunPreparation/TriggerStudy/Cent_plotTurnOn.h"

void L3CentTree_v2(){

  TFile* l3t = new TFile("5v_L3.root", "update");
//  TFile* resl3 = new TFile("L3_result.root", "recreate");

  //l3t->cd("l3pAnalyzer");
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";

  RecoReader recoInfo(reco, false);
  TTree* t1 =(TTree*) l3t->Get("l3pAnalyzer/L3Track");
//  TTree t1 = TTree("L3Track","l3pAnalyzer/L3Track");
  
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track", l3t);
  TTreeReaderValue<int> Run = {r1, "Run"};
  TTreeReaderValue<int> Event = {r1, "Event"};
  int centI;
  t1->Branch("hiBin", &centI, "hiBin/I");

  const int nCores = 8;
  ROOT::EnableImplicitMT();
  ROOT::TProcessExecutor mpe(nCores);
  
  const auto nEntries = recoInfo.getEntries();
  auto findCent = [=] (int idx){
    

    long long count = 0 ;
    for (const auto& iEntry : ROOT::TSeqUL(nEntries)) {
      count++;
      recoInfo.setEntry(iEntry, false, true);

      std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();
      if(evtInfo.second == 152659048 ){std::cout<< "test"<< std::endl;}   
      //std::cout << "Run: "<< evtInfo.first << " Event: " << evtInfo.second << std::endl;

      //r1.Restart();
      //std::cout <<*Run << std::endl;
      while(r1.Next()){
      //for(TTreeReader::Iterator_t idx = r1.begin(); idx != r1.end(); ++idx){
//      std::cout<< "Reading run: " << *Run << " event: " << *Event <<std::endl;
	if( evtInfo.first == *Run && evtInfo.second == *Event ){
	  std::cout<< "Found hibin" << std::endl;
          const auto bObj = recoInfo.getCentrality();
          centI = (int) bObj;
     	  t1->Fill();
        }
      }
      r1.Restart();
    }
  }
    std::cout <<"Number of event: "<< count << std::endl;
    r1.Delete();
    t1->SetTitle("updated_track");
    l3t->Write();
    l3t->Close();

}
