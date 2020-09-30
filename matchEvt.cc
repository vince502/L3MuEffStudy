#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TTree.h>
#include <string>
#include <TStopwatch.h>
#include "Cent_plotTurnOn.h"

void matchEvt(){
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile("Large_Files/L3_crabbed_1131.root","open");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  t1->SetBranchStatus("*", 0);
  t1->SetBranchStatus("Event", 1);
  long long  oEntries;
  Int_t oEvt;
  oEntries = t1->GetEntries();
  t1->SetBranchAddress("Event", &oEvt);
  
  std::ofstream out;
  out.open("realEventpair.txt");
  std::vector<std::pair<int , std::pair<Int_t, long long> > > evlist;

  auto maxevt = std::max(oEntries, nEntries);

  for( int iEntry = 0; iEntry < maxevt; iEntry++){
    if((iEntry%10000)==0) {std::cout << "Running Entry: " << iEntry << "\r"; std::cout.flush();}
    if(oEntries < nEntries +1){
      recoInfo.setEntry(iEntry, false, true);
      std::pair<Int_t, long long> evtInfo = recoInfo.getEventNumber();
      long long recoEvt = evtInfo.second;
      t1->GetEntry(iEntry);
      if(iEntry > oEntries){oEvt = -1;}
      auto p1 = std::make_pair(oEvt, recoEvt); 
      auto p2 = std::make_pair(iEntry, p1);
      evlist.push_back(p2);
//      out << iEntry<< " " << oEvt<< " " << recoEvt<< std::endl;
    }
    else if(oEntries > nEntries){std::cout << "Not all online event Matches RECO!!" << std::endl;}
  }
  std::cout << std::endl;
/*
  std::vector<std::pair<int , std::pair<Int_t, long long> > > sevlist;
  TStopwatch* clock2 = new TStopwatch();
  clock2->Start();
  for( int iEntry = 0; iEntry < maxevt; iEntry++){
    if((iEntry%100000)==0) {std::cout << "Running Entry: " << iEntry << "\r"; std::cout.flush();}
    long long ooEvt = evlist[iEntry].second.first;
    long long rrEvt = evlist[iEntry].second.second;
    if(oEntries < nEntries +1){
      if(iEntry > oEntries){oEvt = -1;}
      auto p3 = std::make_pair(ooEvt, rrEvt);
      auto p4 = std::make_pair(iEntry, p3);
      sevlist.push_back(p4);
//      out << iEntry<< " " << oEvt<< " " << recoEvt<< std::endl;
    }
    else if(oEntries > nEntries){std::cout << "Not all online event Matches RECO!!" << std::endl;}
  }
  clock2->Stop();
  Double_t el = clock2->CpuTime();
  std::cout << el << std::endl;
*/
  //initialize Index vector
  std::vector<int> index;
  for(int j =0; j<maxevt; j++){
    index.push_back(j);
  }

  l3t->Close();
  TStopwatch* clock = new TStopwatch();
  Double_t tbuf, trate;
  std::vector<std::pair<int, int>  > revlist;
  for( int j = 0; j < maxevt; j++){
    clock->Start();
    auto p3 = evlist[j];
    auto ooo = p3.second.first;
    for(auto k : index){
      auto px = evlist[k];
      if((k%10000)==0 && (j%100)==0){std::cout << "on/reco: " << p3.first << "/" << px.first << ". Average rate: " << 1/trate << "(find/s) and going down..."<< "\r"; std::cout.flush();}
      auto rrr = px.second.second;
      if(rrr == ooo){
	auto pr = std::make_pair(p3.first,px.first);
	revlist.push_back(pr);
	index.erase(index.begin()+k);
//	out << p3.first << " "<< px.first << std::endl;
	break;
      }
    }
    clock->Stop();
    tbuf = clock->RealTime();
    trate = (trate+tbuf)/2;
    clock->Reset();
  }
  std::cout << std::endl;
  for(int j =0; j < revlist.size(); j++){
    out << revlist[j].first<<" "<<revlist[j].second<< std::endl;
  }
}

