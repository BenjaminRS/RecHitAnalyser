# RecHitAnalyser

Installation:
```
cmsrel CMSSW_13_3_1
cd CMSSW_13_3_1/src
cmsenv
git init
git clone https://github.com/BenjaminRS/RecHitAnalyser.git
cd RecHitAnalyser
git checkout main
cd ../
scram b -j 16
cd RecHitAnalyser
```

Running:
```
cd RecHitAnalyser
time cmsRun runRecHitPosAna.py
```

Example from the root TH2D:
![TH2D of the xy Global Position of RecHits](http://benjamin.web.cern.ch/benjamin/Docs/GlobalRecHitXY.png)
![TH2D of the rz Global Position of RecHits](http://benjamin.web.cern.ch/benjamin/Docs/GlobalRecHitRZ.png)


The analyser is running over data CMS collected (Run 370293) and then reconstructed with the following command:
```
cmsrel CMSSW_13_3_1
cd CMSSW_13_3_1/src
cmsenv
cmsDriver.py step3\
 --conditions 133X_dataRun3_Prompt_v2\
 --customise Configuration/DataProcessing/RecoTLR.customisePostEra_Run3\
 --data --datatier RECO --era Run3 --eventcontent RECO\
 --filein /store/data/Run2023D/EphemeralHLTPhysics0/RAW/v1/000/370/293/00000/003420ea-0f24-49f4-a69f-0528ffc517c2.root\
 --fileout file:RECO_3.root\
 --nThreads 4 --no_exec --number 3 --process reRECO --python_filename runRECO.py\
 --scenario pp --step RAW2DIGI,L1Reco,RECO
time cmsRun runRECO.py
```
