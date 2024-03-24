import FWCore.ParameterSet.Config as cms

process = cms.Process("recAna")

# Load the services
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
import RecoTracker.TrackProducer.TrackRefitters_cff
process.FinalTrackRefitter = RecoTracker.TrackProducer.TrackRefitter_cfi.TrackRefitter.clone()
process.FinalTrackRefitter.src = cms.InputTag('generalTracks','','reRECO')
process.FinalTrackRefitter.TrajectoryInEvent = True
process.FinalTrackRefitter.NavigationSchool = ''
process.FinalTrackRefitter.TTRHBuilder = "WithAngleAndTemplate"
process.GlobalTag.globaltag = "133X_dataRun3_Prompt_v2"

# Load the input file containing RECO data
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/b/benjamin/public/Reco/RECO_3.root')
)

# Set the maximum number of events to process
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Define the analyzer module
process.recAna = cms.EDAnalyzer('RecHitPositionAnalyzer',
    # recHitCollection = cms.InputTag('generalTracks','','reRECO')
    recHitCollection = cms.InputTag("FinalTrackRefitter")
)

process.TFileService = cms.Service("TFileService", fileName = cms.string("recHits.root") )

# Path definition
process.p = cms.Path(process.FinalTrackRefitter+process.recAna)

