#include "TPad.h"
#include "TLatex.h"
#include "TLine.h"
#include "TBox.h"
#include "TASImage.h"

//
// Global variables
//

TString cmsText     = "CMS";
float cmsTextFont   = 61;  // default is helvetic-bold

bool writeExtraText = false;
TString extraText   = "Internal";
//TString extraText   = "Preliminary";
float extraTextFont = 52;  // default is helvetica-italics

// text sizes and text offsets with respect to the top frame
// in unit of the top margin size
//float lumiTextSize     = 0.6;
float lumiTextSize     = 0.62*1.5; // 
float lumiTextOffset   = 0.22;
//float cmsTextSize      = 0.75;
float cmsTextSize      = 0.75*1.6; // 
float cmsTextOffset    = 0.1;  // only used in outOfFrame version

float relPosX    = 0.045;
float relPosY    = 0.035;
float relExtraDY = 1.2;

// ratio of "CMS" and extra text size
float extraOverCmsTextSize  = 0.76;

TString lumi_pp502TeV  = "28.0 pb^{-1}";
TString lumi_pPb502TeV  = "34.6 nb^{-1}";
TString lumi_PbPb502TeV  = "1.7 nb^{-1}";
TString lumi_PbPbInternalData  = "5.02 TeV (DATA)";
TString lumi_PbPbInternalMC  = "5.02 TeV (MC)";
TString lumi_PbPb502TeV_projected  = "10 nb^{-1} projection";
TString lumi_sqrtS = "";

bool drawLogo      = false;
