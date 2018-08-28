import FWCore.ParameterSet.Config as cms

features = cms.EDAnalyzer("TrackerElectronsFeatures",
                          isMC = cms.bool(True),
                          prescaleFakes = cms.double(0.08),
                          beamspot = cms.InputTag("offlineBeamSpot"),
                          genParticles = cms.InputTag("genParticles"),
                          gsfTracks = cms.InputTag("electronGsfTracks"),
                          #gsfTracks = cms.InputTag("electronGsfTracks"),
                          PFGsfTracks = cms.InputTag("pfTrackElec"),
                          PFBlocks = cms.InputTag('particleFlowBlock'),
                          PFElectrons = cms.InputTag("particleFlowEGamma"),
                          gedElectronCores = cms.InputTag("gedGsfElectronCores"),
                          gedElectrons = cms.InputTag("gedGsfElectrons"),
                          preId = cms.InputTag("trackerDrivenElectronSeeds","preid"),
                          association = cms.InputTag("trackingParticleRecoTrackAsssociation"),
                          trkCandidates = cms.InputTag("electronCkfTrackCandidates"),
                          eleSeeds = cms.InputTag("trackerDrivenElectronSeeds", 'SeedsForGsf'),
                          associator = cms.InputTag("quickTrackAssociatorByHits"),
                          trackingParticles = cms.InputTag("mix", "MergedTrackTruth"),
                          hitAssociation = cms.bool(False),
                          drMax = cms.double(0.02),
                          #pileup = cms.InputTag("addPileupInfo"),
                          #vertices = cms.InputTag("offlinePrimaryVertices"),
                          #generalTracks = cms.InputTag("generalTracks"),
                          )
