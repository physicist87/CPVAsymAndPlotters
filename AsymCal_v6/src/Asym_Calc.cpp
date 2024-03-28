#define Asym_Calc_cxx
#include "../include/Asym_Calc.hpp"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;


void Asym_Calc::SetOutput(TString outputDir)
{
   OutputDir = Form("./output/%s/",outputDir.Data());  
}
void Asym_Calc::SetDATFile(string fdat)
{
   string fdatpath = "./dat/"+fdat;
   datReader->ReadFile(fdatpath);
   datReader->ReadVariables();
}
void Asym_Calc::SetConfFile(string fconf)
{
   FILE* sigEff;
   string fconfpath = "./config/"+fconf;
   sigEff = fopen(fconfpath.c_str(),"r");
   double sigeff =0.0;
   char vari[100];
   if (sigEff != NULL){
   while (fscanf(sigEff, "%s %lf\n", vari,&sigeff) != EOF)
   {
      sig_eff = sigeff; 
   }
   fclose(sigEff);
   }
   else {sig_eff = 0.0; cout << "No SigEff!!!!" << endl;}
   cout << "SetConfFile Sig EFF !!!!" << sig_eff  << endl;
}
void Asym_Calc::SetObs(TString obs)
{
   Obs = obs;
}

void Asym_Calc::SetChannel(TString channel)
{
   Channel = channel;
   if (Channel == "MuMu"){chan = "dimu";}
   if (Channel == "ElEl"){chan = "diel";}
   if (Channel == "MuEl"){chan = "muel";}
   if (Channel == "DiLep"){chan = "dilep";}
   //chan = "dilep";
}
void Asym_Calc::LoadVari()
{
   cout << "Obs : " << Obs << " Channel " << Channel  << endl;
   //dilep_hO1_4Data_asym
   Lumi = datReader->GetNumber("Luminosity");
   cout << "Luminosity !!! " << Lumi << endl;
   cout << "chan ??" << chan << endl;
   for (vector<TString>::iterator it = v_samples.begin(); it != v_samples.end(); ++it)
   {
      TString variName;
      variName = Form("%s_h%s_4%s_asym",chan.Data(),Obs.Data(),(*it).Data());
      double varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_asym[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_asym_err",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_asymErr[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_Nplus",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_Nplus[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_Nplus_err",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_NplusErr[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_fplus",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_fplus[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_Nminus",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_Nminus[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_Nminus_err",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_NminusErr[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_fminus",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_fminus[*it]=varivalue;
      }
      variName = Form("%s_h%s_4%s_total",chan.Data(),Obs.Data(),(*it).Data());
      varivalue = 0.0;
      if (datReader->Check(variName.Data())){
         varivalue = datReader->GetNumber(variName.Data());
         m_sam_Ntotal[*it]=varivalue;
      }
   }
   cout << "-------------Chek Asym----------------" << endl;
   DebugMap(m_sam_asym);
   cout << "-------------Chek AsymErr----------------" << endl;
   DebugMap(m_sam_asymErr);
   cout << "-------------Chek NPlus----------------" << endl;
   DebugMap(m_sam_Nplus);
   cout << "-------------Chek NPlusErr----------------" << endl;
   DebugMap(m_sam_NplusErr);
   cout << "-------------Chek F Plus----------------" << endl;
   DebugMap(m_sam_fplus);
   cout << "-------------Chek NMinus----------------" << endl;
   DebugMap(m_sam_Nminus);
   cout << "-------------Chek NMinusErr----------------" << endl;
   DebugMap(m_sam_NminusErr);
   cout << "-------------Chek F Minus----------------" << endl;
   DebugMap(m_sam_fminus);
   cout << "-------------Chek N Total----------------" << endl;
   DebugMap(m_sam_Ntotal);
}

void Asym_Calc::DebugMap(map<TString,double> tmp)
{
   for(map<TString,double>::iterator it = tmp.begin(); it != tmp.end(); ++it)
   {
      cout << "it first " << it->first << " second : " << it->second << endl;
   }
}
void Asym_Calc::MakeValues()
{
   cout << "Make Values for Likelihood fit !!" << endl;
   m_Vari_Value.clear();

   double N_mc = 0.0;
   double N_plus_obs = 0.0;
   double N_minus_obs = 0.0;
   double f_mc_p = 0.0;
   double f_mc_m = 0.0;

   double N_ttbar = 0.0;
   N_ttbar = m_sam_Nplus["TTJets_Signal"] + m_sam_Nminus["TTJets_Signal"];

   double n_mc_p = 0.0;
   double n_mc_m = 0.0;
   double n_mc_p_unc = 0.0;
   double n_mc_m_unc = 0.0;
   for (map<TString,double>::iterator it = m_sam_Nplus.begin(); it != m_sam_Nplus.end(); ++it)
   {
      if (it->first == "Data"){ continue;}
      if (it->first == "TTJets_Signal"){ continue;}
      cout << "it->first : " << it->first << endl;
      n_mc_p+=it->second;
      n_mc_m+= m_sam_Nminus[it->first];
      n_mc_p_unc += pow(m_sam_NplusErr[it->first],2);
      n_mc_m_unc += pow(m_sam_NminusErr[it->first],2);
   }
   N_mc= n_mc_p+n_mc_m;
   cout << "N_mc : " << N_mc << endl;
   f_mc_p = (n_mc_p)/(N_mc); 
   if (f_mc_p == 0){ f_mc_m = 0;}
   else {f_mc_m = 1 - f_mc_p;}

   N_plus_obs  = m_sam_Nplus["Data"];
   N_minus_obs = m_sam_Nminus["Data"];
   m_Vari_Value["N_mc"]= N_mc;
   m_Vari_Value["N_plus_obs"]= N_plus_obs;
   m_Vari_Value["N_minus_obs"]= N_minus_obs;
   m_Vari_Value["f_mc_p"]= f_mc_p;
   m_Vari_Value["f_mc_m"]= f_mc_m;
   m_Vari_Value["N_plus_mc_unc"]= sqrt(n_mc_p_unc);
   m_Vari_Value["N_minus_mc_unc"]= sqrt(n_mc_m_unc);
   m_Vari_Value["Asym_ttbar"]= m_sam_asym["TTJets_Signal"];
   m_Vari_Value["Asym_ttbarErr"]= m_sam_asymErr["TTJets_Signal"];
   m_Vari_Value["Selection_eff"]= sig_eff;
   m_Vari_Value["Luminosity"]= Lumi;
}
map<TString,double> Asym_Calc::ReturnValue()
{
   cout << "Return Values !!!!" << endl;
   if (m_Vari_Value.size() == 0){cout <<  "Empty !!! Map!!!" << endl;}
   return m_Vari_Value;
}
void Asym_Calc::End()
{

}
