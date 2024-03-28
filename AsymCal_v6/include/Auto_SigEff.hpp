#ifndef _Auto_SigEff_

#define _Auto_SigEff_
  
#include <set>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <TH1.h>
#include <TH2.h> 
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <THStack.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TKey.h>
#include <TStyle.h>
#include <TF1.h>
#include "TMath.h"
#include "TSystem.h"
#include <TCanvas.h> 
#include <sstream>
#include <iostream>    
#include <TLegend.h>
#include "../include/tdrStyle.h"
// Text Reader //
#include "./../TextReader/TextReader.hpp"
using namespace std;

class Auto_SigEff
{
   public:
      Auto_SigEff();
      ~Auto_SigEff();

      //user define functions
      void SetChannel( TString ch);
      void SetSigConOutputDir(TString SigConDir);
      void SetOutputSigConName(TString outname);
      void SetRootFile(std::vector<TFile*>v_RootFiles);
      TString FindSignalSample(TString fname);
      //void SelectHist(TFile* fn,TString hname);
      void SelectHist();
      void CalSigEff();
      void End();

   private:
      //put variables that you want
      TextReader *SysReader;
      TString channel;
      TString chan;
      TString SaveSigConDir;
      TString outName;//SigCon file Name //
      TFile* sigFile;//SigCon file Name //
      TH1F* mainHist;//SigCon file Name //
      double Lumi;
      double Xsec;
   public:
      
};

#endif

#ifdef Auto_SigEff_cxx

Auto_SigEff::Auto_SigEff()
{
   SysReader = new TextReader();
   SysReader->ReadFile("./SetConfig/Customize.cfg");
   SysReader->ReadVariables();
   Lumi = SysReader->GetNumber("Lumi");
   Xsec = SysReader->GetNumber("Xsec");

}

Auto_SigEff::~Auto_SigEff()
{
}

#endif
   
