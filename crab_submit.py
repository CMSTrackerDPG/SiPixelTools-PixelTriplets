from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferLogs = True
config.General.transferOutputs = True
config.General.workArea = 'tasks'
config.General.requestName = 'NAME_OF_SAMPLE'
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.sendPythonFolder = True
config.JobType.psetName = 'PATH_TO_pxl_BPIX_FPIX_genIBC.py'
config.section_('Data')
config.Data.inputDataset = 'DATASET'
config.Data.outputDatasetTag = 'TAG_OF_OUTPUT'
config.Data.publication = False
config.Data.unitsPerJob = 1
config.Data.lumiMask = 'JSON_FILE'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = 'PATH_TO_OUTDIR'
config.section_('Site')
config.Site.blacklist = ['T2_BR_SPRACE', 'T3_UK_London_QMUL']
config.Site.storageSite = 'ADD_T3'
config.section_('User')
config.section_('Debug')
config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
