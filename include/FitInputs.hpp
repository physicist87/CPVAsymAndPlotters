#ifndef FITINPUTS_HPP
#define FITINPUTS_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

// Per-sample N+/N- entry (for debugging dump)
struct SampleEntry {
    double N_plus;
    double N_minus;
    double N_plus_err;
    double N_minus_err;
    bool   is_signal;
    bool   is_data;
    SampleEntry()
        : N_plus(0), N_minus(0), N_plus_err(0), N_minus_err(0),
          is_signal(false), is_data(false) {}
};

// Per-observable fit input values assembled by HistLoader.
// Passed in memory to CPVFitter, and also printed to disk.

struct FitInputs {
    int    obs;            // observable index (1~13)
    double N_plus_obs;     // Data N+
    double N_minus_obs;    // Data N-
    double N_mc;           // total background (non-signal, non-data)
    double f_mc_p;         // background positive fraction
    double f_mc_m;         // background negative fraction
    double N_mc_p_unc;     // background N+ statistical uncertainty
    double N_mc_m_unc;     // background N- statistical uncertainty
    double sel_eff;        // signal selection efficiency
    double luminosity;     // luminosity in pb^-1
    double init_asym;      // TTbar_Signal asymmetry used as Minuit initial value

    // Per-sample breakdown for debugging
    std::map<std::string, SampleEntry> samples;

    FitInputs()
        : obs(0), N_plus_obs(0), N_minus_obs(0), N_mc(0),
          f_mc_p(0), f_mc_m(0), N_mc_p_unc(0), N_mc_m_unc(0),
          sel_eff(0), luminosity(0), init_asym(0) {}

    void print() const {
        std::cout << "  [FitInputs O" << obs << "]" << std::endl;
        std::cout << "    Luminosity   : " << luminosity   << " pb^-1" << std::endl;
        std::cout << "    sel_eff      : " << sel_eff      << std::endl;
        std::cout << "    N_plus_obs   : " << N_plus_obs   << std::endl;
        std::cout << "    N_minus_obs  : " << N_minus_obs  << std::endl;
        std::cout << "    N_mc         : " << N_mc         << std::endl;
        std::cout << "    f_mc_p       : " << f_mc_p       << std::endl;
        std::cout << "    f_mc_m       : " << f_mc_m       << std::endl;
        std::cout << "    N_mc_p_unc   : " << N_mc_p_unc   << std::endl;
        std::cout << "    N_mc_m_unc   : " << N_mc_m_unc   << std::endl;
        std::cout << "    init_asym    : " << init_asym    << std::endl;
    }
};

// Collection of FitInputs for all observables, with dump-to-file support.
class FitInputsSet {
public:
    std::vector<FitInputs> inputs;  // index 0 = O1, ..., index 12 = O13

    FitInputsSet() { inputs.resize(13); }

    FitInputs& get(int obs) { return inputs[obs - 1]; }
    const FitInputs& get(int obs) const { return inputs[obs - 1]; }

    // Write summary + per-sample breakdown to a .dat file for debugging
    bool writeDat(const std::string& filepath) const {
        std::ofstream f(filepath);
        if (!f.is_open()) {
            std::cerr << "Error: Cannot write FitInputs to " << filepath << std::endl;
            return false;
        }

        f << std::fixed << std::setprecision(6);

        for (int i = 1; i <= 13; i++) {
            const FitInputs& fi = get(i);
            if (fi.N_plus_obs == 0 && fi.N_minus_obs == 0) continue;

            f << "# ===== O" << i << " =====" << std::endl;

            // Summary line (same format as before)
            f << "# obs  N_plus_obs  N_minus_obs  N_mc  f_mc_p  f_mc_m  "
              << "N_mc_p_unc  N_mc_m_unc  sel_eff  luminosity  init_asym" << std::endl;
            f << "O" << i << "  "
              << fi.N_plus_obs  << "  "
              << fi.N_minus_obs << "  "
              << fi.N_mc        << "  "
              << fi.f_mc_p      << "  "
              << fi.f_mc_m      << "  "
              << fi.N_mc_p_unc  << "  "
              << fi.N_mc_m_unc  << "  "
              << fi.sel_eff     << "  "
              << fi.luminosity  << "  "
              << fi.init_asym   << std::endl;

            // Per-sample breakdown
            if (!fi.samples.empty()) {
                f << "# --- per-sample breakdown ---" << std::endl;
                f << "# sample  type  N_plus  N_minus  N_plus_err  N_minus_err" << std::endl;
                for (const auto& sp : fi.samples) {
                    const std::string& sname = sp.first;
                    const SampleEntry& se    = sp.second;
                    std::string type = se.is_data ? "Data" : (se.is_signal ? "Signal" : "Bkg");
                    f << "  " << std::left << std::setw(50) << sname
                      << "  " << std::setw(8) << type
                      << "  " << std::right << std::setw(14) << se.N_plus
                      << "  " << std::setw(14) << se.N_minus
                      << "  " << std::setw(14) << se.N_plus_err
                      << "  " << std::setw(14) << se.N_minus_err
                      << std::endl;
                }
            }
            f << std::endl;
        }

        std::cout << "  FitInputs written: " << filepath << std::endl;
        return true;
    }
};

#endif
