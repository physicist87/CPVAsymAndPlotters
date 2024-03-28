#include "tdrstyle.C"
#include "CMS_lumi.C"

void DrawSignificance(const TString& channel, const std::vector<double>& masses, const std::vector<double>& sigs)
{
	const int nMasses = masses.size();
	const double lumi = 35.9;

	// const double minMass = masses[0];
	const double minMass = 4250;
	const double maxMass = masses[nMasses-1];

	TGraph* expected = new TGraph(0);

	for(auto idx = 0; idx < nMasses; ++idx)
	{
		expected->SetPoint(idx, masses[idx], sigs[idx]);
	}

	expected->SetLineColor(1);
	expected->SetLineStyle(kDashed);
	expected->SetLineWidth(2);

	double maxLimit = 100.;
	double minLimit = 1e-1;

	TH2D* massframe = new TH2D("massframe", "", 10, minMass, maxMass, 20, minLimit, maxLimit);

	TString type = "";
	if (channel == "eeg") type = "e*";
	if (channel == "mmg") type = "#it{#mu}*";
	massframe->GetXaxis()->SetTitle("#it{M}_{" + type + "} (GeV)");
	massframe->GetXaxis()->SetTitleOffset(1.1);
	
	TString ytitle = "Expected significance";

	massframe->GetYaxis()->SetTitle(ytitle);
	massframe->GetYaxis()->SetTitleOffset(1.15);
    massframe->GetXaxis()->SetLabelSize(0.04);
    massframe->GetYaxis()->SetLabelSize(0.04);
    massframe->GetXaxis()->SetTitleSize(0.05);
    massframe->GetYaxis()->SetTitleSize(0.05);
	massframe->SetStats(kFALSE);
    massframe->GetXaxis()->SetMoreLogLabels();
    massframe->GetXaxis()->SetNoExponent(kTRUE);

    writeExtraText = true;
    extraText = "Simulation Preliminary";
    drawLogo = false;
    setTDRStyle();

    lumiTextSize = 0.4;
    cmsTextSize = 0.6;

	int W = 1200;
	int H = 1200;
	int H_ref = 1200;
	int W_ref = 1200;

	// references for T, B, L, R
	float T = 0.08*H_ref;
	float B = 0.12*H_ref;
	float L = 0.12*W_ref;
	float R = 0.04*W_ref;

    TCanvas* canv = new TCanvas("canv","",800,800);  
    canv->SetFillColor(0);
	canv->SetLeftMargin(L/W);
	canv->SetRightMargin(R/W);
	canv->SetTopMargin(T/H);
	canv->SetBottomMargin(B/H);
	massframe->Draw();

	expected->Draw("SAME");
	massframe->Draw("AXISSAME");

	TF1* sigma5 = new TF1("sigma5","5",-1000000,1000000);
	sigma5->SetLineColor(kRed);
	sigma5->Draw("SAME");

	TF1* sigma3 = new TF1("sigma3","3",-1000000,1000000);
	sigma3->SetLineColor(kBlue);
	sigma3->Draw("SAME");

	TPaveText* label_5sigma = new TPaveText(0.86,0.59,0.96,0.62, "brNDC");
    label_5sigma->SetBorderSize(0);
    label_5sigma->SetFillColor(0);
    label_5sigma->SetTextSize(0.03);
    label_5sigma->SetTextAlign(13);
    label_5sigma->SetTextFont(42);
    label_5sigma->SetTextColor(kRed);
    label_5sigma->AddText("5#sigma");
    label_5sigma->Draw("SAME");

	TPaveText* label_3sigma = new TPaveText(0.86,0.53,0.96,0.56, "brNDC");
    label_3sigma->SetBorderSize(0);
    label_3sigma->SetFillColor(0);
    label_3sigma->SetTextSize(0.03);
    label_3sigma->SetTextAlign(13);
    label_3sigma->SetTextFont(42);
    label_3sigma->SetTextColor(kBlue);
    label_3sigma->AddText("3#sigma");
    label_3sigma->Draw("SAME");

	CMS_lumi(canv,5,11);
	canv->Update();
	canv->RedrawAxis();
	canv->GetFrame()->Draw();
	canv->SetLogy();
    canv->Print("sigs/"+channel+".pdf");
}
