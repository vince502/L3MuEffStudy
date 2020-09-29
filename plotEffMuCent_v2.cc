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

void Printprogress(float progress, Double_t time){
  if( progress <1.0){
    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth*progress;
    for (int i =0; i < barWidth; ++i) {
      if (i <pos) std::cout << "=";
      else if(i==pos) std::cout << ">";
      else std::cout << " ";
    }
    std::cout << "]" << (progress*100.0) << " %, eta :"<< time*(1-progress)/3600<<" hrs\r";
    std::cout.flush();
  }
  else std::cout << std::endl;
}

void makeDirFile(TFile *f1, const std::string& dir)
{
  TDirectory* subdir = f1->mkdir(dir.c_str());
};

void plotEffMuCent_v2(){
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";
  
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile("Large_Files/L3_crabbed_1131.root","open");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track",l3t);
  TObjArray* blist = t1->GetListOfBranches();
  Long64_t onentries = t1->GetEntries();
  static const int Max_mu_size = 10000;
  int Event, Run;
  t1->SetBranchAddress("Event", &Event);
  t1->SetBranchAddress("Run", &Run);
//  t1->SetBranchAddress("hiBin", &centI);
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);
  TStopwatch* clock1 = new TStopwatch();

  std::vector<TEfficiency> effVec;
  std::vector<std::string> effMapTitle;
  for(int i = 0; i < 13; i++){
    TBranch* br = (TBranch*) blist->At(i+2);
    std::string pname = br->GetName();
    t1->SetBranchAddress( pname.c_str(), &TC);
    effVec.emplace_back(TEfficiency("",("Efficiency_of_Centrality" + pname).c_str(), 20, 0, 100));
    effMapTitle.emplace_back(("Efficiency_of_Centrality" + pname).c_str());
  }
  t1->SetBranchStatus("*",0);
  std::cout << "Done Init EffVecs" << std::endl;
  recoInfo.initBranches("muon");
  
  int totalEnt = onentries;//onentries*(857000-848000);
  int progf = 0;
  Double_t cyt=0;

  for(Long64_t jEntry =0 ; jEntry<onentries; jEntry++){
    clock1->Start();
    if( (progf%1)==0) {float _progress = float(progf)/float(totalEnt); Printprogress(_progress, cyt);}
    t1->GetEntry(jEntry);

    for(int i=0; i< 13; i++){
      TBranch* br = (TBranch*) blist->At(i+2);
      std::string pname = br->GetName();
      t1->SetBranchStatus(pname.c_str(), 1);

      for(int iEntry= 0; iEntry< nEntries ; ++iEntry){
        recoInfo.setEntry(iEntry, false, true);
        std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();
      
//        if( iEntry==848000 && (jEntry%10)==0){std::cout << jEntry << " / "<< pname << std::endl;}
        if(evtInfo.first == Run && evtInfo.second == Event){
          const auto particles = recoInfo.getParticles("muon");
          int cEntries = TC->GetSize();

          if(cEntries>0){

    	  for( auto recM : particles ){
	        double rpt = recM.first.Pt();
	        double reta = recM.first.Eta();
	        double rphi = recM.first.Phi();
		int fillcount=0;
		bool isMatched;
	    for(int k = 0 ; k < cEntries; k++){
      	      TLorentzVector* onv = (TLorentzVector*) TC->At(k);
    	      double opt = onv->Pt();
              double oeta = onv->Eta();
    	      double ophi = onv->Phi();

    	        double dR = std::sqrt((oeta-reta)*(oeta-reta)+(ophi-rphi)*(ophi-rphi));
    	        isMatched = (dR < 0.1) ? true: false;
		if(isMatched){break;}

	      }
	      const auto centI = recoInfo.getCentrality();
	      effVec[i].Fill(isMatched, centI);
            }
	  }
        } 
	//progf +=1;
      }
      t1->SetBranchStatus(pname.c_str(), 0);
    }
    clock1->Stop();
    cyt= clock1->RealTime()*(double(onentries));
    clock1->Reset();
    progf += 1;
  }
  std::cout << "Done Filling" << std::endl;
  TFile* file = new TFile("outputEffMuCent_dump.root","recreate");
  int tt= 0;
  std::vector<TGraphAsymmErrors> graphM;
  for(auto& Eff : effVec){
    auto name = effMapTitle[tt];
    TCanvas c("c", "c", 1000, 1000); c.cd();
    Eff.SetName(name.c_str());
    Eff.SetTitle(name.c_str());
    Eff.Draw(); gPad->Update();

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
