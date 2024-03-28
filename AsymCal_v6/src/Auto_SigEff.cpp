#define Auto_SigEff_cxx
#include "../include/Auto_SigEff.hpp"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;
void Auto_SigEff::SetChannel( TString ch )
{
   channel = ch;
   if (channel == "MuMu") {cout << "MuMu? " << endl; chan = "dimu";}
   else if (channel == "ElEl") {cout << "ElEl? " << endl; chan = "diel";}
   else if (channel == "MuEl") {cout << "MuEl? " << endl; chan = "muel";}
   else if (channel == "MuJet") {cout << "MuJet? " << endl; chan = "mujet";}
   else if (channel == "ElJet") {cout << "ElJet? " << endl; chan = "eljet";}
   else {cout << "Check out your channel!!!" << endl;}
}

void Auto_SigEff::SetSigConOutputDir( TString SigConDir )
{
   SaveSigConDir = Form("./%s",SigConDir.Data()  );
   cout << "Save SigCon Dir ? in Auto_SigEff : " << SaveSigConDir << endl;
}
void Auto_SigEff::SetRootFile(std::vector<TFile*> v_RootFiles)
{
   sigFile = new TFile();
   ///FindSignal File From the Rootfiles ///
   for (std::vector<TFile*>::iterator it = v_RootFiles.begin() ; it != v_RootFiles.end(); ++it)
   {
      TString fn = (*it)->GetName();
      if (FindSignalSample(fn) == "TTJets_Signal")
      {
         sigFile = (*it); 
      }
   }
}
//void Auto_SigEff::SelectHist(TFile* fn,TString hname)
void Auto_SigEff::SelectHist()
{
   ///File Check ///
   mainHist = new TH1F();
   if ( sigFile->IsOpen() == false ) { return; }
   sigFile->cd();
   TDirectory *current_sourcedir = gDirectory;
   TIter nextkey( current_sourcedir->GetListOfKeys() );
   TKey *key;
   TObject *obj;
   string histkey =""; 
   while ( (key= (TKey*)nextkey()))
   {
      sigFile->cd();
      
      obj = key->ReadObj();
      if( TString(obj->ClassName()).Contains("TH1"))
      {       
         histkey = key->GetName();
         sigFile->cd();
         TH1F *Hist = (TH1F*)gDirectory->Get(key->GetName())->Clone(histkey.c_str());
         if (TString(Hist->GetName()).Contains("Reco_CPO1_")) { mainHist = Hist; cout << ""<< endl;}
      }// if
   }// while
}
void Auto_SigEff::CalSigEff()
{
   ///File Check ///
   //cout << mainHist->Integral() << endl;
   // h_Reco_CPO1->Integral()/(77229341*0.386288)
   int total = 77229341;
   //cout << "Lumi : " << Lumi << endl; 
   double normalsf = Xsec*( Lumi /total);
   double sig_eff = mainHist->Integral()/(Xsec*Lumi);
   //cout << "Sig Eff 1 : "<<mainHist->Integral()/(total*normalsf)<< endl;
   //cout << "Sig Eff 2 : "<<mainHist->Integral()/(Xsec*Lumi)<< endl;
   string confName; 
   FILE *conffile;
//   cout << "chan.Data() : " << chan.Data() << " :: " << "outName.Data : " << outName.Data() << endl;
   confName = Form("%s/CP_Inputs_%s_%s.config",SaveSigConDir.Data(),chan.Data(),outName.Data());
//   cout << "confName : " << confName<< endl;
   TString SigEffVariName = "";
//   cout << "SigEffVariName : " << SigEffVariName << " sig_eff : " << sig_eff << endl;
   conffile = fopen(confName.c_str(),"w");
   if (channel == "MuMu") { SigEffVariName = "Selection_Eff_mumu";}
   else if (channel == "ElEl")  {SigEffVariName = "Selection_Eff_elel";}
   else if (channel == "MuEl")  {SigEffVariName = "Selection_Eff_muel";}
   else if (channel == "MuJet") { SigEffVariName = "Selection_Eff_mu";}
   else if (channel == "ElJet") { SigEffVariName = "Selection_Eff_e";}
   else {cout << "Check out your channel!!!" << endl;} 
   if (conffile == NULL ) {cout << "Check !!" << endl;}
   else {
      //printf("%s:  %f \n"  ,SigEffVariName , sig_eff );
      fprintf(conffile,"%s:  %f \n"  ,SigEffVariName.Data() , sig_eff);
      fclose(conffile);
   }
}
TString Auto_SigEff::FindSignalSample(TString fname)
{
   ///FileName -> Sample Name ///
   TString fn = fname;
   char tb = '/';
   fn.Remove(0,fn.Last(tb)+1);
   fn.ReplaceAll(".root","");
   return fn;
}
void Auto_SigEff::SetOutputSigConName( TString outname )
{
   outName = outname;
   cout << "outName? At Auto_SigEff " << outName << endl;
}


void Auto_SigEff::End()
{
   
}

