#!/bin/tcsh
#./Asym_Calc -isComb true -inputFile MuMu SystematicStudy_v12/MuMu/SystematicStudy_v12_MuMu.list -inputFile ElEl SystematicStudy_v12/ElEl/SystematicStudy_v12_ElEl.list -inputFile MuEl SystematicStudy_v12/MuEl/SystematicStudy_v12_MuEl.list  -outputDir SystematicStudy_v12/DiLep/ -scaleOpt MuMu SetScaleMuMu.cfg ElEl SetSetSaleElEl.cfg MuEl SetScaleMuEl.cfg
#./Asym_Calc -channel DiLep -studyName SystematicStudy_v12 -obs O1 -outputDir SystematicStudy_v12/DiLep/ -inputDAT SystematicStudy_v12/DiLep/Asymmetry_dilep_SystematicStudy_v12.dat -config SystematicStudy_v12/DiLep/CP_Inputs_dilep_SystematicStudy_v12.config 
#./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config
mkdir -p output/SystematicStudy_v13/MuMu/ 
./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/ -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName none  
mkdir -p output/SystematicStudy_v13/MuMu/BGStat 
./Asym_Calc -channel MuMu -studyName SystematicStudy_v13 -obs O1 -outputDir SystematicStudy_v13/MuMu/BGStat -inputDAT SystematicStudy_v13/MuMu/Asymmetry_dimu_SystematicStudy_v13.dat -config SystematicStudy_v13/MuMu/CP_Inputs_dimu_SystematicStudy_v13.config -systName BGStat 

