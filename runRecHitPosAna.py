import FWCore.ParameterSet.Config as cms

process = cms.Process("recAna")

# Load the services
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.Services_cff")
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = "133X_dataRun3_Prompt_v2"

# Load the input file containing RECO data
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/b/benjamin/public/Reco/RECO_3.root')
)

# Set the maximum number of events to process
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

# Define the analyzer module
process.recAna = cms.EDAnalyzer('RecHitPositionAnalyzer',
    recHitCollection = cms.InputTag('generalTracks','','reRECO')
)

process.TFileService = cms.Service("TFileService", fileName = cms.string("recHits.root") )

# Path definition
process.p = cms.Path(process.recAna)

