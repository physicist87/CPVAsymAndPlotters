#ifndef HISTLOADER_HPP
#define HISTLOADER_HPP

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <cmath>

#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <TDirectory.h>
#include <TString.h>

#include "ScaleConfigReader.hpp"
#include "FitInputs.hpp"  // includes SampleEntry

// HistLoader reads ROOT files from a .list file,
// extracts h_Reco_CPO{N}_ReRange histograms per sample,
// applies scale factors from ScaleConfigReader,
// and assembles FitInputsSet (N+, N-, f, sel_eff) per observable.
//
// Signal sample : TTbar_Signal
// Background    : all other MC samples except Data
// Data          : sample named "Data"

class HistLoader {
public:
    HistLoader();
    ~HistLoader();

    void setLuminosity(double lumi_pb);
    void setXsec(double xsec_pb);
    void setBranchingFraction(double br);

    // Load scale factors
    bool loadScaleConfig(const ScaleConfigReader& sc);

    // Load ROOT files from a .list file and extract histograms
    bool loadFromList(const std::string& listfile);

    // Build FitInputsSet from loaded histograms
    // Must be called after loadFromList()
    FitInputsSet buildFitInputs() const;

    void printSummary() const;

private:
    // Per-sample N+ and N- for each observable (obs index 1-based)
    // sample name -> obs index -> {N+, N-, N+_err, N-_err}
    struct HistEntry {
        double N_plus;
        double N_minus;
        double N_plus_err;
        double N_minus_err;
        HistEntry() : N_plus(0), N_minus(0), N_plus_err(0), N_minus_err(0) {}
    };

    // sample name -> obs(1~13) -> HistEntry
    std::map<std::string, std::map<int, HistEntry>> data_;

    // Signal efficiency (from TTbar_Signal CPO1 integral)
    double sel_eff_;

    double luminosity_;
    double xsec_;
    double br_;

    const ScaleConfigReader* sc_;

    // Load a single ROOT file
    bool loadRootFile(const std::string& filepath);

    // Extract sample name from file path
    std::string extractSampleName(const std::string& filepath) const;

    // Extract observable index from histogram name
    // e.g. h_Reco_CPO3_ReRange -> 3, returns -1 if not matched
    int extractObsIndex(const std::string& histname) const;

    // Determine if sample is signal
    bool isSignal(const std::string& sample) const;

    // Determine if sample is data
    bool isData(const std::string& sample) const;

    std::string trim(const std::string& s) const;
};

#endif
