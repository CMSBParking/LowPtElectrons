# LowPtElectrons (production branch)

Setup env and create release area.
```
cmsrel CMSSW_9_4_10
cd CMSSW_9_4_10/src
cmsenv
```

Some initialisation.
```
git cms-init
git remote add bainbrid git@github.com:bainbrid/cmssw.git
git remote -v
```

Merge topic for new electron reco (production branch).
```
git cms-merge-topic bainbrid:LowPtElectrons_prod
```

Checkout ntuplizer code.
```
mkdir $CMSSW_BASE/src/LowPtElectrons
cd $CMSSW_BASE/src/LowPtElectrons
git clone git@github.com:bainbrid/LowPtElectrons.git
cd $CMSSW_BASE/src/LowPtElectrons/LowPtElectrons
git remote add bainbrid git@github.com:bainbrid/LowPtElectrons.git
git fetch bainbrid LowPtElectrons_prod
git checkout -b LowPtElectrons_prod bainbrid/LowPtElectrons_prod
```

Build.
``` 
cd $CMSSW_BASE/src
scram b -j8
```

Add models from cms-data.
```
cd $CMSSW_BASE/externals/$SCRAM_ARCH
git clone git@github.com:bainbrid/RecoEgamma-ElectronIdentification.git data/RecoEgamma/ElectronIdentification/data
cd $CMSSW_BASE/external/$SCRAM_ARCH/data/RecoEgamma/ElectronIdentification/data
git fetch origin LowPtElectrons
git checkout LowPtElectrons
```

Run.
``` 
cd $CMSSW_BASE/src/LowPtElectrons/LowPtElectrons/run
voms-proxy-init --voms cms
cmsRun mc_features.py inputFiles=root://cms-xrd-global.cern.ch//store/cmst3/group/bpark/BToKee_Pythia_PUMix_18_03_18_180318_112206_0000/BToKee_PUMix_10.root maxEvents=1
```

