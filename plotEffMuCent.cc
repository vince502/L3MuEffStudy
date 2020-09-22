#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TTree.h>
#include <string>
#include "Cent_plotTurnOn.h"

void makeDirFile(TFile *f1, const std::string& dir)
{
  TDirectory* subdir = f1->mkdir(dir.c_str());
};

void plotEffMuCent(){
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";
  
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile("1v_MBL3.root","open");
  TTree* t1 = (TTree*) l3t->Get("L3Track");
  TTreeReader r1 = TTreeReader("L3Track",l3t);
  TObjArray* blist = t1->GetListOfBranches();
  Long64_t onentries = t1->GetEntries();
  static const int Max_mu_size = 10000;
  int Event, Run, centI;
  t1->SetBranchAddress("Event", &Event);
  t1->SetBranchAddress("Run", &Run);
  t1->SetBranchAddress("hiBin", &centI);
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);

  std::vector<TEfficiency> effVec;
  std::vector<std::string> effMapTitle;
  for(int i = 0; i < 13; i++){
    TBranch* br = (TBranch*) blist->At(i+2);
    std::string pname = br->GetName();
    effVec.emplace_back(TEfficiency("",("Efficiency_of_Centrality" + pname).c_str(), 20, 0, 100));
    effMapTitle.emplace_back(("Efficiency_of_Centrality" + pname).c_str());
  }

  std::cout << "Done Init EffVecs" << std::endl;
  recoInfo.initBranches("muon");
  for(int i=0; i< 13; i++){
    TBranch* br = (TBranch*) blist->At(i+2);
    std::string pname = br->GetName();
    t1->SetBranchAddress( pname.c_str(), &TC);
    for(Long64_t jEntry =0 ; jEntry<onentries; jEntry++){
      t1->GetEntry(jEntry);

      for(int iEntry= 848000; iEntry<857000; ++iEntry){
        recoInfo.setEntry(iEntry, false, true);
        std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();
      
        if( iEntry==848000 && (jEntry%1000)==0){std::cout << jEntry << " / "<< pname << std::endl;}
        if(evtInfo.first == Run && evtInfo.second == Event){
          const auto particles = recoInfo.getParticles("muon");
          int cEntries = TC->GetSize();

          if(cEntries>0){
	    for(int k = 0 ; k < cEntries; k++){
      	      TLorentzVector* onv = (TLorentzVector*) TC->At(k);
    	      double opt = onv->Pt();
              double oeta = onv->Eta();
    	      double ophi = onv->Phi();
    	      for( auto recM : particles ){
	        double rpt = recM.first.Pt();
	        double reta = recM.first.Eta();
	        double rphi = recM.first.Phi();
    	        double dR = std::sqrt((oeta-reta)*(oeta-reta)+(ophi-rphi)*(ophi-rphi));
    	        bool isMatched = (dR < 0.1) ? true: false;
    	        effVec[i].Fill(isMatched, centI);
	      }
            }
	  }
        } 
      }
    }
  }
  std::cout << "Done Filling" << std::endl;
  TFile* file = new TFile("outputEffMuCent.root","recreate");
  int tt= 0;
  std::vector<TGraphAsymmErrors> graphM;
  for(auto& Eff : effVec){
    TCanvas c("c", "c", 1000, 1000); c.cd();
    Eff.Draw(); gPad->Update();
    auto name = effMapTitle[tt];
    std::cout << effMapTitle[tt] << std::endl;
    makeDirFile(file,name);
    TDirectory* subdir_ = file->GetDirectory(name.c_str());
    tt++;
    subdir_->cd();
    c.Write();
    Eff.Write();
    c.Close();
  }
  l3t->Close();
  file->Close();

}
