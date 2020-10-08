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

void matchEvt_map(){
  std::string of = "L3_crabbed_1178_wL2fix_part_1";
  std::string reco = "./Large_Files/Forest_HIMinimumBias2_run327237_merged.root";
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile(("Large_Files/"+of+".root").c_str(),"open");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  t1->SetBranchStatus("*", 0);
  t1->SetBranchStatus("Event", 1);
  long long  oEntries;
  Int_t oEvt;
  oEntries = t1->GetEntries();
  t1->SetBranchAddress("Event", &oEvt);

  std::map<long long, std::pair<int, long long> > Oevlist;
  std::map<long long, std::pair<int, long long> > Revlist;
  
  auto maxevt = std::max(oEntries, nEntries);
  for(int iEntry = 0 ; iEntry < maxevt; iEntry ++){
    if((iEntry%10000)==0) {std::cout << "Running Entry: " << iEntry << "\r"; std::cout.flush();}
    recoInfo.setEntry(iEntry, false, true);
    std::pair<Int_t, long long> evtInfo =  recoInfo.getEventNumber();
    long long recoEvt = evtInfo.second;
    t1->GetEntry(iEntry);
    if(iEntry > oEntries){oEvt = -1;}
    std::pair<int, long long> p1 = std::make_pair(iEntry, recoEvt);
    std::pair<int, long long>p2 = std::make_pair(iEntry, oEvt);
    Oevlist.insert(std::pair<long long, std::pair<int, long long> >(oEvt, p1));
    Revlist.insert(std::pair<long long, std::pair<int, long long> >(recoEvt, p2));
    }
    std::cout.flush();
    std::cout << "Done buff" << std::endl;
    std::vector<int> index;
    for(int j=0; j<maxevt; j++){
      index.push_back(j);
    }

    l3t->Close();
    TStopwatch* clock = new TStopwatch();
    Double_t tbuf, trate;
    std::map<int, int>idxlist;
    int count=0;
    for( std::map<long long, std::pair<int, long long> >::iterator it= Oevlist.begin(); it !=Oevlist.end() ; it++){
      count++;

      long long evtb = it->first;
      int oidx = it->second.first;
      auto ff = Revlist.find(evtb);
      int ridx = ff->second.first;
      idxlist.insert(std::pair<int, int>(oidx,ridx));
      if((count%10000)==0){std::cout<< "Done job: " << count << ", online: " << evtb << ", reco: " << ff->first << "\r";std::cout.flush();}
    }
    std::cout.flush();
    std::cout<< "Done Matching" <<std::endl;
    
    std::ofstream out;
    out.open(Form("realEvtpair_%s.txt",of.c_str()));
    int count2=0;
    for(std::map<int, int>::iterator itt = idxlist.begin(); itt != idxlist.end(); itt++){
      count2++;
      out << itt->first <<" "<< itt->second << std::endl;
      if((count2%10000)==0){std::cout << "Written " << count2 << " events" <<"\r";}
    }
    std::cout.flush();
    std::cout<< "JOB Done" << std::endl;
    out.close(); 
}
