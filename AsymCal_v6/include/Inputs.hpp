#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <iostream>
#include "TROOT.h"

namespace CP_analysis
{

class Inputs
{
   Inputs();
   ~Inputs();

public:

   static Double_t N_ttbar;

   static Double_t N_mc;
   static Double_t f_mc_p;
   static Double_t f_mc_m;

   static Double_t N_plus_obs;
   static Double_t N_minus_obs;

   static Double_t N_plus_pred;
   static Double_t N_minus_pred;

   static Double_t N_mc_p_unc;
   static Double_t N_mc_m_unc;

   static Double_t Selection_eff;

   static Double_t Luminosity;

   static Double_t Init_Asym;
   static Double_t Init_Asym_err;

   static Int_t N_param;
   static Double_t Step_Size;
   static Double_t SET_ERR;
};

}

#endif
