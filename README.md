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
git checkout LowPtElectrons_prod
```

Build.
``` 
cd $CMSSW_BASE/src
scram b -j8
```

Add models from cms-data.
```
cd $CMSSW_BASE/external/$SCRAM_ARCH
git clone git@github.com:CMSBParking/RecoEgamma-ElectronIdentification.git data/RecoEgamma/ElectronIdentification/data
ls -l $CMSSW_BASE/external/$SCRAM_ARCH/data/RecoEgamma/ElectronIdentification/data/LowPtElectrons
```

Run.
``` 
cd $CMSSW_BASE/src/LowPtElectrons/LowPtElectrons/run
voms-proxy-init --voms cms
. test.sh
```
