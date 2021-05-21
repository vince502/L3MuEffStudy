#ifndef UTIL_H
#define UTIL_H

class OniaTreeSetup
{
	public :
		OniaTreeSetup(std::string path, std::string _reco){
		 		recFile = new TFile(Form("%s/%s",path.c_str(), _reco.c_str()),"READ");
				myTree = (TTree*) recFile->Get("hionia/myTree"); 
				isSet=false;
		}
		~OniaTreeSetup(){
		  if (recFile && recFile->IsOpen()) recFile->Close();
		  delete RTC;
		}
		void SetBranch(){
		  if (isSet) std::cout << "branch already set" << std::endl;
			myTree->SetBranchAddress("eventNb", &rEvent);
			myTree->SetBranchAddress("SumET_HF", &sumhf);
			myTree->SetBranchAddress("Reco_mu_size", &recMsize);
			myTree->SetBranchAddress("Reco_mu_nTrkWMea", &Reco_mu_nTrkWMea);
			myTree->SetBranchAddress("Reco_mu_nPixWMea", &Reco_mu_nPixWMea);
			myTree->SetBranchAddress("Reco_mu_dxy", &Reco_mu_dxy);
			myTree->SetBranchAddress("Reco_mu_dz", &Reco_mu_dz);
			myTree->SetBranchAddress("Reco_mu_4mom", &RTC);
			myTree->BuildIndex("eventNb");
			isSet =true;
		}

		const static int MaxMsize = 32000;
		TTree* myTree;
		UInt_t rEvent;
		float sumhf;
		short recMsize;
		TClonesArray* RTC = new TClonesArray("TLorentzVector", MaxMsize);
		int Reco_mu_nTrkWMea[MaxMsize];
		int Reco_mu_nPixWMea[MaxMsize];
		Float_t Reco_mu_dxy[MaxMsize];
		Float_t Reco_mu_dz[MaxMsize];

	private :
		TFile* recFile;
		bool isSet;
};

const static int nBins = 200;
const Double_t binTable[nBins+1] = {0, 12.6154, 14.3128, 16.115, 18.5682, 21.0883, 25.6869, 31.5111, 40.4269, 53.7158, 66.7999, 81.6276, 100.711, 134.029, 165.93, 198.003, 235.598, 283.83, 336.152, 392.058, 476.871, 566.009, 649.72, 736.959, 879.442, 1034.8, 1144.73, 1282.25, 1451.68, 1654.33, 1890.08, 2121.35, 2408.87, 2631.04, 2747.91, 2824.36, 2888.82, 2944.63, 2994.4, 3041.05, 3089.93, 3126.84, 3161.49, 3200.3, 3231.53, 3269.93, 3299.44, 3325.75, 3354.55, 3382.95, 3407.03, 3430.12, 3452.54, 3476.03, 3496.3, 3519.8, 3542.55, 3567.37, 3584.16, 3600.77, 3617.16, 3630.67, 3646.4, 3661.07, 3677.17, 3689.26, 3702.82, 3714.15, 3727.59, 3738.71, 3749.23, 3759.73, 3770.62, 3781.64, 3792.37, 3804.8, 3814.97, 3824.54, 3833.84, 3843.46, 3852.01, 3860.39, 3868.69, 3877.93, 3886.53, 3894.82, 3904.95, 3913.39, 3920.59, 3927.87, 3934.02, 3942.46, 3950.42, 3957.77, 3964.32, 3971.64, 3978.3, 3985.49, 3992.52, 3998.31, 4004.75, 4010.66, 4017.73, 4024.3, 4030.92, 4036.84, 4042.74, 4049.35, 4056.4, 4062.71, 4069.23, 4076.12, 4082.27, 4088.06, 4094.03, 4099.98, 4106.53, 4111.43, 4116.94, 4123.49, 4129.32, 4135.78, 4141.63, 4149.89, 4155.92, 4161.24, 4166.99, 4173.35, 4179.97, 4185.08, 4191.78, 4198.12, 4203.59, 4210.12, 4216.41, 4222.65, 4228.37, 4234.69, 4240.24, 4246.87, 4252.96, 4259.25, 4265.87, 4272.51, 4278.11, 4283.83, 4289.46, 4295.06, 4302.51, 4308.35, 4315.93, 4322.89, 4328.46, 4335.06, 4342.45, 4348.98, 4356.3, 4363.05, 4370.66, 4377.16, 4383.1, 4390.72, 4398.56, 4406.34, 4415.54, 4423.18, 4431.46, 4438.86, 4446.95, 4454.53, 4462.79, 4471.14, 4481.57, 4490.8, 4499.03, 4509.26, 4518.58, 4528.65, 4538.44, 4548.63, 4560.74, 4571.77, 4584.29, 4596.24, 4608.89, 4621.58, 4636.82, 4649.66, 4664.91, 4683.41, 4700.78, 4718.48, 4736.48, 4755.55, 4781.63, 4812.75, 4848.5, 4892.33, 4951.48, 5025.7, 5492.07};

Int_t getHiBinFromhiHF(const Double_t hiHF)
{
  Int_t binPos = -1;
  for(int i = 0; i < nBins; ++i){
    if(hiHF >= binTable[i] && hiHF < binTable[i+1]){
      binPos = i;
      break;
    }
  }

  binPos = nBins - 1 - binPos;

  return (Int_t)(200*((Double_t)binPos)/((Double_t)nBins));
};

std::map< int,  std::pair< std::string, std::string > > ProdList{
  {1, {"hltIterL3OIMuCtfWithMaterialTracksPPOnAA",		"TrackProducer"}},
  {2, {"hltL3MuonsIterL3OIPPOnAA"					,"L3MuonProducer"}},
  {3, {"hltIterL3OIL3MuonCandidatesPPOnAA"				,"L3MuonCandidateProducer"}},
  {4, {"hltIter0IterL3MuonCtfWithMaterialTracksPPOnAA" 		,"TrackProducer"}},
  {5, {"hltIter2IterL3MuonCtfWithMaterialTracksPPOnAA" 		,"TrackProducer"}},
  {6, {"hltIter3IterL3MuonCtfWithMaterialTracksPPOnAA" 		,"TrackProducer"}},
  {7, {"hltL3MuonsIterL3IOPPOnAA" 					,"L3MuonProducer"}},
  {8, {"hltIterL3GlbMuonPPOnAA" 					,"L3MuonProducer"}},
  {9, {"hltIterL3MuonCandidatesPPOnAA" 				,"L3MuonCandidateProducerFromMuons"}},
  {10,{"hltIter0IterL3FromL1MuonCtfWithMaterialTracksPPOnAA" 	,"TrackProducer"}}, 
  {11,{"hltIter2IterL3FromL1MuonCtfWithMaterialTracksPPOnAA" 	,"TrackProducer"}}, 
  {12,{"hltIter3IterL3FromL1MuonCtfWithMaterialTracksPPOnAA" 	,"TrackProducer"}} 
};


bool HSoftID18_pass(int T, int P, Float_t Dxy, Float_t Dz){
  if( T > 5 && P > 0 && fabs(Dxy) < 0.3 && fabs(Dz) < 20.) return true;
  else return false;
};

bool accPass(double pt, double eta){
  if( fabs(eta) < 2.4 ){
    if( (fabs(eta) <1.2 && pt >3.5) || (fabs(eta)>1.2 && fabs(eta) <2.1 && pt > (5.47 - 1.89 * fabs(eta)) ) || (fabs(eta) > 2.1 && pt > 1.5) ) return true;
  }
  return false;
};

void dbg(int x=0){
  std::string indx = (x==0) ? "" : Form("%d",x);
  std::cout << "-------------------------_____DEBUG"<<indx<<"_____----------------------------" << std::endl;
}


#endif
