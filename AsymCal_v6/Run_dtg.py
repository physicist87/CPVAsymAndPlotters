import os
import sys
import subprocess
#mkdir -p output/SystematicStudy_v13/MuMu/ 
#./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName none  
#mkdir -p output/SystematicStudy_v13/MuMu/BGStat 
#./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName BGStat 

def main() :
    Channels = ["MuMu","ElEl","MuEl"]
    StudyName = ["SystematicStudy_v22_dtG"]
    Systematic = [   
                'dtG_0','dtG_0p5207','dtG_1p0415','dtG_2p60364','dtG_m0p5207','dtG_m1p0415','dtG_m2p60364'
    ]
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
