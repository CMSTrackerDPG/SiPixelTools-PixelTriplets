import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import HLTrigger.HLTfilters.hltHighLevel_cfi

from Configuration.StandardSequences.Eras import eras

process = cms.Process('NTUPLE',eras.Run3)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
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
#options.inputFiles = ["/store/express/Run2022A/ExpressPhysics/FEVT/Express-v1/000/353/060/00000/22202638-77fc-41c1-91d1-aa0ff0c4c20a.root"]#only needed for running local
#options.inputFiles = ["root://eoscms.cern.ch//eos/cms/store/data/Run2022A/SingleMuon/ALCARECO/SiPixelCalSingleMuonLoose-PromptReco-v1/000/353/060/00000/6a14e869-712c-4a37-826c-6986b3cd53f9.root"]
options.inputFiles = ["root://eoscms.cern.ch//eos/cms/store/express/Run2022A/StreamExpress/ALCARECO/SiPixelCalSingleMuon-Express-v1/000/352/417/00000/e9bd6fa7-1099-456c-b5a2-859fc338103b.root"]
options.maxEvents = -1
options.outputFile = "tree.root"
options.parseArguments()
#root://cms-xrd-global.cern.ch/

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
########## ACTIVATE THIS FOR RUNNING GEN+IBC METHOD ##########
#process.PixelCPEGenericESProducer.IrradiationBiasCorrection = True # gen + IBC
##############################################################
process.RECOoutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    fileName = cms.untracked.string('RECO_RAW2DIGI_L1Reco_RECO.root'),
    outputCommands = process.RECOEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# Additional output definition

# Other statements
from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, '120X_dataRun3_Prompt_Candidate_2021_11_22_18_14_35', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '120X_dataRun3_Express_v2', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '121X_mcRun3_2021_realistic_forpp900GeV_v6', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '121X_dataRun3_v13', '')
process.GlobalTag = GlobalTag(process.GlobalTag, '123X_dataRun3_Express_v8', '')
#process.GlobalTag = GlobalTag(process.GlobalTag, '123X_dataRun3_Prompt_v8', '')
"""
process.GlobalTag.toGet = cms.VPSet(
  cms.PSet(record = cms.string("TrackerAlignmentRcd"),
           tag = cms.string("TrackerAlignment_EOY21_v1"),
          )
)
"""
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


# Insert resolution stuff here

# Refitter

#process.MeasurementTrackerEvent.pixelClusterProducer = 'ALCARECOSiPixelCalSingleMuonLoose'
#process.MeasurementTrackerEvent.stripClusterProducer = 'ALCARECOSiPixelCalSingleMuonLoose'
process.MeasurementTrackerEvent.pixelClusterProducer = 'ALCARECOSiPixelCalSingleMuon'
process.MeasurementTrackerEvent.stripClusterProducer = 'ALCARECOSiPixelCalSingleMuon'
#process.MeasurementTrackerEvent.pixelClusterProducer = 'siPixelClusters'
#process.MeasurementTrackerEvent.stripClusterProducer = 'siStripClusters'
process.MeasurementTrackerEvent.inactivePixelDetectorLabels = cms.VInputTag()
process.MeasurementTrackerEvent.inactiveStripDetectorLabels = cms.VInputTag()


#process.TrackRefitter.src = 'ALCARECOSiPixelCalSingleMuonLoose'
process.TrackRefitter.src = 'ALCARECOSiPixelCalSingleMuon'
#process.TrackRefitter.src = 'generalTracks'
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
                                                 #track_collection = cms.string('ALCARECOSiPixelCalSingleMuonLoose')#generalTracks
                                                 track_collection = cms.string('ALCARECOSiPixelCalSingleMuon')#generalTracks
                                                 #track_collection = cms.string('generalTracks'),
)
process.BPixResolution_Generic = process.BPixResolution_Template.clone(
    ttrhBuilder = cms.string('WithTrackAngle')
)

# FPix Resolution
process.FPixResolution_Template = cms.EDAnalyzer('Pixel_FPix_phase1',
                                                 triggerSource = cms.InputTag('TriggerResults::RECO'),
                                                 ttrhBuilder = cms.string('WithAngleAndTemplate'),
                                                 #track_collection=cms.string('ALCARECOSiPixelCalSingleMuonLoose'),#generalTracks
                                                 track_collection=cms.string('ALCARECOSiPixelCalSingleMuon'),#generalTracks
                                                 #track_collection=cms.string('generalTracks'),
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
    #process.raw2digi_step,
    #process.L1Reco_step,
    #process.reconstruction_step,
    process.hltFilter_step,
    process.TrackRefitter_step,
    process.FPixResolution_step,
    process.BPixResolution_step
    )

# end of insert to cmsDriver script

#do not add changes to your config after this point (unless you know what you are doing)
#from FWCore.ParameterSet.Utilities import convertToUnscheduled
#process=convertToUnscheduled(process)


# Customisation from command line

# Add early deletion of temporary data products to reduce peak memory need
from Configuration.StandardSequences.earlyDeleteSettings_cff import customiseEarlyDelete
process = customiseEarlyDelete(process)
# End adding early deletion

#processDumpFile = open('fullconfig.txt', 'w')
#print (process.dumpPython(),file=processDumpFile)
#processDumpFile.close()
