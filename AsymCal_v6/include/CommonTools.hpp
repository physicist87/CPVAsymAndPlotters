////////////////////////
///                  ///  
///   Common Tools   ///
///                  ///
////////////////////////

#include <algorithm>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "THStack.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include <sstream>
#include "TStyle.h"
#include "TLorentzVector.h"

void FillHisto(TH1 *hist, Double_t val, Double_t weight=1.0);
void FillHisto(TH2 *hist, Double_t valx, Double_t valy, Double_t weight=1.0);
void FillHisto(TH3 *hist, Double_t valx, Double_t valy, Double_t valz, Double_t weight=1.0);

