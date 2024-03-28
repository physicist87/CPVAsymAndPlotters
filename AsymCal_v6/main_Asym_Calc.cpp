////////////////////////////////////////////////
//                                            //
//                                            //
//  Author: Seungkyu Ha, seungkyu.ha@cern.ch  //
//                                            //
//                                            //
////////////////////////////////////////////////

#include <iostream>
#include <fstream>  
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include "TMinuit.h"
#include <TROOT.h>
#include <TUnixSystem.h>
#include <TChain.h>
#include <TStyle.h>
#include <TApplication.h>
#include <TString.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TFile.h>
#include "./include/Asym_Calc.hpp"
#include "./include/setuputil.hpp"
#include "./include/Inputs.hpp"

using namespace std;
using namespace CP_analysis;

TROOT root ("Plots", "Program for dream analysis");
Int_t _nparam = 0;

double LogPoissonProb(double Nobserved, double Nexpected);
double poisson(double nbar, double n);
double LogGaussProb(double Nbkgpred, double Nbkg, double NbkgUnc);
extern void fcnk0(Int_t & npar, double *gin, double &f, double *x, Int_t iflag);
//void CalcAsym(); 
void CalcAsym(TString outfile, TString outxsecfile); 

//argc: # of arguments, argv:array for arguments
int main(int argc, char **argv)
{
   printf("The number of options is: %i\n",argc-1);
   ////////////////////
   /// UserFunction ///    
   ////////////////////
   bool to_bool(std::string str);
   std::vector<TFile*> GetROOTFile(string filelistpath);
   std::map<TString,double> LoadScaleInfo(string scfile);
   void CloseROOTFiles(vector<TFile*> v_f);



   //////////////////////
   /// User Variables ///    
   //////////////////////
   bool isdebug = false;
   bool isComb        = false;
   string channel   = "none";
   string inputFileName   = "none";
   string studyName     = "none";
   string systName     = "none";
   string outputDir     = "none";
   string object        = "none";
   string confFile        = "none";
  
   //////////////////////////
   /// read input options ///
   //////////////////////////
   for (int iopt = 0; iopt<argc; ++iopt)
   {
      char *argch = argv[iopt];
      if(strcmp(argch,"-channel")==0) 
      {
         channel = argv[iopt+1];
         iopt = iopt+1;
      }
      if(strcmp(argch,"-inputDAT")==0) 
      {
         inputFileName = argv[iopt+1];
         iopt = iopt+1;
      }

      if(strcmp(argch,"-studyName")==0) // Output Directory //
      {
         studyName = argv[iopt+1];
         iopt = iopt+1;
      }

      if(strcmp(argch,"-systName")==0) // Output Directory //
      {
         systName = argv[iopt+1];
         iopt = iopt+1;
      }
      if(strcmp(argch,"-outputDir")==0) // Output Directory //
      {
         outputDir = argv[iopt+1];
         iopt = iopt+1;
      }

      if(strcmp(argch,"-obs")==0)
      {
         object = argv[iopt+1];
         iopt = iopt+1;
      }
      if(strcmp(argch,"-config")==0)
      {
         confFile = argv[iopt+1];
         iopt = iopt+1;
      }
   }

   cout << 
   "channel : " << channel << 
   " studyName : " << studyName << 
   " outputDir : " << outputDir << 
   " object : " << object << 
   " inputFileName : " << inputFileName << 
   " confFile : " << confFile << 
   endl;
   Asym_Calc* asyms = new Asym_Calc();
   asyms->SetChannel(channel);
   asyms->SetObs(object);
   asyms->SetConfFile(confFile);
   asyms->SetDATFile(inputFileName);
   asyms->LoadVari();
   asyms->MakeValues();
   map<TString,double> vari = asyms->ReturnValue();
   ///// Set Obs (setuputil) /////
   TString SetObs = object;
   SetObs.ReplaceAll("O","");
   setuputil::Observable = SetObs.Atoi();
   ///// Set OutputFile /////
   // output file : output/SystematicStudy_v13/MuMu/dimuon_O10_SystematicStudy_v13.txt
   // output/SystematicStudy_v13/MuMu/dimuon_O1_SystematicStudy_v13_diff.txt
   TString asymOut = "";
   TString xsecOut = "";
   /// Set Channel ///
   TString Chan = "";
   if (channel == "MuMu") {Chan = "dimuon";}
   if (channel == "ElEl") {Chan = "dielec";}
   if (channel == "MuEl") {Chan = "muonelec";}
   if (channel == "DiLep") {Chan = "dileptonic";}
   if (systName == "none" || systName == "None"){
       asymOut = Form("./output/%s/%s_%s_%s.txt",outputDir.c_str(),Chan.Data(),object.c_str(),studyName.c_str());
       xsecOut = Form("./output/%s/%s_%s_%s_diff.txt",outputDir.c_str(),Chan.Data(),object.c_str(),studyName.c_str());
   }
   else {
       asymOut = Form("./output/%s/%s_%s_%s_%s.txt",outputDir.c_str(),Chan.Data(),object.c_str(),studyName.c_str(),systName.c_str());
       xsecOut = Form("./output/%s/%s_%s_%s_%s_diff.txt",outputDir.c_str(),Chan.Data(),object.c_str(),studyName.c_str(),systName.c_str());
   }
   ////////////////////
   //// SetSystTYPE ///    
   ////////////////////
   if (systName== "BGStat") { setuputil::SYST_TYPE = 1; Inputs::N_param = 10;}

   //Inputs::N_param = 100;
   Inputs::N_param = 10;
   Inputs::Step_Size =  0.00001;
   Inputs::SET_ERR = 0.5;
   Inputs::Init_Asym = vari["Asym_ttbar"]; 
   Inputs::Init_Asym_err = vari["Asym_ttbarErr"]; 
   Inputs::N_mc      = vari["N_mc"]; 
   Inputs::f_mc_p    = vari["f_mc_p"]; 
   Inputs::f_mc_m    = vari["f_mc_m"];
   Inputs::N_mc_p_unc  = vari["N_plus_mc_unc"]; 
   Inputs::N_mc_m_unc  = vari["N_minus_mc_unc"]; 
   Inputs::N_plus_obs  = vari["N_plus_obs"]; 
   Inputs::N_minus_obs = vari["N_minus_obs"]; 
   Inputs::Luminosity = vari["Luminosity"]; 
   Inputs::Selection_eff = vari["Selection_eff"]; 
   CalcAsym(asymOut,xsecOut); 
   cout << "End processing..." << endl << endl;
   return 0;
}
// 
bool to_bool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}
//void CalcAsym() 
void CalcAsym(TString outfile, TString outxsecfile) 
{

   ofstream fout;
   ofstream fout1;

   fout.open(outfile.Data());
   fout1.open(outxsecfile.Data());
   _nparam = Inputs::N_param;
   cout << "_nparam : " << _nparam << endl;
   double x[_nparam];
   double x_err[_nparam];
   double step_size = Inputs::Step_Size;
   cout << "step_size ? " << step_size << endl;
   x[_nparam-1] = Inputs::Init_Asym;
   cout << "Inputs::Init_Asym ? " <<  Inputs::Init_Asym << endl;
//   x[_nparam-2] = 8.13; 
   x[_nparam-2] = 831.7; 
//   cout << "x[_nparam-1]" << x[_nparam-1] << endl;
   if ( setuputil::SYST_TYPE ==1){ 
      x[_nparam-3] = Inputs::N_mc*Inputs::f_mc_p;
      x[_nparam-4] = Inputs::N_mc*Inputs::f_mc_m;
   }
   TMinuit *gMinuit = new TMinuit(_nparam);
 
   Int_t ierflg = 0;
   static Double_t migradPar[2] ={10000000,.001}; // Migrad parameters
   static Double_t minosPar=0; // minos parameter
   static Double_t UP[1] = {Inputs::SET_ERR};
   static Double_t p0=1;
   static Double_t p1=1;
   static Double_t rnd_seed[1]={123456789};

   gMinuit->mninit(5,6,7);
   gMinuit->SetFCN(fcnk0);

   gMinuit->mnparm(_nparam-2, "sigma_tt",831.7, step_size, 0, 0, ierflg);
   gMinuit->mnparm(_nparam-1, "Asymmetry", Inputs::Init_Asym, step_size, 0, 0, ierflg); 
 
   if (setuputil::SYST_TYPE ==1){
   gMinuit->mnparm(_nparam-4, "Nbkg_m",Inputs::N_mc*Inputs::f_mc_m, step_size, 0, 0, ierflg);
   gMinuit->mnparm(_nparam-3, "Nbkg_p",Inputs::N_mc*Inputs::f_mc_p, step_size, 0, 0, ierflg);
   }
 
   for( int i = 0; i < _nparam; i++ ) gMinuit->FixParameter(i);
   for( int ipar = 1; ipar < _nparam; ipar++ ) 
   {
      gMinuit->Release(_nparam - ipar);
   }
   gMinuit->mnexcm("CALL FCN", &p1 ,1,ierflg);
   gMinuit->mnexcm("SET PRINT", &p0 ,0,ierflg);
   gMinuit->mnexcm("SET RAN", rnd_seed ,1,ierflg);
   gMinuit->mnexcm("MIGRAD", migradPar ,2,ierflg);
   for( int i = 0; i < _nparam; i++ ) gMinuit->GetParameter(i, x[i], x_err[i]);
   //UP=1 for chi-square, UP=0.5 for negative log likelihood
   gMinuit->mnexcm("SET ERR",UP,1,ierflg);
   gMinuit->mnexcm("MINOS", &minosPar, 0, ierflg);
   //gMinuit->mnexcm("SCAN", p2, 1, ierflg);

   //double asym_cw, errplus_cw, errminus_cw, errparab_cw, gcc_cw, dummy_cw;
   //double asym_ww, errplus_ww, errminus_ww, errparab_ww, gcc_ww, dummy_ww;
   double asym, errplus, errminus, errparab, gcc, dummy;
   double xstt, xstt_errplus, xstt_errminus, xstt_errparab, xstt_gcc, xstt_dummy;
   int istat, nparx, npari;
   double fmin, fedm, errdef;
   gMinuit->mnstat(fmin,fedm,errdef,npari,nparx,istat);
   gMinuit->GetParameter(_nparam-1, asym, dummy);
   gMinuit->mnerrs(_nparam-1, errplus, errminus, errparab, gcc);
   gMinuit->GetParameter(_nparam-2, xstt, xstt_dummy);
   gMinuit->mnerrs(_nparam-2, xstt_errplus, xstt_errminus, xstt_errparab, xstt_gcc);

   fout << asym << " " << errplus << " " << errminus << endl;

   fout1 << xstt << " " << xstt_errplus << " " << xstt_errminus << endl;
   fout.close();
   fout1.close();
   //cout << 0.915*asym_cw + 0.085*asym_ww << " " << sqrt((0.915*errplus_cw)*(0.915*errplus_cw)+(0.085*errminus_cw)*(0.085*errminus_ww)) << endl;
   cout << endl << endl;
   cout << "========================================================================================================= " << endl;
   cout << "====                             Summary of likelihood fit by Minuit                                ===== " << endl;
   cout << "========================================================================================================= " << endl;
   cout << endl << endl;
   printf("----------------------------------------------------------------------------------------------------------\n");
   cout << "Asymmetry O" << setuputil::Observable << " = " << asym << " +" << errplus << " -" << errminus << "  (MINOS errors)" << endl;
   cout << "xsection ttbar O" << setuputil::Observable << " = " << xstt << " +" << xstt_errplus << " -" << xstt_errminus << "  (MINOS errors)" << endl;
   printf("----------------------------------------------------------------------------------------------------------\n");
   cout << endl << endl << endl;
}
//============================================================================
//===
//===
//===
//===                     Likelihood function 
//===
//===
//===
//============================================================================

//npar: number of free parameters involved in minimization
//gin: computed gradient values (optional)
//f: the function value itself
//x: vector of constant and variable parameters
//flag: to switch between several actions of FCN
//============================================================================
void fcnk0(Int_t & npar, double *gin, double &f, double *x, Int_t iflag)
//============================================================================
{

   double loglhood = 0;
   Double_t N_plus_pred, N_minus_pred;
   Double_t N_pre_tt; 
   //cout << N_pre_tt << " " << Inputs::N_ttbar_e << " " << x[_nparam-2] << endl;
   //N_pre_tt_e = Inputs::Luminosity * Inputs::Br_tt_ej * Inputs::Selection_eff_e * x[_nparam-2];
//   N_pre_tt_mumu = Inputs::Luminosity * Inputs::Selection_eff_mu * x[_nparam-2];
//   cout << "N_pre_tt_mumu : " << N_pre_tt_mumu << endl;

   
   //
   N_pre_tt = Inputs::Luminosity * Inputs::Selection_eff * x[_nparam-2];

   if(setuputil::SYST_TYPE==1) 
   {


      N_plus_pred = N_pre_tt * (1 + x[_nparam-1])/2 +
                                             (x[_nparam-3]);
      N_minus_pred = N_pre_tt * (1 - x[_nparam-1])/2 +
                                             (x[_nparam-4]);
      loglhood += LogPoissonProb(Inputs::N_plus_obs, N_plus_pred);
      loglhood += LogGaussProb((x[_nparam-3]),Inputs::N_mc*Inputs::f_mc_p,Inputs::N_mc_p_unc); /*cout << "GAUSSIAN !! Inputs::N_mc*Inputs::f_mc_p : "<< Inputs::N_mc*Inputs::f_mc_p << " Inputs::N_mc_p_unc ? " << Inputs::N_mc_p_unc << endl;*/ 
      loglhood += LogPoissonProb(Inputs::N_minus_obs, N_minus_pred);
      loglhood += LogGaussProb((x[_nparam-4]),Inputs::N_mc*Inputs::f_mc_m,Inputs::N_mc_m_unc); /*cout << "GAUSSIAN !! Inputs::N_mc*Inputs::f_mc_m : " << Inputs::N_mc*Inputs::f_mc_m << " Inputs::N_mc_m_unc : " << Inputs::N_mc_m_unc  << endl;*/ 
   } 
   else {
      N_plus_pred = N_pre_tt * (1 + x[_nparam-1])/2 +
                                          ( Inputs::N_mc*Inputs::f_mc_p); 
      N_minus_pred = N_pre_tt * (1 - x[_nparam-1])/2 +
                        (Inputs::N_mc*Inputs::f_mc_m);


      loglhood += LogPoissonProb(Inputs::N_plus_obs, N_plus_pred);
      loglhood += LogPoissonProb(Inputs::N_minus_obs, N_minus_pred);

   } 
   //

   f = loglhood;
}


//==============================
//==== poisson probability =====
//==============================
double LogPoissonProb(double Nobserved, double Nexpected)
{

//  cout << "In LogPoissonProb :  Nexpected " << Nexpected << " Nobserved : " << Nobserved << endl;
  if(Nexpected<0.0000001) return 100000;
  return Nexpected - Nobserved*log(Nexpected);
/*
  //  cout << "Nexpected " << Nexpected << endl;
  if(Nexpected<0.0000001) return 100000;
  return Nexpected - Nobserved*log(Nexpected);*/
}



//---------------------------------------------------
double poisson(double nbar, double n)
//---------------------------------------------------
{
    double u = 1;
    for( int i=0 ; i<n ; i++ ) {
      u *= nbar/(i+1);
    }
    return u*exp(-nbar);
}

//===============================
//==== Gaussian probability =====
//===============================
double LogGaussProb(double Nbkgpred, double Nbkg, double NbkgUnc)
{
  double prob_ = 0.0;
  prob_ = (pow(Nbkgpred-Nbkg,2)/(2*pow(NbkgUnc,2)));
//  cout << "prob_ : " << prob_ << endl;
//  cout << "Nbkgpred " << Nbkgpred << " Nbkg : " << Nbkg << " NbkgUnc " << NbkgUnc  << "prob_ : " << prob_ << endl;

  return prob_;
}
