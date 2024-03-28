void ratio() {
   TH1F *h1 = new TH1F("h1","test1",100,-3,3);
   h1->FillRandom("gaus",200000);
   h1->GetXaxis()->SetLabelFont(63); //font in pixels
   h1->GetXaxis()->SetLabelSize(16); //in pixels
   h1->GetYaxis()->SetLabelFont(63); //font in pixels
   h1->GetYaxis()->SetLabelSize(16); //in pixels
   TH1F *h2 = new TH1F("h2","test2",100,-3,3);
   h2->FillRandom("gaus",100000);
   TCanvas *c1 = new TCanvas("c1","example",600,700);
   TPad *pad1 = new TPad("pad1","pad1",0,0.3,1,1);
   pad1->SetBottomMargin(0);
   pad1->Draw();
   pad1->cd();

   TH1F *h3=h1->DrawCopy();
   h3->SetMinimum(-100);
   h2->Draw("same");
   c1->cd();
   TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.3);
   pad2->SetTopMargin(0);
   pad2->Draw();
   pad2->cd();
   h1->Sumw2();
   h1->SetStats(0);
   h1->Divide(h2);
   h1->SetMarkerStyle(21);
   h1->Draw("ep");
   c1->cd();
}
