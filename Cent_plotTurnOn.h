#ifndef Cent_plotTurnOn_h
#define Cent_plotTurnOn_h

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TEfficiency.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TLorentzVector.h>
#include <TGraphAsymmErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TAxis.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <ROOT/TProcessExecutor.hxx>
#include <ROOT/TSeq.hxx>

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <dirent.h>
#include <memory>

#include "tdrstyle.C"
#include "CMS_lumi.C"


class TreeReaderBase
{
 public:
  TreeReaderBase(){};

  TreeReaderBase(const std::string& path)
  {
    setFile(path);
  };

  ~TreeReaderBase()
  {
    if (file_ && file_->IsOpen()) file_->Close();
  };

  // getters
  Long64_t getEntries() const 
  {
    if (reader_.empty()) { return 0; }
    return reader_.begin()->second->GetEntries(false);
  };

  // setters
  void setFile(const std::string& path)
  {
    file_.reset(TFile::Open(path.c_str(), "READ"));
    if (!file_->IsOpen() || file_->IsZombie()) {
      throw std::runtime_error("[ERROR] File "+path+" failed to open!");
    }
  };

  void setEntry(const Long64_t& i, const bool& checkValue, const bool& onDemand)
  {
    if (i<0) { throw std::runtime_error(Form("[ERROR] Invalid index: %lld", i)); }
    onDemand_ = onDemand;
    index_ = i;
    checkValue_ = checkValue;
    if (!onDemand_) {
      for (const auto& r : reader_) {
        loadEntry(r.first);
      }
    }
  };

 protected:
  void checkValue(ROOT::Internal::TTreeReaderValueBase* value) const
  {
    if (value==NULL) {
      throw std::runtime_error("[ERROR] Value pointer is null");
    }
    else if (value->GetSetupStatus() < 0) {
      throw std::runtime_error(Form("[ERROR] Status %d when setting up reader for %s", value->GetSetupStatus(), value->GetBranchName()));
    }
  };

  virtual void checkValues(const std::string& tree) const
  {
  };

  void setTreeReader(const std::string& name, const std::string& dir)
  {
    if (reader_.find(name)!=reader_.end() || !file_) return;
    reader_[name].reset(new TTreeReader(dir.c_str(), file_.get()));
    if (!reader_.at(name)->GetTree()) {
      throw std::runtime_error("[ERROR] Failed to open "+dir+" !");
    }
    currentEntry_[name] = -1;
  };

  void loadTreeEntry(TTreeReader* r) const
  {
    if (!r) return;
    const auto status = r->SetEntry(index_);
    if (status!=TTreeReader::kEntryValid) {
      std::string msg = "";
      if      (status==TTreeReader::kEntryNotLoaded) { msg = "no entry has been loaded yet"; }
      else if (status==TTreeReader::kEntryNoTree) { msg = "the tree does not exist"; }
      else if (status==TTreeReader::kEntryNotFound) { msg = "the tree entry number does not exist"; }
      else if (status==TTreeReader::kEntryChainSetupError) { msg = "problem in accessing a chain element, e.g. file without the tree"; }
      else if (status==TTreeReader::kEntryChainFileError) { msg = "problem in opening a chain's file"; }
      else if (status==TTreeReader::kEntryDictionaryError) { msg = "problem reading dictionary info from tree"; }
      //else if (status==TTreeReader::kEntryLast) { msg = "last entry was reached"; }
      throw std::runtime_error("[ERROR] Invalid entry: "+msg);
    }
  };

  void loadEntry(const std::string& tree) const
  {
    if (index_!=currentEntry_.at(tree)) {
      loadTreeEntry(reader_.at(tree).get());
      if (checkValue_) { checkValues(tree); }
      const_cast<std::map<std::string, Long64_t >* >(&currentEntry_)->at(tree) = index_;
    }
  };

  std::unique_ptr<TFile> file_;
  std::map<std::string, std::unique_ptr<TTreeReader> > reader_;
  bool checkValue_, onDemand_;
  Long64_t index_;
  std::map<std::string, Long64_t > currentEntry_;
};


class TriggerReader : public TreeReaderBase
{
 public:
  TriggerReader(const std::string& path, const bool& buildIndex=false)
  {
    setFile(path);
    setTreeReader("HltTree", "hltanalysis/HltTree");
    if (buildIndex) {
      std::cout<< "[INFO] Building index in TriggerReader for HltTree" << std::endl;
      reader_.at("HltTree")->GetTree()->BuildIndex("Run","Event");
      if (getEntries()>0) { reader_.at("HltTree")->GetTree()->LoadTree(0); }
    }
    evtI_["runN"].reset(new TTreeReaderValue<int>(*reader_.at("HltTree"), "Run"));
    evtUL_["eventN"].reset(new TTreeReaderValue<ULong64_t>(*reader_.at("HltTree"), "Event"));
  };

  // getters
  std::pair<Long64_t, Long64_t> getEventNumber() const
  {
    if (onDemand_) { loadEntry("HltTree"); }
    if (evtUL_.find("eventN")==evtUL_.end()) { return {0, 0}; }
    return {*evtI_.at("runN")->Get(), *evtUL_.at("eventN")->Get()};
  };
    
  bool isTriggerMatched(const TLorentzVector& p4, const std::string& path) const
  {
    // set entry on demand for HltTree
    if (onDemand_) { loadEntry("HltTree"); }
    if (!*evtI_.at(path)->Get()) { return false; }
    // set entry on demand for TriggerObject
    if (onDemand_) { loadEntry(path);      }
    // define delta R threshold
    const auto dR = (path.rfind("HLT_HIL3",0)==0 ? 0.1 : 0.3);
    const auto dPt = (path.rfind("HLT_HIL1",0)==0 ? 2.0 : 10.0);
    // check trigger objects
    bool isMatch = false;
    for (size_t i=0; i<obj_.at(path).at("eta")->Get()->size(); i++) {
      // compare object momenta
      TLorentzVector trigP4; trigP4.SetPtEtaPhiM(obj_.at(path).at("pt")->Get()->at(i), obj_.at(path).at("eta")->Get()->at(i), obj_.at(path).at("phi")->Get()->at(i), p4.M());
      isMatch = (path.rfind("HLT_HIL1",0)==0 ? std::abs(trigP4.Eta()-p4.Eta()) < 0.2 : trigP4.DeltaR(p4) < dR) && std::abs(trigP4.Pt()-p4.Pt())/p4.Pt() < dPt;
      if (isMatch) break;
    }
    return isMatch;
  };

  // setters
  void addTrigger(const std::string& name)
  {
    if (name.rfind("HLT_",0)!=0) {
      throw std::runtime_error("[ERROR] Invalid trigger name "+name+" !");
    }
    if (evtI_.find(name)!=evtI_.end()) return;
    const auto nm = name.substr(0,name.rfind("_v")+2);
    setTreeReader(name, "hltobject/"+nm);
    evtI_[name].reset(new TTreeReaderValue<int>(*reader_.at("HltTree"), name.c_str()));
    for (const auto& var : {"pt", "eta", "phi"}) {
      obj_[name][var].reset(new TTreeReaderValue<std::vector<float> >(*reader_.at(name), var));
    }
  };

  using TreeReaderBase::setEntry;

  bool setEntry(const std::pair<Long64_t, Long64_t>& evtN, const bool& checkValue=true, const bool& onDemand=true)
  {
    const auto index = reader_.at("HltTree")->GetTree()->GetEntryNumberWithIndex(evtN.first, evtN.second);
    if (index<0) { return false; }
    setEntry(index, checkValue, onDemand);
    return true;
  };

 private:
  void checkValues(const std::string& tree) const
  {
    if (tree=="HltTree") {
      for (const auto& r : evtI_ ) { checkValue(r.second.get()); }
      for (const auto& r : evtUL_) { checkValue(r.second.get()); }
    }
    else if (obj_.find(tree)!=obj_.end()) {
      for (const auto& o : obj_.at(tree)) { checkValue(o.second.get()); }
    }
  };

  std::map<std::string, std::unique_ptr<TTreeReaderValue<int> > > evtI_;
  std::map<std::string, std::unique_ptr<TTreeReaderValue<ULong64_t> > > evtUL_;
  std::map<std::string, std::map<std::string, std::unique_ptr<TTreeReaderValue<std::vector<float> > > > > obj_;
};


class RecoReader : public TreeReaderBase
{
 public:
  RecoReader(const std::string& path, const bool& buildIndex=false)
  {
    setFile(path);
    setTreeReader("ggHiNtuplizerGED", "ggHiNtuplizerGED/EventTree");
    setTreeReader("skimanalysis", "skimanalysis/HltTree");
    setTreeReader("hiEvtAnalyzer", "hiEvtAnalyzer/HiTree");
    if (buildIndex) {
      std::cout<< "[INFO] Building index in RecoReader for hiEvtAnalyzer" << std::endl;
      reader_.at("hiEvtAnalyzer")->GetTree()->BuildIndex("run","evt");
      if (getEntries()>0) { reader_.at("hiEvtAnalyzer")->GetTree()->LoadTree(0); }
    }
    initEventInfo();
  };

  typedef std::vector<std::pair<TLorentzVector, int> > ParticleCollection;

  // getters
  std::pair<Long64_t, Long64_t> getEventNumber() const
  {
    if (onDemand_) { loadEntry("hiEvtAnalyzer"); }
    return {*evtUI_.at("runN")->Get(), *evtUL_.at("eventN")->Get()};
  };

  ParticleCollection getParticles(const std::string& type) const
  {
    if (onDemand_) { loadEntry("ggHiNtuplizerGED"); }
    if (getSize(type)==0) return {};
    // loop over muons
    ParticleCollection particles;
    for (size_t i=0; i<getSize(type); i++) {
      if (passParticleCut(type, i)) {
        particles.push_back({getP4(type, i), getPdgID(type, i)});
      }
    }
    return particles;
  };

  bool passEventSelection() const
  {
    if (onDemand_) { loadEntry("skimanalysis"); }
    return *skimI_.at("evtSel")->Get();
  };

  int getCentrality() const
  {
    if (onDemand_) { loadEntry("hiEvtAnalyzer"); }
    return *evtI_.at("hiBin")->Get();
  };

  // setters
  void initBranches(const std::string& type)
  {
    if      (type=="electron") { initElectron();  }
    else if (type=="photon"  ) { initPhoton();    }
    else if (type=="muon"    ) { initMuon();      }
  };

  using TreeReaderBase::setEntry;

  bool setEntry(const std::pair<Long64_t, Long64_t>& evtN, const bool& checkValue=true, const bool& onDemand=true)
  {
    const auto& index = reader_.at("hiEvtAnalyzer")->GetTree()->GetEntryNumberWithIndex(evtN.first, evtN.second);
    if (index<0) { return false; }
    setEntry(index, checkValue, onDemand);
    return true;
  };

 private:
  void checkValues(const std::string& tree) const
  { 
    if (tree=="ggHiNtuplizerGED") {
      for (const auto& o : objI_) { checkValue(o.second.get()); }
      for (const auto& o : objF_) { checkValue(o.second.get()); }
    }
    else if (tree=="hiEvtAnalyzer") {
      for (const auto& o : evtI_ ) { checkValue(o.second.get()); }
      for (const auto& o : evtUI_) { checkValue(o.second.get()); }
      for (const auto& o : evtUL_) { checkValue(o.second.get()); }
    }
    else if (tree=="skimanalysis") {
      for (const auto& o : skimI_ ) { checkValue(o.second.get()); }
    } 
  };

  size_t getSize(const std::string& type) const
  {
    if      (type=="electron") { return (objF_.find("elePt")!=objF_.end() ? objF_.at("elePt")->Get()->size() : 0); }
    else if (type=="photon"  ) { return (objF_.find("phoEt")!=objF_.end() ? objF_.at("phoEt")->Get()->size() : 0); }
    else if (type=="muon"    ) { return (objF_.find("muPt" )!=objF_.end() ? objF_.at("muPt" )->Get()->size() : 0); }
    return 0;
  };

  TLorentzVector getP4(const std::string& type, const size_t& i) const
  {
    TLorentzVector p4;
    if      (getSize(type)==0) { return p4; }
    else if (type=="electron") { p4.SetPtEtaPhiM(objF_.at("elePt")->Get()->at(i), objF_.at("eleSCEta")->Get()->at(i), objF_.at("eleSCPhi")->Get()->at(i), 0.000511);   }
    else if (type=="photon"  ) { p4.SetPtEtaPhiM(objF_.at("phoEt")->Get()->at(i), objF_.at("phoSCEta")->Get()->at(i), objF_.at("phoSCPhi")->Get()->at(i), 0.0);        }
    else if (type=="muon"    ) { p4.SetPtEtaPhiM(objF_.at("muPt" )->Get()->at(i), objF_.at("muEta"   )->Get()->at(i), objF_.at("muPhi"   )->Get()->at(i), 0.10565837); }
    return p4;
  };

  int getPdgID(const std::string& type, const size_t& i) const
  { 
    int pdgID = 0;
    if      (getSize(type)==0) { return pdgID; }
    else if (type=="electron") { pdgID = -11 * objI_.at("eleCharge")->Get()->at(i); }
    else if (type=="photon"  ) { pdgID = 22; }
    else if (type=="muon"    ) { pdgID = -13 * objI_.at("muCharge")->Get()->at(i); }
    return pdgID;
  };

  bool passParticleCut(const std::string& type, const size_t& i) const
  {
    if      (getSize(type)==0) { return false; }
    else if (type=="electron") { return passElectronCut(i); }
    else if (type=="photon"  ) { return passPhotonCut(i);   }
    else if (type=="muon"    ) { return passMuonCut(i);     }
    return false;
  };

  bool passElectronCut(const size_t& i) const
  {
    // set entry on demand
    if (onDemand_) { loadEntry("hiEvtAnalyzer");    }
    // check kinematics
    if (objF_.at("elePt")->Get()->at(i) <= 20.) return false;
    if ((std::abs(objF_.at("eleSCEta")->Get()->at(i)) >= 1.4 && std::abs(objF_.at("eleSCEta")->Get()->at(i)) <= 1.6) ||
        std::abs(objF_.at("eleSCEta")->Get()->at(i)) >= 2.1) return false;
    // use Loose ID working points for PbPb 2018
    float cutSigmaIEtaIEta, cutdEtaSeedAtVtx, cutdPhiAtVtx, cutEoverPInv, cutHoverEBc;
    if (*evtI_.at("hiBin")->Get() <= 60) {
      if (std::abs(objF_.at("eleSCEta")->Get()->at(i)) < 1.479) {
        cutSigmaIEtaIEta = 0.013451;
        cutdEtaSeedAtVtx = 0.003814;
        cutdPhiAtVtx = 0.037586;
        cutEoverPInv = 0.017664;
        cutHoverEBc = 0.161613;
      }
      else {
        cutSigmaIEtaIEta = 0.046571;
        cutdEtaSeedAtVtx = 0.006293;
        cutdPhiAtVtx = 0.118592;
        cutEoverPInv = 0.020135;
        cutHoverEBc = 0.131705;
      }
    }
    else {
      if (std::abs(objF_.at("eleSCEta")->Get()->at(i)) < 1.479) {
        cutSigmaIEtaIEta = 0.010867;
        cutdEtaSeedAtVtx = 0.003284;
        cutdPhiAtVtx = 0.020979;
        cutEoverPInv = 0.077633;
        cutHoverEBc = 0.126826;
      }
      else {
        cutSigmaIEtaIEta = 0.033923;
        cutdEtaSeedAtVtx = 0.006708;
        cutdPhiAtVtx = 0.083766;
        cutEoverPInv = 0.019279;
        cutHoverEBc = 0.097703;
      }
    }
    const bool passID = objF_.at("eleSigmaIEtaIEta_2012")->Get()->at(i) < cutSigmaIEtaIEta && std::abs(objF_.at("eledEtaSeedAtVtx")->Get()->at(i)) < cutdEtaSeedAtVtx &&
                        std::abs(objF_.at("eledPhiAtVtx")->Get()->at(i)) < cutdPhiAtVtx && objF_.at("eleEoverPInv")->Get()->at(i) < cutEoverPInv &&
                        objF_.at("eleHoverEBc")->Get()->at(i) < cutHoverEBc && std::abs(objF_.at("eleIP3D")->Get()->at(i)) < 0.03 && objI_.at("eleMissHits")->Get()->at(i) <= 1;
    return passID; 
  };

  bool passPhotonCut(const size_t& i) const
  {
    // check kinematics
    if (objF_.at("phoEt")->Get()->at(i) <= 40.) return false;
    if ((std::abs(objF_.at("phoSCEta")->Get()->at(i)) >= 1.44 && std::abs(objF_.at("phoSCEta")->Get()->at(i)) <= 1.57) ||
        std::abs(objF_.at("phoSCEta")->Get()->at(i)) >= 2.4) return false;
    // apply pre-selection
    if(!(objF_.at("phoSigmaIEtaIEta_2012")->Get()->at(i) > 0.002 && objF_.at("pho_swissCrx")->Get()->at(i) < 0.9 && abs(objF_.at("pho_seedTime")->Get()->at(i)) < 3)) return false;
    if(!(objF_.at("phoSigmaIEtaIEta_2012")->Get()->at(i) < 0.5 && objF_.at("pho_trackIsoR3PtCut20")->Get()->at(i) > -500 && objF_.at("pho_trackIsoR3PtCut20")->Get()->at(i) < 500)) return false;
    // use Tight ID working points for PbPb 2018
    bool passID = false;
    const auto iso = objF_.at("pho_ecalClusterIsoR3")->Get()->at(i) + objF_.at("pho_hcalRechitIsoR3")->Get()->at(i) + objF_.at("pho_trackIsoR3PtCut20")->Get()->at(i);
    if (std::abs(objF_.at("phoSCEta")->Get()->at(i)) < 1.5) {
      passID = iso < 4.1708 && objF_.at("phoSigmaIEtaIEta_2012")->Get()->at(i) < 0.01072 && objF_.at("phoHoverE")->Get()->at(i) < 0.1609;
    }
    else {
      passID = iso < 8.589 && objF_.at("phoSigmaIEtaIEta_2012")->Get()->at(i) < 0.0440 && objF_.at("phoHoverE")->Get()->at(i) < 0.324;
    }
    return passID;
  };

  bool passMuonCut(const size_t& i, const bool& useTightID=false) const
  {
    // check kinematics
    const auto& eta = objF_.at("muEta")->Get()->at(i);
    const auto& pt = objF_.at("muPt")->Get()->at(i);
    const bool inAcc = ((std::abs(eta)<1.2 && pt>=3.5) ||
                        (1.2<=std::abs(eta) && abs(eta)<2.1 && pt>=5.47-1.89*std::abs(eta)) ||
                        (2.1<=std::abs(eta) && std::abs(eta)<2.4 && pt>=1.5));
    if (!inAcc) return false;
    // use Tight ID working points for PbPb 2018 (if requested)
    if (useTightID) return objI_.at("muIDTight")->Get()->at(i);
    // use Hybrid Soft ID working points for PbPb 2018
    const bool passID = objI_.at("muIsGlobal")->Get()->at(i) && objI_.at("muIsTracker")->Get()->at(i) && objI_.at("muTrkLayers")->Get()->at(i) > 5 &&
                        objI_.at("muPixelLayers")->Get()->at(i) > 0 && std::abs(objF_.at("muD0")->Get()->at(i)) < 0.3 && std::abs(objF_.at("muDz")->Get()->at(i)) < 20.0;
    return passID;
  };

  void initEventInfo()
  {
    evtUI_["runN"].reset(new TTreeReaderValue<UInt_t>(*reader_.at("hiEvtAnalyzer"), "run"));
    evtUL_["eventN"].reset(new TTreeReaderValue<ULong64_t>(*reader_.at("hiEvtAnalyzer"), "evt"));
    evtI_["hiBin"].reset(new TTreeReaderValue<int>(*reader_.at("hiEvtAnalyzer"), "hiBin"));
    skimI_["evtSel"].reset(new TTreeReaderValue<int>(*reader_.at("skimanalysis"), "collisionEventSelectionAODv2"));
  };

  void initElectron()
  {
    for (const auto& var : {"eleCharge", "eleMissHits"}) {
      objI_[var].reset(new TTreeReaderValue<std::vector<int> >(*reader_.at("ggHiNtuplizerGED"), var));
    }
    for (const auto& var : {"elePt", "eleSCEta", "eleSCPhi", "eleSigmaIEtaIEta_2012", "eledEtaSeedAtVtx", "eledPhiAtVtx", "eleEoverPInv", "eleHoverEBc", "eleIP3D"}) {
      objF_[var].reset(new TTreeReaderValue<std::vector<float> >(*reader_.at("ggHiNtuplizerGED"), var));
    }
  };

  void initPhoton()
  {
    for (const auto& var : {"phoEt", "phoSCEta", "phoSCPhi", "pho_ecalClusterIsoR3", "pho_hcalRechitIsoR3", "pho_trackIsoR3PtCut20", "phoSigmaIEtaIEta_2012", "phoHoverE", "pho_swissCrx", "pho_seedTime"}) {
      objF_[var].reset(new TTreeReaderValue<std::vector<float> >(*reader_.at("ggHiNtuplizerGED"), var));
    }
  };

  void initMuon()
  {
    for (const auto& var : {"muCharge", "muIsGlobal", "muIsTracker", "muIDTight", "muTrkLayers", "muPixelLayers"}) {
      objI_[var].reset(new TTreeReaderValue<std::vector<int> >(*reader_.at("ggHiNtuplizerGED"), var));
    };
    for (const auto& var : {"muPt", "muEta", "muPhi", "muD0", "muDz"}) {
      objF_[var].reset(new TTreeReaderValue<std::vector<float> >(*reader_.at("ggHiNtuplizerGED"), var));
    }
  };
  
  std::map<std::string, std::unique_ptr<TTreeReaderValue<UInt_t> > > evtUI_;
  std::map<std::string, std::unique_ptr<TTreeReaderValue<ULong64_t> > > evtUL_;
  std::map<std::string, std::unique_ptr<TTreeReaderValue<int> > > evtI_, skimI_;
  std::map<std::string, std::unique_ptr<TTreeReaderValue<std::vector<int> > > > objI_;
  std::map<std::string, std::unique_ptr<TTreeReaderValue<std::vector<float> > > > objF_;
};


void formatEff(TGraphAsymmErrors& graph, const std::string& par, const std::string& var)
{
  // add particle label
  std::string parLbl;
  if      (par=="SingleMuon") { parLbl = "Muon"; }
  else if (par=="DoubleMuon") { parLbl = "Dimuon"; }
  else if (par=="SingleElectron") { parLbl = "Electron"; }
  else if (par=="DoubleElectron") { parLbl = "Dielectron"; }
  else if (par=="SinglePhoton") { parLbl = "Photon"; }
  // X-axis label
  std::string xLabel = parLbl;
  if      (var=="Pt") { xLabel += " p_{T} [GeV/c]"; }
  else if (var=="Eta") { xLabel += " #eta"; }
  else if (var=="Rapidity") { xLabel += " y"; }
  // Y-axis label
  const std::string yLabel = "Efficiency";
  // General
  graph.SetMarkerColor(kBlue);
  graph.SetMarkerStyle(20);
  graph.SetMarkerSize(1.5);
  graph.SetFillStyle(1001);
  // X-axis
  graph.GetXaxis()->CenterTitle(kTRUE);
  graph.GetXaxis()->SetTitleOffset(0.90);
  graph.GetXaxis()->SetTitleSize(0.055);
  graph.GetXaxis()->SetLabelSize(0.035);
  // Y-axis
  graph.GetYaxis()->CenterTitle(kTRUE);
  graph.GetYaxis()->SetTitleOffset(1.05);
  graph.GetYaxis()->SetTitleSize(0.055);
  graph.GetYaxis()->SetLabelSize(0.035);
  graph.GetYaxis()->SetRangeUser(0.0, 1.4);
  // Set Axis Titles
  graph.SetTitle(Form(";%s;%s", xLabel.c_str(), yLabel.c_str()));
  gPad->Update();
};


bool existDir(const std::string& dir)
{
  bool exist = false;
  const auto& dirp = gSystem->OpenDirectory(dir.c_str());
  if (dirp) { gSystem->FreeDirectory(dirp); exist = true; }
  return exist;
};


void makeDir(const std::string& dir)
{
  if (existDir(dir)==false){
    std::cout << "[INFO] DataSet directory: " << dir << " doesn't exist, will create it!" << std::endl;  
    gSystem->mkdir(dir.c_str(), kTRUE);
  }
};


const std::map<std::string, std::map<std::string, std::vector<double> > > BINMAP = {
    { "SingleMuon" , 
                      {
                        { "Pt"  , { 0, 1, 2, 3, 4, 5, 6.5, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 21, 25, 30, 35, 40, 50 } },
                        { "Eta" , { -2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4 } }
                      }
    },
    { "DoubleMuon" , 
                      { 
                        { "Pt"  , { 0, 1, 2, 3, 4, 5, 6.5, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 21, 25, 30, 35, 40, 50 } },
                        { "Rapidity" , { -2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4 } }
                      }
    },
    { "SingleElectron" , 
                      { 
                        { "Pt"  , {5,7,10,13,16,18, 20, 21, 22, 25, 30, 35, 40, 50, 60, 100 } },
                        { "Eta" , { -2.1, -1.6, -1.2, -0.8, -0.4, 0.4, 0.8, 1.2, 1.6, 2.1 } }
                      }
    },
    { "DoubleElectron" ,
                      {
                        { "Pt"  , { 0, 1, 2, 3, 4, 5, 6.5, 7, 8, 9, 10, 11, 12, 13, 15, 17, 19, 21, 25, 30, 35, 40, 50 } },
                        { "Rapidity" , { -2.1, -1.6, -1.2, -0.8, -0.4, 0.4, 0.8, 1.2, 1.6, 2.1 } }
                      }
    },
    { "SinglePhoton" ,
                      {
                        { "Pt"  , {10,15,20,25,30,35, 40, 41, 42, 45, 50, 55, 60, 65, 70, 80, 100, 200 } },
                        { "Eta" , { -2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4 } }
                      }
    }
};


const std::map< std::string , std::vector<std::string> > TRIGLIST = {
    { "SingleMuon" , 
                      {  
                        "HLT_HIL1MuOpen_Centrality_70_100_v1",
                        "HLT_HIL1MuOpen_Centrality_80_100_v1",
                        "HLT_HIL2Mu3_NHitQ15_v1",
                        "HLT_HIL2Mu5_NHitQ15_v1",
                        "HLT_HIL2Mu7_NHitQ15_v1",
                        "HLT_HIL3Mu12_v1",
                        "HLT_HIL3Mu15_v1",
                        "HLT_HIL3Mu20_v1",
                        "HLT_HIL3Mu3_NHitQ10_v1",
                        "HLT_HIL3Mu5_NHitQ10_v1",
                        "HLT_HIL3Mu7_NHitQ10_v1"
                     }
    },
    { "DoubleMuon" , 
                     {
                        "HLT_HIL1DoubleMu10_v1",
                        "HLT_HIL1DoubleMuOpen_v1",
                        "HLT_HIL2DoubleMuOpen_v1",
                        "HLT_HIL2_L1DoubleMu10_v1",
                        "HLT_HIL3DoubleMuOpen_M60120_v1",
                        "HLT_HIL3DoubleMuOpen_Upsi_v1",
                        "HLT_HIL3DoubleMuOpen_v1",
                        "HLT_HIL3Mu0_L2Mu0_v1",
                        "HLT_HIL3Mu2p5NHitQ10_L2Mu2_M7toinf_v1",
                        "HLT_HIL3_L1DoubleMu10_v1",
                        "HLT_HIL1DoubleMuOpen_Centrality_50_100_v1",
                        "HLT_HIL1DoubleMuOpen_OS_Centrality_40_100_v1",
                        "HLT_HIL3DoubleMuOpen_JpsiPsi_v1",
                        "HLT_HIL3Mu0NHitQ10_L2Mu0_MAXdR3p5_M1to5_v1" 
                     }
    },
    { "SingleElectron" , 
                     {
                        "HLT_HIEle10Gsf_v1",
                        "HLT_HIEle15Gsf_v1",
                        "HLT_HIEle20Gsf_v1",
                        "HLT_HIEle30Gsf_v1",
                        "HLT_HIEle40Gsf_v1",
                        "HLT_HIEle50Gsf_v1"
                      }
    },
    { "DoubleElectron" , 
                     {
                        "HLT_HIDoubleEle10GsfMass50_v1",
                        "HLT_HIDoubleEle10Gsf_v1",
                        "HLT_HIDoubleEle15GsfMass50_v1",
                        "HLT_HIDoubleEle15Gsf_v1",
                        "HLT_HIEle15Ele10GsfMass50_v1",
                        "HLT_HIEle15Ele10Gsf_v1"
                     }
    },
    { "SinglePhoton" ,
                     {
                        "HLT_HIGEDPhoton30_EB_v1",
                        "HLT_HIGEDPhoton30_v1",
                        "HLT_HIGEDPhoton40_EB_v1",
                        "HLT_HIGEDPhoton40_v1",
                        "HLT_HIGEDPhoton50_EB_v1",
                        "HLT_HIGEDPhoton50_v1",
                        "HLT_HIGEDPhoton60_EB_v1",
                        "HLT_HIGEDPhoton60_v1",
                        "HLT_HIGEDPhoton10_EB_v1",
                        "HLT_HIGEDPhoton10_v1",
                        "HLT_HIGEDPhoton20_EB_v1",
                        "HLT_HIGEDPhoton20_v1"
                     }
    }

};

    const std::map<std::string,  std::vector<double> > hiBin = {{"hiBin", {0, 10, 20, 30, 40 ,50 ,60 ,70 ,80 ,90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}}
    };

#endif // ifndef Cent_plotTurnOn_h
