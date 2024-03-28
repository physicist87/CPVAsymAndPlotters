#include "tdrStyle.C"
#include "CMS_lumi.C"
void CPDraw()
{
/*
TStyle myStyle("HHStyle","HHStyle");
  setHHStyle(myStyle);
  myStyle.SetStripDecimals(true);
  myStyle.cd();
  gROOT->SetStyle("HHStyle");
  gROOT->ForceStyle();
  TGaxis::SetMaxDigits(2);*/
  setTDRStyle();
  writeExtraText = true;      
lumi_sqrtS = "#sqrt{s} = 13 TeV";       // used with iPeriod = 0, e.g. for simulation-only plots (default is an empty string)
  int iPeriod = 0;    // 1=7TeV, 2=8TeV, 3=7+8TeV, 7=7+8+13TeV, 0=free form (uses lumi_sqrtS)  
  cmsTextSize = 0.3;
  lumiTextSize = 0.3; 
  // Get Histogram //
  TFile *f = TFile::Open(Form("/storage/palgongsan/sha/Analysis/CPViolation/ForTop2018/METNoPhiCor_Check_v2/RootFiles/METAddX_1_v2/MuEl/TTJets_Signal.root"));
  TString HistName = "h_Reco_CPO1_ReRange";
  TH1F* h1 = f->Get(HistName.Data()); 
  TCanvas* c1 = new TCanvas("c1","c1",600,600);
//  TCanvas* c1 = new TCanvas("c1", "c1", 800, 800);
  //TCanvas* c1 = new TCanvas("c1","c1");
  c1->cd();
  CMS_lumi( c1, 0, 33);
  h1->GetYaxis()->SetTitle("Enetries");
  h1->Draw();
}
