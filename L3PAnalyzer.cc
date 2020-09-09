//system include files
#include <memory>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/TrackReco/interface/TrackToTrackMap.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoChargedCandidateFwd.h"

#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"


// class decclaration
class L3PAnalyzer : public edm::EDAnalyzer {
public:
  explicit L3PAnalyzer(const edm::ParameterSet&);
  ~L3PAnalyzer() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void beginJob() override;
  void endJob() override;
  void beginRun(const edm::Run& r, const edm::EventSetup& iSetup) override;
  void endRun(const edm::Run& r, const edm::EventSetup& iSetup) override;

private:

  edm::Service<TFileService> fs_;
  double timeCut_;
  TTree* myTree_;

  static const int Max_mu_size = 10000;

  template <class T>
  void fillArray(TClonesArray* arr, const T& col ){
    int size =0;
    TLorentzVector vMuon;
    if(col.isValid()){ 
        for( auto  idx = col->begin(); idx != col->end(); idx++){
           auto pt = idx->pt();
           auto eta = idx->eta();
           auto phi = idx->phi();
        vMuon.SetPtEtaPhiM(pt, eta, phi ,0.1056583745);
        new((*arr)[size++])TLorentzVector(vMuon);
      }
    }
  }

  int  irun;
  unsigned long long ievt;
  double ipt, ieta, iphi;
  TClonesArray* TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA           ; 
  TClonesArray* TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA      ; 
  TClonesArray* TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA      ; 
  TClonesArray* TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA      ; 
  TClonesArray* TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA; 
  TClonesArray* TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA; 
  TClonesArray* TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA; 

  TClonesArray* L3MuonProducer_hltL3MuonsIterL3OIPPOnAA;
  TClonesArray* L3MuonProducer_hltL3MuonsIterL3IOPPOnAA;
  TClonesArray* L3MuonProducer_hltIterL3GlbMuonPPOnAA  ;

  TClonesArray* L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA      ;
  TClonesArray* L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA ;

  TClonesArray* ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA;

  edm::EDGetTokenT<reco::TrackCollection> token_hltIterL3OIMuCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_;

  edm::EDGetTokenT<reco::TrackCollection> token_hltL3MuonsIterL3OIPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltL3MuonsIterL3IOPPOnAA_;
  edm::EDGetTokenT<reco::TrackCollection> token_hltIterL3GlbMuonPPOnAA_;

  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> token_hltIterL3OIL3MuonCandidatesPPOnAA_;
  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> token_hltIterL3MuonCandidatesPPOnAA_;

  edm::EDGetTokenT<reco::RecoChargedCandidateCollection> token_hltIter3IterL3MuonL2CandidatesPPOnAA_;

};

L3PAnalyzer::L3PAnalyzer(const edm::ParameterSet& iConfig) {

  TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA            = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA       = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA       = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA       = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA = new TClonesArray("TLorentzVector", Max_mu_size);
  TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA = new TClonesArray("TLorentzVector", Max_mu_size);

  L3MuonProducer_hltL3MuonsIterL3OIPPOnAA= new TClonesArray("TLorentzVector", Max_mu_size);
  L3MuonProducer_hltL3MuonsIterL3IOPPOnAA= new TClonesArray("TLorentzVector", Max_mu_size);
  L3MuonProducer_hltIterL3GlbMuonPPOnAA  = new TClonesArray("TLorentzVector", Max_mu_size);

  L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA      = new TClonesArray("TLorentzVector", Max_mu_size);
  L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA = new TClonesArray("TLorentzVector", Max_mu_size);

  ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA = new TClonesArray("TLorentzVector", Max_mu_size);



  token_hltIterL3OIMuCtfWithMaterialTracksPPOnAA_ = consumes<reco::TrackCollection>(edm::InputTag("hltIterL3OIMuCtfWithMaterialTracksPPOnAA"));
  token_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA"));
  token_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA"));
  token_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA"));
  token_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA"));
  token_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA"));
  token_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA"));

  token_hltL3MuonsIterL3OIPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltL3MuonsIterL3OIPPOnAA"));
  token_hltL3MuonsIterL3IOPPOnAA_= consumes<reco::TrackCollection>(edm::InputTag("hltL3MuonsIterL3IOPPOnAA"));
  token_hltIterL3GlbMuonPPOnAA_  = consumes<reco::TrackCollection>(edm::InputTag("hltIterL3GlbMuonPPOnAA"));

  token_hltIterL3OIL3MuonCandidatesPPOnAA_  = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("hltIterL3OIL3MuonCandidatesPPOnAA"));
  token_hltIterL3MuonCandidatesPPOnAA_      = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("hltIterL3MuonCandidatesPPOnAA"));

  token_hltIter3IterL3MuonL2CandidatesPPOnAA_ = consumes<reco::RecoChargedCandidateCollection>(edm::InputTag("hltIter3IterL3MuonL2CandidatesPPOnAA"));
}

L3PAnalyzer::~L3PAnalyzer(){
TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA->Clear()           ; 
TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA->Clear()      ; 
TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA->Clear()      ;
TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA->Clear()      ;
TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA->Clear();
TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA->Clear();
TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA->Clear();
L3MuonProducer_hltL3MuonsIterL3OIPPOnAA->Clear();
L3MuonProducer_hltL3MuonsIterL3IOPPOnAA->Clear();
L3MuonProducer_hltIterL3GlbMuonPPOnAA->Clear()  ;
L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA->Clear();
L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA->Clear();
ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA->Clear();

} 

void L3PAnalyzer::beginRun(const edm::Run& r, const edm::EventSetup& iSetup) {}


void L3PAnalyzer::endRun(const edm::Run& r, const edm::EventSetup& iSetup) {
return;}

void L3PAnalyzer::beginJob(){
  myTree_ = fs_->make<TTree>("L3Track", "L3Track Tree");
  myTree_->Branch("Run", &irun, "Run/I");
  myTree_->Branch("Event", &ievt, "Event/I");
  myTree_->Branch("TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA, 32000, 0);
  myTree_->Branch("TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA","TClonesArray" , &TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA, 32000, 0);

  myTree_->Branch("L3MuonProducer_hltL3MuonsIterL3OIPPOnAA","TClonesArray" , &L3MuonProducer_hltL3MuonsIterL3OIPPOnAA, 32000, 0);
  myTree_->Branch("L3MuonProducer_hltL3MuonsIterL3IOPPOnAA","TClonesArray" , &L3MuonProducer_hltL3MuonsIterL3IOPPOnAA, 32000, 0);
  myTree_->Branch("L3MuonProducer_hltIterL3GlbMuonPPOnAA","TClonesArray" , &L3MuonProducer_hltIterL3GlbMuonPPOnAA  , 32000, 0);

  myTree_->Branch("L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA","TClonesArray" , &L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA , 32000, 0);
  myTree_->Branch("L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA"   ,"TClonesArray" , &L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA, 32000, 0);

  myTree_->Branch("ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA"   ,"TClonesArray" , &ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA, 32000, 0);

  std::cout << "Begin Job" << std::endl;
}

void L3PAnalyzer::endJob() {
 std::cout << "End Event"<< std::endl;
 edm::LogInfo("AnalyzerMu") << "next event";
}
void L3PAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup&) {
  edm::LogInfo("L3MuAnalzer") << " Start L3PAnalyzer::analyze " << iEvent.id().run() << ":" << iEvent.id().event();
  ievt = iEvent.id().event();
  irun = iEvent.id().run();
  std::cout << "Run/Event : " << irun << "/" << ievt << std::endl;
  edm::Handle<reco::TrackCollection> hltIterL3OIMuCtfWithMaterialTracksPPOnAA;
  edm::Handle<reco::TrackCollection> hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA       ;
  edm::Handle<reco::TrackCollection> hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA       ;
  edm::Handle<reco::TrackCollection> hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA       ;
  edm::Handle<reco::TrackCollection> hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA ;
  edm::Handle<reco::TrackCollection> hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA ;
  edm::Handle<reco::TrackCollection> hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA ;
  edm::Handle<reco::TrackCollection> hltL3MuonsIterL3OIPPOnAA;
  edm::Handle<reco::TrackCollection> hltL3MuonsIterL3IOPPOnAA;
  edm::Handle<reco::TrackCollection> hltIterL3GlbMuonPPOnAA  ;
  edm::Handle<reco::RecoChargedCandidateCollection> hltIterL3OIL3MuonCandidatesPPOnAA;
  edm::Handle<reco::RecoChargedCandidateCollection> hltIterL3MuonCandidatesPPOnAA;
  edm::Handle<reco::RecoChargedCandidateCollection> hltIter3IterL3MuonL2CandidatesPPOnAA; 

  iEvent.getByToken(token_hltIterL3OIMuCtfWithMaterialTracksPPOnAA_, hltIterL3OIMuCtfWithMaterialTracksPPOnAA);
  iEvent.getByToken(token_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA_      , hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA      );
  iEvent.getByToken(token_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA_      , hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA      );
  iEvent.getByToken(token_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA_      , hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA      );
  iEvent.getByToken(token_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_, hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);
  iEvent.getByToken(token_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_, hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);
  iEvent.getByToken(token_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA_, hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);

  iEvent.getByToken(token_hltL3MuonsIterL3OIPPOnAA_, hltL3MuonsIterL3OIPPOnAA);
  iEvent.getByToken(token_hltL3MuonsIterL3IOPPOnAA_, hltL3MuonsIterL3IOPPOnAA);
  iEvent.getByToken(token_hltIterL3GlbMuonPPOnAA_, hltIterL3GlbMuonPPOnAA  );

  iEvent.getByToken(token_hltIterL3OIL3MuonCandidatesPPOnAA_, hltIterL3OIL3MuonCandidatesPPOnAA);
  iEvent.getByToken(token_hltIterL3MuonCandidatesPPOnAA_, hltIterL3MuonCandidatesPPOnAA);

  iEvent.getByToken(token_hltIter3IterL3MuonL2CandidatesPPOnAA_, hltIter3IterL3MuonL2CandidatesPPOnAA);

  fillArray<edm::Handle<reco::TrackCollection>>( TrackProducer_hltIterL3OIMuCtfWithMaterialTracksPPOnAA           ,hltIterL3OIMuCtfWithMaterialTracksPPOnAA           );
  fillArray<edm::Handle<reco::TrackCollection>>( TrackProducer_hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA      ,hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA      );
  fillArray<edm::Handle<reco::TrackCollection>>(TrackProducer_hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA      ,hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA      );
  fillArray<edm::Handle<reco::TrackCollection>>(TrackProducer_hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA      ,hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA      );
  fillArray<edm::Handle<reco::TrackCollection>>(TrackProducer_hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA,hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);
  fillArray<edm::Handle<reco::TrackCollection>>(TrackProducer_hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA,hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);
  fillArray<edm::Handle<reco::TrackCollection>>(TrackProducer_hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA,hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA);
  fillArray<edm::Handle<reco::TrackCollection>>(L3MuonProducer_hltL3MuonsIterL3OIPPOnAA, hltL3MuonsIterL3OIPPOnAA);
  fillArray<edm::Handle<reco::TrackCollection>>(L3MuonProducer_hltL3MuonsIterL3IOPPOnAA, hltL3MuonsIterL3IOPPOnAA);
  fillArray<edm::Handle<reco::TrackCollection>>(L3MuonProducer_hltIterL3GlbMuonPPOnAA, hltIterL3GlbMuonPPOnAA);
  fillArray<edm::Handle<reco::RecoChargedCandidateCollection>>(L3MuonCandidateProducer_hltIterL3OIL3MuonCandidatesPPOnAA, hltIterL3OIL3MuonCandidatesPPOnAA);
  fillArray<edm::Handle<reco::RecoChargedCandidateCollection>>(L3MuonCandidateProducerFromMuons_hltIterL3MuonCandidatesPPOnAA, hltIterL3MuonCandidatesPPOnAA);
  fillArray<edm::Handle<reco::RecoChargedCandidateCollection>>( ConcreteChargedCandidateProducer_hltIter3IterL3MuonL2CandidatesPPOnAA, hltIter3IterL3MuonL2CandidatesPPOnAA);
  myTree_->Fill();
  }


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(L3PAnalyzer);
