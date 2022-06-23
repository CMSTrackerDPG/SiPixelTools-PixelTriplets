# Auto generated configuration file
# using: 
# Revision: 1.19 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: RECO -s RAW2DIGI,L1Reco,RECO --data --scenario pp --conditions 123X_dataRun3_Express_v9 --era Run3 --process NTUPLE --eventcontent RECO --datatier RECO --filein /store/express/Run2022A/ExpressPhysics/FEVT/Express-v1/000/353/060/00000/22202638-77fc-41c1-91d1-aa0ff0c4c20a.root --python_filename=run_Resolution_ReReco_Data_cfg.py -n 10
import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import HLTrigger.HLTfilters.hltHighLevel_cfi

from Configuration.Eras.Era_Run3_cff import Run3

process = cms.Process('NTUPLE',Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.load("RecoVertex.BeamSpotProducer.BeamSpot_cff")
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
process.load("TrackingTools.TransientTrack.TransientTrackBuilder_cfi")
process.load("RecoTracker.MeasurementDet.MeasurementTrackerEventProducer_cfi")


options = VarParsing.VarParsing ('analysis')
options.inputFiles = ["/store/express/Run2022A/ExpressPhysics/FEVT/Express-v1/000/353/060/00000/22202638-77fc-41c1-91d1-aa0ff0c4c20a.root"]
options.maxEvents = -1
options.outputFile = "tree.root"
options.parseArguments()


# Input source
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring (options.inputFiles),
    secondaryFileNames = cms.untracked.vstring()
)
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(options.maxEvents)
)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    annotation = cms.untracked.string('RECO nevts:10'),
    name = cms.untracked.string('Applications'),
    version = cms.untracked.string('$Revision: 1.19 $')
)

# Output definition

process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('RECO_RAW2DIGI_L1Reco_RECO.root'),
    outputCommands = process.RECOEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '123X_dataRun3_Express_v9', '')

# Path and EndPath definitions
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RECOoutput_step = cms.EndPath(process.RECOoutput)

# Schedule definition
process.schedule = cms.Schedule(process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.endjob_step,process.RECOoutput_step)
from PhysicsTools.PatAlgos.tools.helpers import associatePatAlgosToolsTask
associatePatAlgosToolsTask(process)


#process.MeasurementTrackerEvent.pixelClusterProducer = 'ALCARECOSiPixelCalSingleMuon'
#process.MeasurementTrackerEvent.stripClusterProducer = 'ALCARECOSiPixelCalSingleMuon'
process.MeasurementTrackerEvent.pixelClusterProducer = 'siPixelClusters'
process.MeasurementTrackerEvent.stripClusterProducer = 'siStripClusters'
process.MeasurementTrackerEvent.inactivePixelDetectorLabels = cms.VInputTag()
process.MeasurementTrackerEvent.inactiveStripDetectorLabels = cms.VInputTag()


#process.TrackRefitter.src = 'ALCARECOSiPixelCalSingleMuon'
process.TrackRefitter.src = 'generalTracks'
process.TrackRefitter.TrajectoryInEvent = True
process.TrackRefitter_step = cms.Path(process.offlineBeamSpot * process.MeasurementTrackerEvent * process.TrackRefitter)

process.HLTFilter = HLTrigger.HLTfilters.hltHighLevel_cfi.hltHighLevel.clone(
    HLTPaths = ['HLT_*PixelCluster*'],
    throw = False #dont throw except on unknown path names
)
process.hltFilter_step = cms.Path(process.HLTFilter)

#process.TrackRefitter_step = cms.Path(process.MeasurementTrackerEvent*process.TrackRefitter)

#------------------------------------------
#  Define your Analyzer(s) here
#------------------------------------------

# BPix Resolution
process.BPixResolution_Template = cms.EDAnalyzer('Pixel_BPix_phase1',
                                                 triggerSource = cms.InputTag('TriggerResults::RECO'),
                                                 ttrhBuilder = cms.string('WithAngleAndTemplate'),
                                                 #track_collection = cms.string('ALCARECOSiPixelCalSingleMuon')#generalTracks
                                                 track_collection = cms.string('generalTracks'),
)
process.BPixResolution_Generic = process.BPixResolution_Template.clone(
    ttrhBuilder = cms.string('WithTrackAngle')
)

# FPix Resolution
process.FPixResolution_Template = cms.EDAnalyzer('Pixel_FPix_phase1',
                                                 triggerSource = cms.InputTag('TriggerResults::RECO'),
                                                 ttrhBuilder = cms.string('WithAngleAndTemplate'),
                                                 #track_collection=cms.string('ALCARECOSiPixelCalSingleMuon'),#generalTracks
                                                 track_collection=cms.string('generalTracks'),
                                                 doBPix = cms.bool(False),
                                                 doFPix = cms.bool(True)
)
process.FPixResolution_Generic = process.FPixResolution_Template.clone(
    ttrhBuilder = cms.string('WithTrackAngle')
)

# TFileService used for both BPix/FPix resolution
process.TFileService = cms.Service('TFileService',
    fileName = cms.string(options.outputFile),
)

# Paths
process.BPixResolution_step = cms.Path(process.BPixResolution_Template*process.BPixResolution_Generic)
process.FPixResolution_step = cms.Path(process.FPixResolution_Template*process.FPixResolution_Generic)

# Schedule definition
process.schedule = cms.Schedule(
    process.raw2digi_step,
    process.L1Reco_step,
    process.reconstruction_step,
    process.hltFilter_step,
    process.TrackRefitter_step,
    process.FPixResolution_step,
    process.BPixResolution_step
    )

# end of insert to cmsDriver script

# Customisation from command line

#Have logErrorHarvester wait for the same EDProducers to finish as those providing data for the OutputModule
from FWCore.Modules.logErrorHarvester_cff import customiseLogErrorHarvesterUsingOutputCommands
process = customiseLogErrorHarvesterUsingOutputCommands(process)

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion
