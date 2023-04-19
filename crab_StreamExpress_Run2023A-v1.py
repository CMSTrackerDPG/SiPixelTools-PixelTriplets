from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferLogs = True
config.General.transferOutputs = True
config.General.workArea = 'tasks'
config.General.requestName = 'StreamExpress_Run2023A-SiPixelCalSingleMuonTight-Express-v1'
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.sendPythonFolder = True
config.JobType.psetName = '/work/pbaertsc/pixel/CMSSW_12_4_10/src/SiPixelTools/PixelTriplets/pxl_BPIX_FPIX_genIBC.py'
config.section_('Data')
config.Data.inputDataset = '/StreamExpress/Run2023A-SiPixelCalSingleMuonTight-Express-v1/ALCARECO'
config.Data.outputDatasetTag = 'StreamExpress_Run2023A-SiPixelCalSingleMuonTight-Express-v1'
config.Data.publication = False
config.Data.unitsPerJob = 1
config.Data.lumiMask = 'Cert_Collisions2023A_365753_365889_900GeV_DCSOnly_TkPx.json'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.outLFNDirBase = '/store/user/pbaertsc/pixel/'
config.section_('Site')
config.Site.blacklist = ['T2_BR_SPRACE', 'T3_UK_London_QMUL']
config.Site.storageSite = 'T3_CH_PSI'
config.section_('User')
config.section_('Debug')
config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
