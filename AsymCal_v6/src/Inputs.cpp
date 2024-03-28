#include <iostream>
#include "TROOT.h"
#include "../include/Inputs.hpp"

using namespace std;

namespace CP_analysis 
{

Inputs::Inputs(){}

Inputs::~Inputs(){}

Double_t Inputs::N_ttbar = 0.; //for muonelec 

Double_t Inputs::N_mc = 0.; // for muel
Double_t Inputs::f_mc_p = 0.; // for muel
Double_t Inputs::f_mc_m = 0.; // for muel

Double_t Inputs::N_plus_obs = 0.;
Double_t Inputs::N_minus_obs = 0.;

Double_t Inputs::N_plus_pred = 0.;
Double_t Inputs::N_minus_pred = 0.;

Double_t Inputs::N_mc_p_unc = 0.;
Double_t Inputs::N_mc_m_unc = 0.;

Double_t Inputs::Selection_eff = 0.;

Double_t Inputs::Luminosity = 0.;

Double_t Inputs::Init_Asym = 0.;
Double_t Inputs::Init_Asym_err = 0.;

Int_t    Inputs::N_param = 0;
Double_t Inputs::Step_Size = 0.0001;
Double_t Inputs::SET_ERR = 0.5;
}
