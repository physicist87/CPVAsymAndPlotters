import os
import sys
import subprocess
#mkdir -p output/SystematicStudy_v13/MuMu/ 
#./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName none  
#mkdir -p output/SystematicStudy_v13/MuMu/BGStat 
#./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName BGStat 

def main() :
    Channels = ["MuMu","ElEl","MuEl","DiLep"]
    #Channels = ["DiLep"]
    StudyName = ["SystematicStudy_v15_NewJER"]
    StudyName = ["SystematicStudy_v15"]
    StudyName = ["SystematicStudy_v17"]
    Systematic = ["none",
                 "Central","LepIDUp","LepIDDown","LepIsoUp","LepIsoDown",
                 "LepTrackUp","LepTrackDown","LepRecoUp","LepRecoDown",
                 "TrigSFUp","TrigSFDown",
                 "PileUpUp","PileUpDown","JetEnUp","JetEnDown",
                 "JetResUp","JetResDown",
                 "BTagSFBHadUp","BTagSFBHadDown", "BTagSFCHadUp","BTagSFCHadDown","BTagSFLFUp","BTagSFLFDown",
                 "BTagEffBHadUp","BTagEffBHadDown", "BTagEffCHadUp","BTagEffCHadDown", "BTagEffLFUp","BTagEffLFDown",
                 "FactReno_1","FactReno_2","FactReno_3","FactReno_4","FactReno_5","FactReno_6","FactReno_7","FactReno_8",
                 "FragmentCentral","FragmentUp","FragmentDown","FragmentPeterson","DecayTableUp","DecayTableDown",
                 "MuonEnUp","MuonEnDown",
                 "EleScSmSysDownDown","EleScSmSysDownUp","EleScSmSysUpDown","EleScSmSysUpUp",
                 "FSRUp","FSRDown",
                 "TopMass1755",
                 "TopMass1735",
                 "TopMass1715",
                 "TopMass1695",
                 "ISRUp","ISRDown",#"TuneUp","TuneDown","Herwig","EvtGen",#"JetPtResolUp","JetPtResolDown"
                 "TuneUp","TuneDown","hdampUp","hdampDown",#"JetPtResolUp","JetPtResolDown"
                 "Herwig",#"EvtGen",
                 'erdON',
                 'QCDCRTune',
                 'GluoneMoveCRTune',
                 'GluoneMoveCRTune_erdON',
    "BGStat",
    "METSysUp","METSysDown",
    "BGNormUp","BGNormDown",
    "MuonEnUp","MuonEnDown",
    "L1PreFireUp","L1PreFireDown"
    ]
    #Systematic = []
    #for ipdf in range(1,103):
    #    isyspdf = "PDF_%s"%(ipdf)
    #    Systematic.append(isyspdf)
    #Systematic = ["none","JetResUp","JetResDown"]
    #Systematic = ["METSysUp","METSysDown"]
    #Systematic = ["BGNormUp","BGNormDown"]
    Systematic = ["TopPtSys"]
    #Systematic = ["Herwig"]
    #Systematic = ["MuonEnUp","MuonEnDown"]
    #Systematic = ["none"]
    #Systematic = ["L1PreFireUp","L1PreFireDown"]
    Obs = []
    for iobs in range(1,14):
        print iobs
        Obs.append("O%s"%(iobs))
    for istudy in StudyName:
        for ich in Channels:
            chan = ''
            if ich == "MuMu" : chan = "dimu"
            elif ich == "ElEl" : chan = "diel"
            elif ich == "MuEl" : chan = "muel"
            elif ich == "DiLep" : chan = "dilep"
            else: 
                print "Check out channel !!!!!!!!!" 
                break
            for isys in Systematic:
                OutputDir = ""
                InputDAT = ""
                ConfFile = ""
                if isys != "none" and isys != "None" and isys != "BGStat":
                    OutputDir = "%s/%s/%s"%(istudy,ich,isys)
                    InputDAT = "%s/%s/%s/Asymmetry_%s_%s_%s.dat"%(istudy,ich,isys,chan,istudy,isys)
                    ConfFile = "%s/%s/%s/CP_Inputs_%s_%s_%s.config"%(istudy,ich,isys,chan,istudy,isys)
                else :
                    OutputDir = "%s/%s"%(istudy,ich)
                    InputDAT = "%s/%s/Asymmetry_%s_%s.dat"%(istudy,ich,chan,istudy)
                    ConfFile = "%s/%s/CP_Inputs_%s_%s.config"%(istudy,ich,chan,istudy)
                mkDir = 'mkdir -p output/%s'%(OutputDir)
                os.system(mkDir)
                for iobs in Obs:
                    cmd = "./Asym_Calc -channel %s -studyName %s -obs %s -outputDir %s -inputDAT %s -config %s -systName %s"%(ich, istudy, iobs, OutputDir, InputDAT, ConfFile, isys)
                    print(cmd)
                    os.system(cmd)
                    pass## Obs 
                pass ### Sys
            pass ## Channel 
        pass ## Study Loop
    pass # End of Main Function
main()  
