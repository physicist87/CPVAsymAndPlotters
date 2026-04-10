#ifndef CPVFITTER_HPP
#define CPVFITTER_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#include <TMinuit.h>
#include <TROOT.h>

#include "FitInputs.hpp"

// Per-observable fit result
struct FitResult {
    int    obs;
    double asym;
    double asym_err_plus;
    double asym_err_minus;
    double sigma_tt;
    double sigma_tt_err_plus;
    double sigma_tt_err_minus;
    double fit_min;
    int    fit_status;

    FitResult()
        : obs(0), asym(0), asym_err_plus(0), asym_err_minus(0),
          sigma_tt(831.76), sigma_tt_err_plus(0), sigma_tt_err_minus(0),
          fit_min(0), fit_status(-1) {}

    void print() const {
        std::cout << "  O" << obs
                  << "  A = " << asym
                  << " +" << asym_err_plus
                  << " " << asym_err_minus
                  << "  sigma_tt = " << sigma_tt
                  << " +" << sigma_tt_err_plus
                  << " " << sigma_tt_err_minus
                  << "  status=" << fit_status
                  << std::endl;
    }
};

// Syst type selector (matches original code convention)
enum SystType {
    SYST_NONE   = 0,   // Poisson-only, bkg fixed
    SYST_BGSTAT = 1    // Bkg floated with Gaussian constraint
};

// CPVFitter wraps TMinuit and runs the likelihood fit
// for a single observable given FitInputs.
// Uses global state required by TMinuit callback (standard ROOT pattern).

class CPVFitter {
public:
    CPVFitter();
    ~CPVFitter();

    void setSystType(SystType stype);

    // Run fit for a single observable
    FitResult fit(const FitInputs& inputs);

    // Run fits for all observables in FitInputsSet
    std::vector<FitResult> fitAll(const FitInputsSet& inputSet);

    // Write results table to file
    // Format: obs  asym  err+  err-  sigma_tt  sigma_tt_err+  sigma_tt_err-  status
    bool writeResults(const std::vector<FitResult>& results,
                      const std::string& filepath) const;

    // Print results table to stdout
    void printResults(const std::vector<FitResult>& results) const;

private:
    SystType syst_type_;
};

// Global state for TMinuit FCN callback (ROOT requirement)
namespace CPVFitGlobal {
    extern int    n_param;   // always 10 (original convention)
    extern double N_plus_obs;
    extern double N_minus_obs;
    extern double N_mc;
    extern double f_mc_p;
    extern double f_mc_m;
    extern double N_mc_p_unc;
    extern double N_mc_m_unc;
    extern double sel_eff;
    extern double luminosity;
    extern int    syst_type;
}

// TMinuit FCN
void cpvFcn(Int_t& npar, double* gin, double& f, double* x, Int_t iflag);

#endif
