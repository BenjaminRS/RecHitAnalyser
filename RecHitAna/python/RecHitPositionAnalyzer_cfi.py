import FWCore.ParameterSet.Config as cms

RecHitPositionAnalyzer = cms.EDAnalyzer('RecHitPositionAnalyzer',
    recHitCollection = cms.InputTag('generalTracks','','reRECO')
)
