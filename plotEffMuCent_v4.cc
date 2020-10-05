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

void plotEffMuCent_v4(std::string filen ="L3_crabbed_1171_part1" /*"1v_MBL3"*/){
  std::string reco = "/Users/soohwanlee/RunPreparation/store/Forest_HIMinimumBias2_run327237_merged.root";

  //initiate evt number list


  //initiate ratio map
  std::map<std::string, std::pair<TH1D, TH1D> > ratioM;
/*  for(int i=0; i<13; i++){
    TBranch* br = (TBranch*) blist->At(i+2);
    std::string pname = br->GetName();
    ratioM[pname] = std::make_pair(TH1D(("H_"), ("Ratio " + pname).c_str(),20, 0, 100),TH1D("H2",("Fake Ratio "+ panem).c_str(),20, 0, 100));
  }*/

  //fill ratio
  const int nCores = 13;
  ROOT::EnableImplicitMT();
  ROOT::TProcessExecutor mpe(nCores);

  TH1::AddDirectory(kFALSE);
  auto fillRatio = [=](int idx)
  {
  std::ifstream evml;
  evml.open(("./matchTXT/realEvtpair_"+filen+".txt").c_str());
  std::vector<std::pair<int, int> >evl;
  std::string str;
  int icount=0;
  while( getline(evml, str)){
    if((icount%10000)==0){std::cout << "Getting line: " << icount << "\r"; std::cout.flush();}
    icount++;
//    if(icount >100000){break;}
    int oev, rev;
    std::istringstream strb(str);
    strb >> oev >> rev;
    std::pair<int, int>pbuf = std::make_pair(oev, rev);
    evl.push_back(pbuf);
  }
  evml.close();
  std::cout<< std::endl;
  std::cout<< "DONE Evt Init" <<std::endl;
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile(("Large_Files/"+filen+".root").c_str(),"open");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track",l3t);
  TObjArray* blist = t1->GetListOfBranches();
  int oEvent;
  t1->SetBranchAddress("Event", &oEvent);
//  t1->SetBranchStatus("*",0);
//  t1->SetBranchStatus("Event",1);
//  t1->SetBranchStatus("Run",1);
  Long64_t onentries = t1->GetEntries();
  static const int Max_mu_size = 32000;
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);
  int count=0;
  TH1D* hr = new TH1D("hr","hr", 20,0, 100);
  TH1D* ho = new TH1D("ho","ho", 20,0, 100);
  TH1D* hof = new TH1D("hof","hof", 20,0, 100);
  std::cout << "total Events: " << evl.size() << std::endl;
  recoInfo.initBranches("muon");
  TBranch* br = (TBranch*) blist->At(idx+2);
  std::string pname = br->GetName();
  t1->SetBranchAddress(pname.c_str(), &TC);
  for(std::vector<std::pair<int, int> >::const_iterator it = evl.begin(); it != evl.end(); it++){
    int oev = it->first; int rev = it->second;
    count++;
    t1->GetEntry(oev);
    recoInfo.setEntry(rev, false, true);
    const auto particles = recoInfo.getParticles("muon");
    int cEntries = TC->GetEntries();
    const auto centI = recoInfo.getCentrality();
    if((count%1000)==0){std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();std::cout<< oEvent<<"/" << evtInfo.second << std::endl;
    std::cout <<"Idx: "<<oev<< " / "<<rev << std::endl;} 
    if(cEntries>0){
//    std::cout << "matching online muon: " <<pname.c_str() << "cEntries: "<< cEntries << std::endl;
      for( auto recM : particles ){
        double rpt = recM.first.Pt();
        double reta = recM.first.Eta();
        double rphi = recM.first.Phi();
        hr->Fill(centI);
        bool isMatched;
        for(int k=0; k<cEntries; k++){
          TLorentzVector* onv = (TLorentzVector*) TC->At(k);
          double opt = onv->Pt();
          double oeta = onv->Eta();
          double ophi = onv->Phi();
          double dR = std::sqrt((oeta-reta)*(oeta-reta)+(ophi-rphi)*(ophi-rphi));
          isMatched = (dR < 0.1) ? true : false;
          if(isMatched){
            ho->Fill(centI);
          }
          else{
            hof->Fill(centI);
          }
        }
      }
    }
  }
  TH1D* h3 = (TH1D*) ho->Clone("h3");
  TH1D* h4 = (TH1D*) hof->Clone("h3");
  h3->Divide(hr);
  h4->Divide(hr);
  h3->SetName(Form("%s_online",pname.c_str()));
  h4->SetName(Form("%s_online fake",pname.c_str()));
  std::pair<TH1D, TH1D> hp = std::make_pair(*h3, *h4);
  auto ratioP = std::pair<std::string, std::pair<TH1D, TH1D> >(pname, hp);
  hr->Clear(); ho->Clear(); hof->Clear(); h3->Delete(); h4->Delete();
  
  std::cout<< std::endl;
  std::cout << "DONE filling ratio plot: " << pname.c_str() <<std::endl;
  return ratioP;
  };
  
  const auto& res = mpe.Map(fillRatio, ROOT::TSeqI(nCores));

  for (const auto& r: res){
    ratioM.insert(r);
  }
  std::cout << "DONE allocating ratio maps" << std::endl;
  //modify plots & draw
  TFile* out = new TFile(("outputratioL3_"+filen+".root").c_str(),"recreate");
  for(std::map<std::string, std::pair<TH1D, TH1D> >::const_iterator itt = ratioM.begin(); itt != ratioM.end(); itt++){
    auto name = itt->first;
    makeDirFile(out, name);
    TDirectory* subdir_ = out ->GetDirectory(name.c_str());
    TH1D HO = itt->second.first;
    TH1D HOF = itt->second.second;
    //HO.SetName(Form("%s_online",name.c_str()));
    //HOF.SetName(Form("%s_oneline_fake",name.c_str()));
    subdir_->cd();
    HO.Write();
    HOF.Write();
  }
  out->Write();
  out->Close();
  std::cout << "Done Saving" << std::endl;
}
