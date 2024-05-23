import CRABClient
from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferLogs = True
config.General.transferOutputs = True
config.General.workArea = 'tasks'
config.General.requestName = 'REQUESTNAME'
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.sendPythonFolder = True
config.JobType.psetName = './SiPixelTools/PixelTriplets/pxl_BPIX_FPIX_genIBC.py'
config.section_('Data')
config.Data.inputDataset = 'DASPATH'
config.Data.outputDatasetTag = 'OUTPUTNAME'
config.Data.publication = False
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.lumiMask = 'LUMIMASK'
config.Data.outLFNDirBase = 'OUTPUTDIR'
config.section_('Site')
config.Site.blacklist = ['T2_BR_SPRACE', 'T3_UK_London_QMUL']
config.Site.storageSite = 'T3_CH_PSI'
config.section_('User')
config.section_('Debug')
config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
