////////////////////////////////////////////////
//                                            //
//  CalCPVFit - CP Violation Likelihood Fitter//
//  Author: Seungkyu Ha, seungkyu.ha@cern.ch  //
//                                            //
////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>

#include "include/ScaleConfigReader.hpp"
#include "include/HistLoader.hpp"
#include "include/FitInputs.hpp"
#include "include/CPVFitter.hpp"

static void usage(const char* prog) {
    std::cout << "Usage: " << prog
              << " <input.list[,input2.list,...]>"
              << " <ScaleConfig.txt[,ScaleConfig2.txt,...]>"
              << " <channel> <runperiod> <studyname>"
              << " [--syst none|BGStat]"
              << " [--lumi <pb^-1>]"
              << " [--xsec <pb>]"
              << " [--br <value>]"
              << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  Single channel/period:" << std::endl;
    std::cout << "    " << prog
              << " input/.../MuMu.list ScaleConfig/.../MuMu.txt MuMu UL2018 AN_v6p4"
              << std::endl;
    std::cout << "  Dilepton (comma-separated):" << std::endl;
    std::cout << "    " << prog
              << " MuMu.list,ElEl.list,MuEl.list MuMu.txt,ElEl.txt,MuEl.txt"
              << " Dilepton UL2018 AN_v6p4"
              << std::endl;
    std::cout << "  Run2 MuMu (4 periods comma-separated):" << std::endl;
    std::cout << "    " << prog
              << " PreVFP.list,PostVFP.list,2017.list,2018.list"
              << " PreVFP.txt,PostVFP.txt,2017.txt,2018.txt"
              << " MuMu Run2 AN_v6p4 --lumi 137600"
              << std::endl;
}

static void mkdirRecursive(const std::string& path) {
    size_t pos = 0;
    std::string dir;
    while ((pos = path.find('/', pos)) != std::string::npos) {
        dir = path.substr(0, pos++);
        if (!dir.empty()) mkdir(dir.c_str(), 0755);
    }
    mkdir(path.c_str(), 0755);
}

static std::vector<std::string> splitComma(const std::string& s) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) result.push_back(item);
    }
    return result;
}

int main(int argc, char** argv) {
    if (argc < 6) { usage(argv[0]); return 1; }

    std::string inputListArg   = argv[1];
    std::string scaleConfigArg = argv[2];
    std::string channel        = argv[3];
    std::string runPeriod      = argv[4];
    std::string studyName      = argv[5];

    // Optional arguments
    SystType systType = SYST_NONE;
    double lumi = 59832.422397;
    double xsec = 831.76;
    double br   = 0.10706;

    for (int i = 6; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--syst" && i + 1 < argc) {
            std::string sval = argv[++i];
            if (sval == "BGStat") systType = SYST_BGSTAT;
        } else if (arg == "--lumi" && i + 1 < argc) {
            lumi = std::stod(argv[++i]);
        } else if (arg == "--xsec" && i + 1 < argc) {
            xsec = std::stod(argv[++i]);
        } else if (arg == "--br" && i + 1 < argc) {
            br = std::stod(argv[++i]);
        }
    }

    std::vector<std::string> inputLists   = splitComma(inputListArg);
    std::vector<std::string> scaleConfigs = splitComma(scaleConfigArg);

    if (inputLists.size() != scaleConfigs.size()) {
        std::cerr << "Error: number of input lists (" << inputLists.size()
                  << ") != number of scale configs (" << scaleConfigs.size() << ")" << std::endl;
        return 1;
    }

    std::cout << std::string(70, '=') << std::endl;
    std::cout << "CalCPVFit" << std::endl;
    std::cout << std::string(70, '=') << std::endl;
    std::cout << "  Study      : " << studyName << std::endl;
    std::cout << "  RunPeriod  : " << runPeriod << std::endl;
    std::cout << "  Channel    : " << channel   << std::endl;
    std::cout << "  Luminosity : " << lumi      << " pb^-1" << std::endl;
    std::cout << "  Xsec       : " << xsec      << " pb"    << std::endl;
    std::cout << "  BR         : " << br         << std::endl;
    std::cout << "  SystType   : " << (systType == SYST_BGSTAT ? "BGStat" : "None") << std::endl;
    std::cout << "  Inputs (" << inputLists.size() << "):" << std::endl;
    for (size_t i = 0; i < inputLists.size(); i++) {
        std::cout << "    [" << i << "] " << inputLists[i]
                  << "  |  " << scaleConfigs[i] << std::endl;
    }
    std::cout << std::string(70, '=') << std::endl;

    // Output directory: output/StudyName/RunPeriod/Channel/
    std::string outDir       = "output/" + studyName + "/" + runPeriod + "/" + channel;
    std::string fitInputsDat = outDir + "/FitInputs_" + channel + "_" + runPeriod + ".dat";
    std::string fitResultTxt = outDir + "/FitResult_"  + channel + "_" + runPeriod + ".txt";
    mkdirRecursive(outDir);

    // Step 1 & 2: Load each (list, scaleconfig) pair into a single HistLoader
    // HistLoader accumulates histograms across multiple loads (Run periods / channels)
    HistLoader loader;
    loader.setLuminosity(lumi);
    loader.setXsec(xsec);
    loader.setBranchingFraction(br);

    for (size_t i = 0; i < inputLists.size(); i++) {
        std::cout << "\n[Load " << i+1 << "/" << inputLists.size() << "] "
                  << inputLists[i] << std::endl;

        ScaleConfigReader sc;
        if (!sc.load(scaleConfigs[i])) return 1;
        sc.printSummary();

        loader.loadScaleConfig(sc);
        if (!loader.loadFromList(inputLists[i])) return 1;
    }

    loader.printSummary();

    // Step 3: Build FitInputs
    std::cout << "\n[3] Building FitInputs..." << std::endl;
    FitInputsSet fitInputs = loader.buildFitInputs();

    for (int obs = 1; obs <= 13; obs++) {
        const FitInputs& fi = fitInputs.get(obs);
        if (fi.N_plus_obs == 0 && fi.N_minus_obs == 0) continue;
        fi.print();
    }
    fitInputs.writeDat(fitInputsDat);

    // Step 4: Fit
    std::cout << "\n[4] Running likelihood fits (O1~O13)..." << std::endl;
    CPVFitter fitter;
    fitter.setSystType(systType);
    std::vector<FitResult> results = fitter.fitAll(fitInputs);

    // Step 5: Output
    std::cout << "\n[5] Writing results..." << std::endl;
    fitter.printResults(results);
    fitter.writeResults(results, fitResultTxt);

    std::cout << "\nDone. Output: " << outDir << std::endl;
    return 0;
}
