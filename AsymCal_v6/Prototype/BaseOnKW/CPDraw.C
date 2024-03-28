#include "tdrstyle.C"
#include "CMS_lumi.C"
//#include "TAttAxis.h"
void CPDraw()
{
  // Get Histogram //
  TFile *f = TFile::Open(Form("/storage/palgongsan/sha/Analysis/CPViolation/ForTop2018/RootFiles/SystematicStudy_v5/MuEl/TTJets_Signal.root"));
  cout << "f : " << f->GetName() << endl;
   //TString HistName = "h_Reco_CPO1_ReRange";
  TString HistName = "h_Toppt";
  TH1F* h1 = f->Get(HistName.Data());
  h1->Rebin(10); 
  TH1F* h2 = h1->Clone("newHist");
  cout << "h1 Integral: " << h1->Integral() << endl; 
  /// X-axis()
  //h1->Draw();
  h1->GetXaxis()->SetTitle("CPO1");
 // h1->GetXaxis()->SetTitleOffset(1.1);
  h1->GetXaxis()->SetTitleSize(0.05);
  h1->GetXaxis()->SetLabelSize(0.03);
  /// Y-axis()
  h1->GetYaxis()->SetTitle("Enetries");
  //h1->GetYaxis()->SetTitleOffset(1.15);
  h1->GetYaxis()->SetTitleOffset(1.3);
  //h1->GetYaxis()->SetLabelSize(0.04);
  h1->GetYaxis()->SetLabelSize(0.03);
  h1->GetYaxis()->SetTitleSize(0.05);
  //h1->GetYaxis()->SetMaxDigits(3);


    writeExtraText = true;
    //extraText = "Simulation Preliminary";
    drawLogo = false;
    setTDRStyle();

    lumiTextSize = 0.4;
    cmsTextSize = 0.6;

	int W = 1200;
	int H = 1200;
	int H_ref = 1200;
	int W_ref = 1200;

	// references for T, B, L, R
	float T = 0.08*H_ref;// origing 0.08
	float B = 0.12*H_ref;// 0.12
	float L = 0.12*W_ref;// 0.12
	float R = 0.04*W_ref;// 0.04

    TCanvas* canv = new TCanvas("canv","",800,800);  
    canv->SetFillColor(0);
        cout << "L/W" << L/W<< endl;
	canv->SetLeftMargin(L/W +0.01);
	canv->SetRightMargin(R/W);
	canv->SetTopMargin(T/H);
	canv->SetBottomMargin(B/H);
	h1->Draw("Hist");
        h2->SetFillStyle(3004);
        h2->SetFillColor(13);
        h2->SetLineColor(13);
        h2->SetLineWidth(1);
        h2->SetMarkerColor(13);
//`        h2->SetMarkerStyle(25);
        h2->SetMarkerSize(0);
        h2->Draw("E2SAME");


	//CMS_lumi(canv,5,11);
	//CMS_lumi(canv,4,10);
	CMS_lumi(canv,4,0);
	canv->Update();
	//canv->SetStripDecimals();
	canv->RedrawAxis();
        //TGaxis::SetMaxDigits(3);
	canv->GetFrame()->Draw();
//    canv->Print("Test.pdf");
//	canv->SetLogy();
//    canv->Print("sigs/"+channel+".pdf");



}
