#ifndef _Asym_Calc_

#define _Asym_Calc_
  
#include <set>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <TKey.h>
#include "TMath.h"
#include "TSystem.h"
#include <sstream>
#include <iostream>    
// Text Reader //
#include "./../TextReader/TextReader.hpp"
using namespace std;

class Asym_Calc
{
   public:
      Asym_Calc();
      ~Asym_Calc();

      //user define functions
      void SetOutput(TString outputDir);
      void SetDATFile(string fdat);
      void SetConfFile(string fconf);
      void SetObs(TString obs);
      void SetChannel(TString chan);
      void LoadVari();
      void MakeValues();
      void DebugMap(map<TString,double> tmp);
      map<TString,double> ReturnValue();
      //void CalcAsym();
      void End();

      double LogPoissonProb(double Nobserved, double Nexpected);

   private:
      //put variables that you want
      TextReader *datReader;
      TextReader *SampleLoad;
      TString OutputDir;
      TString Obs;
      TString Channel;
      TString chan;
      double sig_eff;
      double Lumi;
      vector<TString> v_samples; 
      map<TString,double> m_sam_asym; 
      map<TString,double> m_sam_asymErr; 
      map<TString,double> m_sam_Nplus; 
      map<TString,double> m_sam_NplusErr; 
      map<TString,double> m_sam_fplus; 
      map<TString,double> m_sam_Nminus; 
      map<TString,double> m_sam_NminusErr; 
      map<TString,double> m_sam_fminus; 
      map<TString,double> m_sam_Ntotal;

      map<TString,double> m_Vari_Value;

   public:


};

#endif

#ifdef Asym_Calc_cxx

Asym_Calc::Asym_Calc()
{
   datReader = new TextReader();
   SampleLoad = new TextReader();
   SampleLoad->ReadFile("./SetConfig/Customize.cfg");
   SampleLoad->ReadVariables();
   v_samples.clear();
   for (int i = 0; i < SampleLoad->Size("Samples"); ++i) 
   {
      TString sam = SampleLoad->GetText("Samples",i+1);// << endl;
      v_samples.push_back(sam);
   }
/*
 N_param = SampleLoad->Size("N_param");
 * Step_Size =  SampleLoad->Size("Step_Size");
   SET_ERR = SampleLoad->Size("SET_ERR");*/
}

Asym_Calc::~Asym_Calc()
{
   delete datReader;
   delete SampleLoad;
}

#endif
   
