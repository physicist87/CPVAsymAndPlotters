#include "../include/CPVFitter.hpp"

// Global state for TMinuit FCN callback
namespace CPVFitGlobal {
    int    n_param    = 10;
    double N_plus_obs = 0.0;
    double N_minus_obs= 0.0;
    double N_mc       = 0.0;
    double f_mc_p     = 0.0;
    double f_mc_m     = 0.0;
    double N_mc_p_unc = 0.0;
    double N_mc_m_unc = 0.0;
    double sel_eff    = 0.0;
    double luminosity = 0.0;
    int    syst_type  = 0;
}

// Negative log-likelihood function for TMinuit
//
// N_param = 10 (fixed, matches original code convention with dummy padding)
// Active parameters always occupy the last indices:
//   SYST_NONE   (n_param=10): x[8]=sigma_tt,  x[9]=Asymmetry
//   SYST_BGSTAT (n_param=10): x[6]=Nbkg_m, x[7]=Nbkg_p, x[8]=sigma_tt, x[9]=Asymmetry
//
// Parameters 0~5 (SYST_NONE) or 0~5 (SYST_BGSTAT) are dummy/fixed and unused.
void cpvFcn(Int_t& npar, double* gin, double& f, double* x, Int_t iflag) {
    using namespace CPVFitGlobal;

    int np = n_param;  // always 10
    double N_pre_tt = luminosity * sel_eff * x[np - 2];

    double N_plus_pred  = 0.0;
    double N_minus_pred = 0.0;
    double loglhood     = 0.0;

    auto logPoisson = [](double Nobs, double Nexp) -> double {
        if (Nexp < 1e-7) return 1e5;
        return Nexp - Nobs * std::log(Nexp);
    };

    auto logGauss = [](double pred, double central, double unc) -> double {
        if (unc < 1e-10) return 0.0;
        double diff = pred - central;
        return (diff * diff) / (2.0 * unc * unc);
    };

    if (syst_type == SYST_BGSTAT) {
        // x[np-3]=Nbkg_p, x[np-4]=Nbkg_m (floated bkg)
        N_plus_pred  = N_pre_tt * (1.0 + x[np - 1]) / 2.0 + x[np - 3];
        N_minus_pred = N_pre_tt * (1.0 - x[np - 1]) / 2.0 + x[np - 4];
        loglhood += logPoisson(N_plus_obs,  N_plus_pred);
        loglhood += logPoisson(N_minus_obs, N_minus_pred);
        loglhood += logGauss(x[np - 3], N_mc * f_mc_p, N_mc_p_unc);
        loglhood += logGauss(x[np - 4], N_mc * f_mc_m, N_mc_m_unc);
    } else {
        // SYST_NONE: bkg fixed at N_mc * f_mc
        N_plus_pred  = N_pre_tt * (1.0 + x[np - 1]) / 2.0 + N_mc * f_mc_p;
        N_minus_pred = N_pre_tt * (1.0 - x[np - 1]) / 2.0 + N_mc * f_mc_m;
        loglhood += logPoisson(N_plus_obs,  N_plus_pred);
        loglhood += logPoisson(N_minus_obs, N_minus_pred);
    }

    f = loglhood;
}

CPVFitter::CPVFitter() : syst_type_(SYST_NONE) {}
CPVFitter::~CPVFitter() {}

void CPVFitter::setSystType(SystType stype) { syst_type_ = stype; }

FitResult CPVFitter::fit(const FitInputs& inputs) {
    using namespace CPVFitGlobal;

    // Load global state for FCN
    N_plus_obs  = inputs.N_plus_obs;
    N_minus_obs = inputs.N_minus_obs;
    N_mc        = inputs.N_mc;
    f_mc_p      = inputs.f_mc_p;
    f_mc_m      = inputs.f_mc_m;
    N_mc_p_unc  = inputs.N_mc_p_unc;
    N_mc_m_unc  = inputs.N_mc_m_unc;
    sel_eff     = inputs.sel_eff;
    luminosity  = inputs.luminosity;
    syst_type   = static_cast<int>(syst_type_);

    // N_param = 10 always (original convention: dummy padding + active params at tail)
    const int np = 10;
    n_param = np;

    FitResult result;
    result.obs = inputs.obs;

    if (inputs.sel_eff < 1e-10) {
        std::cerr << "  CPVFitter O" << inputs.obs
                  << ": sel_eff=0, skipping fit" << std::endl;
        return result;
    }

    TMinuit* gMinuit = new TMinuit(np);

    // mminit must come before SetFCN and mnparm - it resets internal state
    gMinuit->mninit(5, 6, 7);
    gMinuit->SetFCN(cpvFcn);

    Int_t ierflg = 0;
    double step = 0.00001;  // matches original Step_Size

    // Register active parameters at tail indices (np-2, np-1)
    // Dummy parameters 0~np-3 (or 0~np-5 for BGStat) will remain fixed
    gMinuit->mnparm(np - 2, "sigma_tt",  831.7,            step, 0, 0, ierflg);
    gMinuit->mnparm(np - 1, "Asymmetry", inputs.init_asym, step, 0, 0, ierflg);

    if (syst_type_ == SYST_BGSTAT) {
        gMinuit->mnparm(np - 4, "Nbkg_m", N_mc * f_mc_m, step, 0, 0, ierflg);
        gMinuit->mnparm(np - 3, "Nbkg_p", N_mc * f_mc_p, step, 0, 0, ierflg);
    }

    // Fix all parameters first, then release 1~np-1 (index 0 stays fixed)
    // Matches original: for(ipar=1; ipar<_nparam; ipar++) Release(_nparam-ipar)
    // releases indices np-1, np-2, ..., 1  (all except 0)
    for (int i = 0;    i < np; i++)  gMinuit->FixParameter(i);
    for (int ipar = 1; ipar < np; ipar++) gMinuit->Release(np - ipar);

    static Double_t p0 = 1.0;
    static Double_t p1 = 1.0;
    static Double_t set_err       = 0.5;
    static Double_t migrad_par[2] = {10000000, 0.001};
    static Double_t minos_par     = 0;
    static Double_t rnd_seed[1]   = {123456789};

    gMinuit->mnexcm("CALL FCN",  &p1,        1, ierflg);
    gMinuit->mnexcm("SET PRINT", &p0,        0, ierflg);
    gMinuit->mnexcm("SET RAN",   rnd_seed,   1, ierflg);
    gMinuit->mnexcm("MIGRAD",    migrad_par, 2, ierflg);
    gMinuit->mnexcm("SET ERR",   &set_err,   1, ierflg);
    gMinuit->mnexcm("MINOS",     &minos_par, 0, ierflg);

    double asym, asym_dummy;
    double sigma, sigma_dummy;
    double errplus, errminus, errparab, gcc;
    double sigma_errplus, sigma_errminus, sigma_errparab, sigma_gcc;
    double fmin, fedm, errdef;
    int istat, nparx, npari;

    gMinuit->mnstat(fmin, fedm, errdef, npari, nparx, istat);
    gMinuit->GetParameter(np - 1, asym,  asym_dummy);
    gMinuit->GetParameter(np - 2, sigma, sigma_dummy);
    gMinuit->mnerrs(np - 1, errplus,       errminus,       errparab,       gcc);
    gMinuit->mnerrs(np - 2, sigma_errplus, sigma_errminus, sigma_errparab, sigma_gcc);

    result.asym               = asym;
    result.asym_err_plus      = errplus;
    result.asym_err_minus     = errminus;
    result.sigma_tt           = sigma;
    result.sigma_tt_err_plus  = sigma_errplus;
    result.sigma_tt_err_minus = sigma_errminus;
    result.fit_min            = fmin;
    result.fit_status         = istat;

    delete gMinuit;
    return result;
}

std::vector<FitResult> CPVFitter::fitAll(const FitInputsSet& inputSet) {
    std::vector<FitResult> results;
    for (int obs = 1; obs <= 13; obs++) {
        const FitInputs& fi = inputSet.get(obs);
        if (fi.N_plus_obs == 0 && fi.N_minus_obs == 0) {
            std::cout << "  O" << obs << ": no data, skipping" << std::endl;
            continue;
        }
        std::cout << "  Fitting O" << obs << " ..." << std::endl;
        FitResult r = fit(fi);
        r.print();
        results.push_back(r);
    }
    return results;
}

bool CPVFitter::writeResults(const std::vector<FitResult>& results,
                              const std::string& filepath) const {
    std::ofstream f(filepath);
    if (!f.is_open()) {
        std::cerr << "CPVFitter: Cannot write results to " << filepath << std::endl;
        return false;
    }

    f << std::fixed << std::setprecision(6);
    f << "# CPV likelihood fit results" << std::endl;
    f << "# Obs  Asym  ErrPlus  ErrMinus  Sigma_tt  SigErrP  SigErrM  FitMin  Status"
      << std::endl;

    for (const auto& r : results) {
        f << "O" << r.obs       << "  "
          << r.asym             << "  "
          << r.asym_err_plus    << "  "
          << r.asym_err_minus   << "  "
          << r.sigma_tt         << "  "
          << r.sigma_tt_err_plus  << "  "
          << r.sigma_tt_err_minus << "  "
          << r.fit_min          << "  "
          << r.fit_status       << std::endl;
    }

    f.close();
    std::cout << "  Results written: " << filepath << std::endl;
    return true;
}

void CPVFitter::printResults(const std::vector<FitResult>& results) const {
    std::cout << std::endl;
    std::cout << std::string(100, '=') << std::endl;
    std::cout << "  CPV Likelihood Fit Results (MINOS errors)" << std::endl;
    std::cout << std::string(100, '=') << std::endl;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << std::left
              << std::setw(6)  << "Obs"
              << std::setw(14) << "Asym"
              << std::setw(14) << "ErrPlus"
              << std::setw(14) << "ErrMinus"
              << std::setw(12) << "Sigma_tt"
              << std::setw(12) << "SigErrP"
              << std::setw(12) << "SigErrM"
              << std::setw(8)  << "Status"
              << std::endl;
    std::cout << std::string(100, '-') << std::endl;
    for (const auto& r : results) {
        std::cout << std::left
                  << std::setw(6)  << ("O" + std::to_string(r.obs))
                  << std::setw(14) << r.asym
                  << std::setw(14) << r.asym_err_plus
                  << std::setw(14) << r.asym_err_minus
                  << std::setw(12) << r.sigma_tt
                  << std::setw(12) << r.sigma_tt_err_plus
                  << std::setw(12) << r.sigma_tt_err_minus
                  << std::setw(8)  << r.fit_status
                  << std::endl;
    }
    std::cout << std::string(100, '=') << std::endl;
}
