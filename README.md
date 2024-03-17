# RecHitAnalyser

Installation:
```
cmsrel CMSSW_13_3_1
cd CMSSW_13_3_1/src
cmsenv
git init
git clone https://github.com/BenjaminRS/RecHitAnalyser.git
cd RecHitAnalyser
git checkout dev
cd ../
scram b -j 16
cd RecHitAnalyser
```

Running:
```
time cmsRun runRecHitPosAna.py
```

Example from the root TH2D:
![TH2D of the xy global position of RecHits](http://benjamin.web.cern.ch/benjamin/Docs/GlobalRecHitXY.png)
