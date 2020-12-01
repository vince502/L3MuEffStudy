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

struct fourh{
  TH1D hrec, hon, honf, hronf;
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
void plotEffMuCent_v5_hwan(std::string filen ="L3_crabbed_1176_wL2fix"){
  std::string reco = "../L3MuEffStudy/Large_Files/Forest_HIMinimumBias2_run327237_merged.root";

  //initiate ratio map
  std::map<std::string, std::pair<TH1D, TH1D> > ratioM; 
  int modulen = 14;
  std::vector<struct fourh> vit;

  //Initiate Multi-Threading
  const int nCores = modulen;
  ROOT::EnableImplicitMT();
  ROOT::TProcessExecutor mpe(nCores);
 
  //Multi-THread fill ratio function
  TH1::AddDirectory(kFALSE);
  auto fillRatio = [=](int idx)
  {
  //fill event pair vector
  std::ifstream evml;
  evml.open(("realEvtpair_"+filen+".txt").c_str());
  std::vector<std::pair<int, int> >evl;
  std::string str;
  int icount=0;
  while( getline(evml, str)){
    if((icount%10000)==0){std::cout << "Getting line: " << icount << "\r"; std::cout.flush();}
    icount++;
    int oev, rev;
    std::istringstream strb(str);
    strb >> oev >> rev;
    std::pair<int, int>pbuf = std::make_pair(oev, rev);
    evl.push_back(pbuf);
  }
  evml.close();
  std::cout<< "DONE Evt Init" <<std::endl;

  //File Initiate
  RecoReader recoInfo(reco, false);
  const auto nEntries = recoInfo.getEntries();
  TFile* l3t = new TFile(("../L3MuEffStudy/Large_Files/"+filen+".root").c_str(),"open");
  TTree* t1 = (TTree*) l3t->Get("l3pAnalyzer/L3Track");
  TTreeReader r1 = TTreeReader("l3pAnalyzer/L3Track",l3t);
  TObjArray* blist = t1->GetListOfBranches();
  int oEvent;
  t1->SetBranchAddress("Event", &oEvent);
  Long64_t onentries = t1->GetEntries();
  static const int Max_mu_size = 32000;
  TClonesArray* TC = new TClonesArray("TLorentzVector", Max_mu_size);

  int count=0;


  //Init Histograms
  TH1::SetDefaultSumw2;
  TH1D* hr = new TH1D("hr","hr", 20,0, 100);
  TH1D* ho = new TH1D("ho","ho", 20,0, 100);
  TH1D* hof = new TH1D("hof","hof", 20,0, 100);
  TH1D* hrof = new TH1D("hrof","hrof", 20,0, 100);
  hr->Sumw2();
  ho->Sumw2();
  hof->Sumw2();
  hrof->Sumw2();
  std::cout << "total Events: " << evl.size() << std::endl;
  
  //RECO muon init
  recoInfo.initBranches("muon");

  //Branch Set
  TBranch* br = (TBranch*) blist->At(idx+2);
  std::string pname = br->GetName();
  t1->SetBranchAddress(pname.c_str(), &TC);

  //Loop over muons
  for(std::vector<std::pair<int, int> >::const_iterator it = evl.begin(); it != evl.end(); it++){
    int oev = it->first; int rev = it->second;
    t1->GetEntry(oev);
    recoInfo.setEntry(rev, false, true);
    const auto particles = recoInfo.getParticles("muon");
    int cEntries = TC->GetEntries();
    const auto centI = recoInfo.getCentrality()/2;
    if((count%100000)==0){std::cout << "[INFO] Core: ["<< idx <<"], doing entry: "<< count<< std::endl; /*std::pair<Long64_t, Long64_t> evtInfo = recoInfo.getEventNumber();std::cout<< oEvent<<"/" << evtInfo.second << std::endl;
    std::cout <<"Idx: "<<oev<< " / "<<rev << std::endl;*/} 
    count++;
    //std::vector<bool>  OMatchedV;
    std::vector< std::pair< bool, bool > > OMatchedV(cEntries, {false, false});
    for(int k =0; k < cEntries; k++){
      TLorentzVector* onmuon = (TLorentzVector*) TC->At(k);
      if ( (std::abs(onmuon->Eta())<0.3 && onmuon->Pt()>=3.4) ||
  	    (0.3<=std::abs(onmuon->Eta())<1.1 && onmuon->Pt()>=3.3) ||
  	    (1.1<=std::abs(onmuon->Eta())<1.4 && onmuon->Pt()>=7.7-4.0*std::abs(onmuon->Eta())) ||
  	    (1.4<=std::abs(onmuon->Eta())<1.55 && onmuon->Pt()>=2.1) ||
  	    (1.55<=std::abs(onmuon->Eta())<2.2 && onmuon->Pt()>=4.25 -1.39*std::abs(onmuon->Eta())) ||
  	    (2.2<=std::abs(onmuon->Eta())<2.4 && onmuon->Pt()>=1.2) )
	    //OMatchedV.push_back(false);
  	    {OMatchedV[k].second=true;}
    }
    for( auto recM : particles){
      TLorentzVector recomuon = recM.first;
      hr->Fill(centI); 
      bool isMatched = false;
      for(int k=0; k<cEntries; k++){
	bool OMatched = false;
        TLorentzVector* onmuon = (TLorentzVector*) TC->At(k);
	if (!OMatchedV[k].second){continue;}
	double dRcut = pname.find("L2") ? 0.3 : 0.1;
	if(onmuon->DeltaR(recomuon) < dRcut && std::abs(onmuon->Pt()-recomuon.Pt())/recomuon.Pt() < 0.1){isMatched = true; OMatched = true;}

        //OMatchedV[k] = OMatchedV[k]||OMatched;
        OMatchedV[k].first = OMatchedV[k].first||OMatched;
      }
      if(isMatched){ho->Fill(centI);}
    }
    for(int j=0; j < cEntries; j++){
      if (!OMatchedV[j].second){continue;}
      hrof->Fill(centI);
      //if(!OMatchedV[j]){hof->Fill(centI);}
      if(!OMatchedV[j].first){hof->Fill(centI);}
    }
  }
  TH1D* h3 = (TH1D*) ho->Clone("h3");
  TH1D* h4 = (TH1D*) hof->Clone("h4");
  h3->Divide(hr);
  h4->Divide(hrof);
  h3->SetName(Form("%s_efficiency",pname.c_str()));
  h4->SetName(Form("%s_online_fake_ratio",pname.c_str()));
  hr->SetName(Form("%s_reco",pname.c_str()));
  ho->SetName(Form("%s_online_matched_reco",pname.c_str()));
  hof->SetName(Form("%s_reco_notmatched_online",pname.c_str()));
  hrof->SetName(Form("%s_online",pname.c_str()));
  std::pair<TH1D, TH1D> hp = std::pair(*h3, *h4);
  auto ratiop1 = std::pair<std::string, std::pair<TH1D, TH1D> >(pname, hp);
  struct fourh tran = {*hr, *ho, *hof, *hrof};
  auto ratioP = std::make_pair(ratiop1, tran);
  hr->Clear(); ho->Clear(); hof->Clear(); hrof->Clear();  h3->Delete(); h4->Delete();
  
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
  TFile* out = new TFile(("outputratioL3_"+filen+"L2_MT_gyeonghwan.root").c_str(),"recreate");
  std::vector<struct fourh>::iterator vitt = vit.begin();
  for(std::map<std::string, std::pair<TH1D, TH1D> >::iterator itt = ratioM.begin(); itt != ratioM.end(); itt++){

    auto name = itt->first;
    std::cout << "Saving Histogram: " << name << std::endl; 
    makeDirFile(out, name);
    TDirectory* subdir_ = out ->GetDirectory(name.c_str());
    TH1D HO = itt->second.first;
    TH1D HOF = itt->second.second;
    TH1D HON, HOFN, HRD, HROF;
    std::string honame = HO.GetName();
    honame = honame.substr(0,honame.size()-11);
    std::cout << "Parsed: " << honame << std::endl;
    for(std::vector<struct fourh>::iterator vitt = vit.begin(); vitt != vit.end(); vitt++){
      TH1D HON_ = vitt->hon;
      TH1D HOFN_ = vitt->honf;
      TH1D HRD_ = vitt->hrec;
      TH1D HROF_ = vitt->hronf;
      std::string Hname = HON_.GetName();
      Hname = Hname.substr(0,Hname.size()-20);
      if( Hname.compare(honame) ==0){
	HON = HON_;
        HOFN = HOFN_;
        HRD = HRD_;
	HROF = HROF_;
      }
    }
    HO.SetTitle(Form("Efficiency %s_reco_online",name.c_str()));
    HOF.SetTitle(Form("Ratio %s_online fake",name.c_str()));
    HON.SetTitle(Form("%s_online_matched_reco",name.c_str()));
    HOFN.SetTitle(Form("%s_reco_notmatched_online",name.c_str()));
    HRD.SetTitle(Form("%s_reco",name.c_str()));
    HROF.SetTitle(Form("%s_online",name.c_str()));
    subdir_->cd();
    std::cout<< HO.GetName() << std::endl;
    HO.Write();
    HOF.Write();
    HON.Write();
    HOFN.Write();
    HRD.Write();
    HROF.Write();
    vitt++;
  }
  out->Write();
  out->Close();
  std::cout << "Done Saving" << std::endl;
}
