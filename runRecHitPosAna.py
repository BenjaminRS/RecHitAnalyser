import FWCore.ParameterSet.Config as cms

process = cms.Process("recAna")

# Load the services
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = "133X_dataRun3_Prompt_v2"
process.load('RecoLocalTracker.SiStripRecHitConverter.SiStripRecHitConverter_cfi')

#Loading the Track Reffiter:
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
import RecoTracker.TrackProducer.TrackRefitters_cff
process.FinalTrackRefitter = RecoTracker.TrackProducer.TrackRefitter_cfi.TrackRefitter.clone()
process.FinalTrackRefitter.src = cms.InputTag('generalTracks','','reRECO')
process.FinalTrackRefitter.TrajectoryInEvent = True
# process.FinalTrackRefitter.useHitsSplitting = cms.bool(True) #This crashes due to an assert
process.FinalTrackRefitter.NavigationSchool = ''
process.FinalTrackRefitter.TTRHBuilder = "WithAngleAndTemplate"

# Load the input file containing RECO data
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/b/benjamin/public/Reco/RECO_3.root')
)

# Set the maximum number of events to process
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Run the SiStripRecHitConverter in order to get the matched SiStrip modules and the z coordinate for those:
process.SiStripRHConv = cms.EDProducer("SiStripRecHitConverter",    
   ClusterProducer = cms.InputTag("siStripClusters"),    
   rphiRecHits = cms.string("rphiRecHit"),    
   stereoRecHits = cms.string("stereoRecHit"),    
   matchedRecHits = cms.string("matchedRecHit"),    
   doMatching = cms.bool(True)
)

# Define the analyzer module
process.recAna = cms.EDAnalyzer('RecHitPositionAnalyzer',
    # recHitCollection = cms.InputTag('generalTracks','','reRECO')
    recHitCollection = cms.InputTag("FinalTrackRefitter"),
    siMatchHitCollection = cms.InputTag("SiStripRHConv","matchedRecHit","recAna" )
)

process.TFileService = cms.Service("TFileService", fileName = cms.string("recHits.root") )

# # In order to find the event content after running:
# process.load('Configuration.StandardSequences.EndOfProcess_cff')
# process.Output = cms.OutputModule("PoolOutputModule",
#     fileName = cms.untracked.string('file:TEST.root'),
#     splitLevel = cms.untracked.int32(0)
# )
# process.Out = cms.EndPath(process.Output)

# Path definition
process.p = cms.Path(process.SiStripRHConv+process.FinalTrackRefitter+process.recAna)

