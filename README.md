# Pixel Triplet Code

Measure hit resolution in BPix using the triplet method.
The main code is ``pxl_BPIX_FPIX_genIBC.py`` which uses the ``Pixel_FPix_phase1.cc`` and ``Pixel_BPix_phase1.cc`` files in the ``src`` folder. 

The Triplet method uses tracks with pt>12 GeV for the barrel and pt>4 GeV for the forwards disk with at least 3 hits in the barrel layers/disks. Then the code predicts the position in one layer/disk with the hits from two other layers/disks. There are two reconstruction methods. The generic method is a simple method based on the track angle and position. The template method is based on detailed cluster shape simulations. The difference from the predicted to the actual hit gives the residual distribution.

## Software setup

Prepare your working directory with CMSSW

```
cmsrel CMSSW_14_0_4
cd CMSSW_14_0_4/src
cmsenv
git clone git@github.com:CMSTrackerDPG/SiPixelTools-PixelTriplets.git SiPixelTools/PixelTriplets
scram b -j 8
cd SiPixelTools/PixelTriplets/
```
Check that the CMSSW version matches the version used for the production of the file.

## Running the code locally

Run with cmsRun

```
cmsRun pxl_BPIX_FPIX_genIBC.py
```

Command line options:

- `inputFiles=FILENAME[,FILENAME,..]`
- `outputFile=OUTPUTFILE.root`

The global Tag, listed in the config in the Data Aggregation System, has to be changed in `process.GlobalTag` in `pxl_BPIX_FPIX_genIBC.py`.


Remarks:

- Gen+IBC method can be activated by uncommenting the line `process.PixelCPEGenericESProducer.IrradiationBiasCorrection = True` in `pxl_BPIX_FPIX_genIBC.py`

## Running the code with CRAB

The code is best run with CRAB. One example of a CRAB submit file is included.
Change following lines:
 
- `config.General.requestName` 
- `config.JobType.psetName`  path to the `pxl_BPIX_FPIX_genIBC.py` code to be run
- `config.Data.inputDataset`  DAS path to file
- `config.Data.outputDatasetTag`
- `config.Data.lumiMask` take the current lumi mask
- `config.Data.outLFNDirBase`  Path to output directory
- `config.Site.storageSite`  If you have access to the PSI tier 3 this is `T3_CH_PSI`.

``requestName`` is the name for the CRAB job, ``psetName`` is the code to run, which in our case is ``pxl_BPIX_FPIX_genIBC.py``, ``inputDataset`` is the DAS path to the dataset, ``outputDatasetTag`` is the name of the output, ``lumiMask`` should be the most up to data lumimask for the year of data processing, to be found in [Collisions24](https://cms-service-dqmdc.web.cern.ch/CAF/certification/Collisions24/) for 2024, ``outLFNDirBase`` is the path to the directory where you want the output to be stored and  ``storageSite`` needs the tier3 you have access to.

Make sure that `process.GlobalTag` in ``pxl_BPIX_FPIX_genIBC.py`` is correct before running the CRAB jobs.

## Downstream code

The plotting code for FPix and BPix is [here](https://gitlab.cern.ch/pbaertsc/pixelresolution)
The code for making trend-plots is [here](https://gitlab.cern.ch/pbaertsc/trend-plotter)