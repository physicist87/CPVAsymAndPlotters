#include "../include/HistLoader.hpp"

HistLoader::HistLoader()
    : sel_eff_(0.0), luminosity_(59832.422397),
      xsec_(831.76), br_(0.10706), sc_(nullptr) {}

HistLoader::~HistLoader() {}

void HistLoader::setLuminosity(double lumi_pb) { luminosity_ = lumi_pb; }
void HistLoader::setXsec(double xsec_pb)       { xsec_ = xsec_pb; }
void HistLoader::setBranchingFraction(double br){ br_ = br; }

bool HistLoader::loadScaleConfig(const ScaleConfigReader& sc) {
    sc_ = &sc;
    return true;
}

std::string HistLoader::trim(const std::string& s) const {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string HistLoader::extractSampleName(const std::string& filepath) const {
    size_t slash = filepath.find_last_of('/');
    std::string base = (slash != std::string::npos) ? filepath.substr(slash + 1) : filepath;
    size_t dot = base.find(".root");
    if (dot != std::string::npos) base = base.substr(0, dot);
    return base;
}

int HistLoader::extractObsIndex(const std::string& histname) const {
    // Match h_Reco_CPO{N}_ReRange
    const std::string prefix = "h_Reco_CPO";
    const std::string suffix = "_ReRange";
    size_t ppos = histname.find(prefix);
    if (ppos == std::string::npos) return -1;
    size_t spos = histname.find(suffix);
    if (spos == std::string::npos) return -1;
    size_t numStart = ppos + prefix.size();
    if (spos <= numStart) return -1;
    std::string numStr = histname.substr(numStart, spos - numStart);
    try {
        int idx = std::stoi(numStr);
        if (idx >= 1 && idx <= 13) return idx;
    } catch (...) {}
    return -1;
}

bool HistLoader::isSignal(const std::string& sample) const {
    return (sample == "TTbar_Signal");
}

bool HistLoader::isData(const std::string& sample) const {
    return (sample == "Data");
}

bool HistLoader::loadFromList(const std::string& listfile) {
    std::ifstream f(listfile);
    if (!f.is_open()) {
        std::cerr << "HistLoader: Cannot open list file " << listfile << std::endl;
        return false;
    }

    std::string line;
    int nloaded = 0;
    while (std::getline(f, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        if (loadRootFile(line)) nloaded++;
    }

    f.close();
    std::cout << "HistLoader: Loaded " << nloaded << " ROOT files from " << listfile << std::endl;
    return true;
}

bool HistLoader::loadRootFile(const std::string& filepath) {
    TFile* file = TFile::Open(filepath.c_str(), "READ");
    if (!file || !file->IsOpen() || file->IsZombie()) {
        std::cerr << "HistLoader: Cannot open " << filepath << std::endl;
        if (file) { file->Close(); delete file; }
        return false;
    }

    std::string sampleName = extractSampleName(filepath);

    // Get scale factor for this sample
    double sf = 1.0;
    if (sc_) {
        if (sampleName.find("DYJets") != std::string::npos ||
            sampleName.find("DYJetsToLL") != std::string::npos) {
            sf = sc_->getDYSF();
        } else {
            sf = sc_->getSF(sampleName);
        }
    }

    TDirectory* dir = gDirectory;
    file->cd();

    TIter nextkey(file->GetListOfKeys());
    TKey* key;

    while ((key = (TKey*)nextkey())) {
        TObject* obj = key->ReadObj();
        if (!obj) continue;

        std::string classname = obj->ClassName();
        if (classname.find("TH1") == std::string::npos) {
            delete obj;
            continue;
        }

        std::string histname = key->GetName();
        int obsIdx = extractObsIndex(histname);
        if (obsIdx < 0) {
            delete obj;
            continue;
        }

        TH1* h = dynamic_cast<TH1*>(obj);
        if (!h) { delete obj; continue; }

        // h_Reco_CPO{N}_ReRange is a symmetric histogram around 0.
        // N+ = integral over x > 0 bins
        // N- = integral over x < 0 bins
        int nBins = h->GetNbinsX();
        double Np = 0.0, Nm = 0.0;
        double Np_err2 = 0.0, Nm_err2 = 0.0;

        for (int b = 1; b <= nBins; b++) {
            double center = h->GetBinCenter(b);
            double val    = h->GetBinContent(b) * sf;
            double err    = h->GetBinError(b)   * sf;
            if (center > 0.0) {
                Np     += val;
                Np_err2 += err * err;
            } else if (center < 0.0) {
                Nm     += val;
                Nm_err2 += err * err;
            }
            // center == 0: skip (boundary bin)
        }

        double Np_err = std::sqrt(Np_err2);
        double Nm_err = std::sqrt(Nm_err2);

        auto& entry = data_[sampleName][obsIdx];
        entry.N_plus      += Np;
        entry.N_minus     += Nm;
        entry.N_plus_err  = std::sqrt(entry.N_plus_err  * entry.N_plus_err  + Np_err * Np_err);
        entry.N_minus_err = std::sqrt(entry.N_minus_err * entry.N_minus_err + Nm_err * Nm_err);

        delete obj;
    }

    if (dir) dir->cd();
    file->Close();
    delete file;
    return true;
}

FitInputsSet HistLoader::buildFitInputs() const {
    FitInputsSet fis;

    // Calculate sel_eff from all accumulated TTbar_Signal events (CPO1)
    // sel_eff = signal_events / (xsec * lumi)  -- no BR, matches Auto_SigEff.cpp
    double sel_eff = 0.0;
    auto sigIt = data_.find("TTbar_Signal");
    if (sigIt != data_.end()) {
        auto obsIt = sigIt->second.find(1);
        if (obsIt != sigIt->second.end()) {
            double total_signal = obsIt->second.N_plus + obsIt->second.N_minus;
            sel_eff = total_signal / (xsec_ * br_ * luminosity_);
            std::cout << "HistLoader: sel_eff = " << sel_eff
                      << "  (signal_events=" << total_signal
                      << ", xsec*br*lumi=" << xsec_ * br_ * luminosity_ << ")" << std::endl;
        }
    } else {
        std::cerr << "HistLoader: Warning - TTbar_Signal not found, sel_eff = 0" << std::endl;
    }

    for (int obs = 1; obs <= 13; obs++) {
        FitInputs& fi = fis.get(obs);
        fi.obs        = obs;
        fi.sel_eff    = sel_eff;
        fi.luminosity = luminosity_;

        double n_mc_p     = 0.0;
        double n_mc_m     = 0.0;
        double n_mc_p_unc = 0.0;
        double n_mc_m_unc = 0.0;
        double sig_np     = 0.0;
        double sig_nm     = 0.0;

        for (const auto& sampPair : data_) {
            const std::string& sample = sampPair.first;
            auto obsIt = sampPair.second.find(obs);
            if (obsIt == sampPair.second.end()) continue;

            const HistEntry& entry = obsIt->second;

            if (isData(sample)) {
                fi.N_plus_obs  = entry.N_plus;
                fi.N_minus_obs = entry.N_minus;
            } else if (isSignal(sample)) {
                sig_np = entry.N_plus;
                sig_nm = entry.N_minus;
            } else {
                // Background: accumulate
                n_mc_p     += entry.N_plus;
                n_mc_m     += entry.N_minus;
                n_mc_p_unc  = std::sqrt(n_mc_p_unc * n_mc_p_unc + entry.N_plus_err  * entry.N_plus_err);
                n_mc_m_unc  = std::sqrt(n_mc_m_unc * n_mc_m_unc + entry.N_minus_err * entry.N_minus_err);
            }

            // Store per-sample entry for debugging dump
            SampleEntry se;
            se.N_plus     = entry.N_plus;
            se.N_minus    = entry.N_minus;
            se.N_plus_err = entry.N_plus_err;
            se.N_minus_err= entry.N_minus_err;
            se.is_data    = isData(sample);
            se.is_signal  = isSignal(sample);
            fi.samples[sample] = se;
        }

        fi.N_mc       = n_mc_p + n_mc_m;
        fi.N_mc_p_unc = n_mc_p_unc;
        fi.N_mc_m_unc = n_mc_m_unc;

        if (fi.N_mc > 0.0) {
            fi.f_mc_p = n_mc_p / fi.N_mc;
            fi.f_mc_m = n_mc_m / fi.N_mc;
        } else {
            fi.f_mc_p = 0.0;
            fi.f_mc_m = 0.0;
        }

        // init_asym from TTbar_Signal: (N+ - N-) / (N+ + N-)
        double sig_total = sig_np + sig_nm;
        if (sig_total > 0.0)
            fi.init_asym = (sig_np - sig_nm) / sig_total;
        else
            fi.init_asym = 0.0;
    }

    return fis;
}

void HistLoader::printSummary() const {
    std::cout << "[HistLoader Summary]" << std::endl;
    std::cout << "  Luminosity : " << luminosity_ << " pb^-1" << std::endl;
    std::cout << "  Xsec       : " << xsec_       << " pb" << std::endl;
    std::cout << "  Samples loaded:" << std::endl;
    for (const auto& sp : data_) {
        int nobs = sp.second.size();
        std::cout << "    " << sp.first << " (" << nobs << " observables)" << std::endl;
    }
}
