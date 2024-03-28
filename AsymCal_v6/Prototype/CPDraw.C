//#include "tdrstyle.C"
//#include "CMS_lumi.C"
#include "HHStyle.h"
void CPDraw()
{
TStyle myStyle("HHStyle","HHStyle");
  setHHStyle(myStyle);
  myStyle.SetStripDecimals(true);
  myStyle.cd();
  gROOT->SetStyle("HHStyle");
  gROOT->ForceStyle();
  TGaxis::SetMaxDigits(2);
//  setTDRStyle();
  // Get Histogram //
  TFile *f = TFile::Open(Form("/storage/palgongsan/sha/Analysis/CPViolation/ForTop2018/METNoPhiCor_Check_v2/RootFiles/METAddX_1_v2/MuEl/TTJets_Signal.root"));
  TString HistName = "h_Reco_CPO1_ReRange";
  TH1F* h1 = f->Get(HistName.Data()); 
  TCanvas* c1 = new TCanvas("c1","c1",600,600);
  //TCanvas* c1 = new TCanvas("c1","c1");
  //c1->cd();
  h1->GetYaxis()->SetTitle("Enetries");
  h1->Draw();
  //c1->Update();
  c1->Print("TEST.C");
}
