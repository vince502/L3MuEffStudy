#include <TROOT.h>
#include <TFile.h>
#include <TString.h>
#include <iostream>
#include <iomanip>
#include <memory>
#include <TTree.h>
#include <string>
#include <TH2D.h>
#include <TStopwatch.h>
#include "Cent_plotTurnOn.h"

struct fourh{
  TH2D hrec, hon, honf, hronf;
};

bool SetOEntry(const std::pair<Long64_t, Long64_t>& recoevt, TTree* t1){
  const auto index = t1->GetEntryNumberWithIndex(recoevt.first, recoevt.second);
  if (index<0) { return false;}
  t1->GetEntry(index);
  return true;
};
bool SetOEntry(const Long64_t& recoevt, TTree* t1){
  const auto index = t1->GetEntryNumberWithIndex(recoevt);
  if (index<0) { return false;}
  t1->GetEntry(index);
  return true;
};

//Progress Printer
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

//TFile make directory
void makeDirFile(TFile *f1, const std::string& dir)
{
  TDirectory* subdir = f1->mkdir(dir.c_str());
};

//Main Function
void plotEffMuCent_PGvsONIA_pteta(std::string filen ="L3_2021May_113X_Pt_0p5_100"){
  std::string reco = "Oniatree_MC_miniAOD_PG_Pt_0p5_100_Hydjet_5p02TeV_cmssw11_2_2_Run3Cond_merged.root";

  //initiate ratio map
  std::map<std::string, std::pair<std::pair<TH2D, TH2D>, std::pair<TH2D, TH2D> > > ratioM;
  int modulen = 12;
  std::vector<std::pair<struct fourh, struct fourh> > vit;

  //Initiate Multi-Threading
  const int nCores = modulen;
  ROOT::EnableImplicitMT();
  ROOT::TProcessExecutor mpe(nCores);
 
  //Multi-THread fill ratio function
  TH1::AddDirectory(kFALSE);
  auto fillRatio = [=](int idx)
  {

  //File Initiate
  //RecoReader recoInfo(reco, false);
  TFile* recoFile = new TFile(("../L3MuEffStudy/Large_Files/"+reco).c_str(),"READ");
  TTree* myTree = (TTree*) recoFile->Get("hionia/myTree");
  TTreeReader r0 = TTreeReader("hionia/myTree");
  const auto nEntries = myTree->GetEntries(); 
  TFile* l3t = new TFile(("../L3MuEffStudy/Large_Files/"+filen+".root").c_str(),"READ");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track",l3t);
  TObjArray* blist = t1->GetListOfBranches();
  Int_t oEvent, oRun;
  UInt_t rEvent;
  int iCent;
  myTree->SetBranchAddress("eventNb", &rEvent);
  myTree->SetBranchAddress("Centrality", &iCent);
  t1->SetBranchAddress("Event", &oEvent);
  t1->SetBranchAddress("Run", &oRun);
  Long64_t onentries = t1->GetEntries();
  static const int Max_mu_size = 32000;
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);
  TClonesArray* OTC = new TClonesArray("TLorentzVector", Max_mu_size);

  int count=0;


  //Init Histograms
  TH2::SetDefaultSumw2;
/*  TH2D* hr = new TH2D("hr","hr", 20,0, 100);
  TH2D* ho = new TH2D("ho","ho", 20,0, 100);
  TH2D* hof = new TH2D("hof","hof", 20,0, 100);
  TH2D* hrof = new TH2D("hrof","hrof", 20,0, 100);*/
  TH2D* hrl = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hol = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hofl = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hrofl = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hrh = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hoh = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hofh = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);
  TH2D* hrofh = new TH2D("hr", "hr", 15, 0, 30, 24, -2.4, 2.4);

  hrl->Sumw2();
  hol->Sumw2();
  hofl->Sumw2();
  hrofl->Sumw2();
  hrh->Sumw2();
  hoh->Sumw2();
  hofh->Sumw2();
  hrofh->Sumw2();
  
  //RECO muon init
//  recoInfo.initBranches("muon");

  //Online Build Index
  t1->BuildIndex("Event");
  myTree->BuildIndex("eventNb");

  //Branch Set
  TBranch* br = (TBranch*) blist->At(idx+2);
  std::string pname = br->GetName();
  t1->SetBranchAddress(pname.c_str(), &TC);
  myTree->SetBranchAddress("Reco_mu_4mom", &OTC);

  //Loop over muons
  for (const auto& iEntry : ROOT::TSeqUL(nEntries)){
    //test
  //  if (count >300) break;
    myTree->GetEntry(iEntry);
    //recoInfo.setEntry(iEntry, false, true);
    //const auto particles = recoInfo.getParticles("muon");
    const auto particles = *OTC;
    if( !SetOEntry(rEvent, t1)) continue;
    int cEntries = TC->GetEntries();
    const auto centI =iCent ;// recoInfo.getCentrality()/2;
    double centralcut, periphcut;
    centralcut = 10; periphcut = 40;
    if( centI > centralcut && centI < periphcut){continue;}
    if((count%10000)==0){std::cout << "[INFO] Core: ["<< idx <<"], doing entry: "<< count<< std::endl; /*std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();std::cout<< oEvent<<"/" << evtInfo.second << std::endl;
    std::cout <<"Idx: "<<oev<< " / "<<rev << std::endl;*/} 
    count++;
    //std::vector<bool>  OMatchedV;
    std::vector< std::pair< bool, bool > > OMatchedV(cEntries, {false, false});
    for( auto recM : particles){
      TLorentzVector* recomuon = (TLorentzVector*) recM;
      if(centI <= centralcut){
	hrl->Fill(recomuon->Pt(), recomuon->Eta());
      }
      else if(centI >= periphcut){
	hrh->Fill(recomuon->Pt(), recomuon->Eta());
      }
      bool isMatched = false;
      for(int k =0; k < cEntries; k++){
	bool OMatched = false;
        TLorentzVector* onmuon = (TLorentzVector*) TC->At(k);
        if ( (std::abs(onmuon->Eta())<0.3 && onmuon->Pt()>=3.4) ||
             (0.3<=std::abs(onmuon->Eta())<1.1 && onmuon->Pt()>=3.3) ||
             (1.1<=std::abs(onmuon->Eta())<1.4 && onmuon->Pt()>=7.7-4.0*std::abs(onmuon->Eta())) ||
             (1.4<=std::abs(onmuon->Eta())<1.55 && onmuon->Pt()>=2.1) ||
             (1.55<=std::abs(onmuon->Eta())<2.2 && onmuon->Pt()>=4.25 -1.39*std::abs(onmuon->Eta())) ||
             (2.2<=std::abs(onmuon->Eta())<2.4 && onmuon->Pt()>=1.2) )
            //OMatchedV.push_back(false);
             {OMatchedV[k].second=true; continue;}
  	double dRcut = pname.find("L2") ? 0.3 : 0.1;
  	if(onmuon->DeltaR(*recomuon) < dRcut && std::abs(onmuon->Pt()-recomuon->Pt())/recomuon->Pt() < 0.1){isMatched = true; OMatched = true;}
        OMatchedV[k].first = OMatchedV[k].first||OMatched;
        if(isMatched){
  	  if(centI <= centralcut){
  	    hol->Fill(recomuon->Pt(),recomuon->Eta());
  	  }
  	  else if(centI >= periphcut){
  	    hoh->Fill(recomuon->Pt(),recomuon->Eta());
	  }
  	}
      }
    }
    for(int j=0; j < cEntries; j++){
      TLorentzVector* onmuon = (TLorentzVector*) TC->At(j);
      if(!OMatchedV[j].second){ continue; }
      if(centI <= centralcut){
	hrofl->Fill(onmuon->Pt(),onmuon->Eta());
      }
      else if(centI >=periphcut){
        hrofh->Fill(onmuon->Pt(),onmuon->Eta());
      }
      if(!OMatchedV[j].first) {
	if(centI <= centralcut){
	  hofl->Fill(onmuon->Pt(),onmuon->Eta());
	}
	else if(centI >=periphcut){
	  hofh->Fill(onmuon->Pt(),onmuon->Eta());
	}
      }
    }
  }
  TH2D* h3l = (TH2D*) hol->Clone("h3l");
  TH2D* h4l = (TH2D*) hofl->Clone("h4l");
  TH2D* h3h = (TH2D*) hoh->Clone("h3h");
  TH2D* h4h = (TH2D*) hofh->Clone("h4h");

  h3l->Divide(hrl);
  h4l->Divide(hrofl);
  h3h->Divide(hrh);
  h4h->Divide(hrofh);

  h3l->SetName(Form("%s_efficiency_0-10",pname.c_str()));
  h4l->SetName(Form("%s_online_fake_ratio_0-10",pname.c_str()));
  hrl->SetName(Form("%s_reco_0-10",pname.c_str()));
  hol->SetName(Form("%s_online_matched_reco_0-10",pname.c_str()));
  hofl->SetName(Form("%s_reco_notmatched_online_0-10",pname.c_str()));
  hrofl->SetName(Form("%s_online_0-10",pname.c_str()));

  h3h->SetName(Form("%s_efficiency_70-100",pname.c_str()));
  h4h->SetName(Form("%s_online_fake_ratio_70-100",pname.c_str()));
  hrh->SetName(Form("%s_reco_70-100",pname.c_str()));
  hoh->SetName(Form("%s_online_matched_reco_70-100",pname.c_str()));
  hofh->SetName(Form("%s_reco_notmatched_online_70-100",pname.c_str()));
  hrofh->SetName(Form("%s_online_70-100",pname.c_str()));

  std::pair<TH2D, TH2D> hpl = std::pair(*h3l, *h4l);
  std::pair<TH2D, TH2D> hph = std::pair(*h3h, *h4h);
  std::pair<std::pair<TH2D, TH2D>, std::pair<TH2D, TH2D> > hp = std::pair(hpl, hph);
  auto ratiop1 = std::pair<std::string, std::pair<std::pair<TH2D, TH2D>, std::pair<TH2D, TH2D> > >(pname, hp);
  struct fourh tranl = {*hrl, *hol, *hofl, *hrofl};
  struct fourh tranh = {*hrh, *hoh, *hofh, *hrofh};
  std::pair<struct fourh, struct fourh>tran = std::make_pair(tranl, tranh);
  auto ratioP = std::make_pair(ratiop1, tran);
  hrl->Clear(); hol->Clear(); hofl->Clear(); hrofl->Clear();  h3l->Delete(); h4l->Delete();
  hrh->Clear(); hoh->Clear(); hofh->Clear(); hrofh->Clear();  h3h->Delete(); h4h->Delete();
  
  std::cout<< std::endl;
  std::cout << "DONE filling ratio plot: " << pname.c_str() <<std::endl;
  return ratioP;
  };
  
  const auto& res = mpe.Map(fillRatio, ROOT::TSeqI(nCores));
  std::cout << "DEBUG split" << std::endl;
  for (const auto& r: res){
    ratioM.insert(r.first);
    vit.push_back(r.second);
  }
  std::cout << "DONE allocating ratio maps" << std::endl;
  //modify plots & draw
  TFile* out = new TFile(("outputratioL3_"+filen+"_L2_PtEta_MT_PGvsONIA.root").c_str(),"recreate");
  std::vector<std::pair<struct fourh, struct fourh> >::iterator vitt = vit.begin();
  for(std::map<std::string, std::pair<std::pair<TH2D, TH2D>, std::pair<TH2D, TH2D> > >::iterator itt = ratioM.begin(); itt != ratioM.end(); itt++){

    auto name = itt->first;
    std::cout << "Saving Histogram: " << name << std::endl; 
    makeDirFile(out, name);
    TDirectory* subdir_ = out ->GetDirectory(name.c_str());
    TH2D HOL = itt->second.first.first;
    TH2D HOFL = itt->second.first.second;
    TH2D HOH = itt->second.second.first;
    TH2D HOFH = itt->second.second.second;
    TH2D HONL, HOFNL, HRDL, HROFL;
    TH2D HONH, HOFNH, HRDH, HROFH;
    std::string honame = HOL.GetName();
    honame = honame.substr(0,honame.size()-16);
    std::cout << "Parsed: " << honame << std::endl;
    for(std::vector<std::pair<struct fourh, struct fourh> >::iterator vitt = vit.begin(); vitt != vit.end(); vitt++){
      TH2D HONL_ = vitt->first.hon;
      TH2D HOFNL_ = vitt->first.honf;
      TH2D HRDL_ = vitt->first.hrec;
      TH2D HROFL_ = vitt->first.hronf;
      TH2D HONH_ = vitt->second.hon;
      TH2D HOFNH_ = vitt->second.honf;
      TH2D HRDH_ = vitt->second.hrec;
      TH2D HROFH_ = vitt->second.hronf;
      std::string Hname = HONL_.GetName();
      Hname = Hname.substr(0,Hname.size()-25);
      std::cout << Hname << "/" <<honame<< std::endl;
      if( Hname.compare(honame) ==0){
	HONL = HONL_;
        HOFNL = HOFNL_;
        HRDL = HRDL_;
	HROFL = HROFL_;
	HONH = HONH_;
        HOFNH = HOFNH_;
        HRDH = HRDH_;
	HROFH = HROFH_;
      }
    }
    HOL.SetTitle(Form("Efficiency %s_reco_online_0-10",name.c_str()));
    HOFL.SetTitle(Form("Ratio %s_online fake_0-10",name.c_str()));
    HONL.SetTitle(Form("%s_online_matched_reco_0-10",name.c_str()));
    HOFNL.SetTitle(Form("%s_reco_notmatched_online_0-10",name.c_str()));
    HRDL.SetTitle(Form("%s_reco_0-10",name.c_str()));
    HROFL.SetTitle(Form("%s_online_0-10",name.c_str()));
    HOH.SetTitle(Form("Efficiency %s_reco_online_70_100",name.c_str()));
    HOFH.SetTitle(Form("Ratio %s_online fake_70_100",name.c_str()));
    HONH.SetTitle(Form("%s_online_matched_reco_70_100",name.c_str()));
    HOFNH.SetTitle(Form("%s_reco_notmatched_online_70_100",name.c_str()));
    HRDH.SetTitle(Form("%s_reco_70_100",name.c_str()));
    HROFH.SetTitle(Form("%s_online_70_100",name.c_str()));
    subdir_->cd();
    std::cout<< HOL.GetName() << std::endl;
    HOL.Write();
    HOFL.Write();
    HONL.Write();
    HOFNL.Write();
    HRDL.Write();
    HROFL.Write();
    HOH.Write();
    HOFH.Write();
    HONH.Write();
    HOFNH.Write();
    HRDH.Write();
    HROFH.Write();
    vitt++;
  }
  out->Write();
  out->Close();
  std::cout << "Done Saving" << std::endl;
}
