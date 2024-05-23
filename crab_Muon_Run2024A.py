import CRABClient
from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferLogs = True
config.General.transferOutputs = True
config.General.workArea = 'tasks'
config.General.requestName = 'Muon0_Run2024A-SiPixelCalSingleMuonTight-PromptReco-v1'
config.section_('JobType')
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/work/pbaertsc/pixel/CMSSW_14_0_4/src/SiPixelTools/PixelTriplets/pxl_BPIX_FPIX_genIBC.py'
config.section_('Data')
config.Data.inputDataset = '/Muon0/Run2024A-SiPixelCalSingleMuonTight-PromptReco-v1/ALCARECO'
config.Data.outputDatasetTag = 'Muon0_Run2024A-SiPixelCalSingleMuonTight-PromptReco-v1'
config.Data.publication = False
config.Data.unitsPerJob = 1
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.lumiMask = 'Cert_Collisions2024_378981_380115_Golden.json'
config.Data.outLFNDirBase = '/store/user/pbaertsc/pixel/'
config.section_('Site')
config.Site.blacklist = ['T2_BR_SPRACE', 'T3_UK_London_QMUL']
config.Site.storageSite = 'T3_CH_PSI'
config.section_('User')
config.section_('Debug')
config.Debug.extraJDL = ['+CMS_ALLOW_OVERFLOW=False']
