# Pixel Triplet Code

Measure hit resolution in BPix using the triplet method.


## Software setup

Prepare your working directory with CMSSW

```
export SCRAM_ARCH=slc7_amd64_gcc900
cmsrel CMSSW_12_1_1
cd CMSSW_12_1_1/src
cmsenv
git clone https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets.git SiPixelTools/PixelTriplets
scram b -j 8
cd SiPixelTools/PixelTriplets/
```
Check that the CMSSW version matches the version used for the production of the file.

## Running the code

Run with cmsRun

```
cmsRun pxl_BPIX_FPIX_genIBC.py
```

Command line options:

- `inputFiles=FILENAME[,FILENAME,..]`
- `outputFile=OUTPUTFILE.root`


Your can set the GT with:

```
cmsRun pxl.py GlobalTag='auto:phase1_2017_realistic'
```

Remarks:

- Gen+IBC method can be activated by uncommenting the line `process.PixelCPEGenericESProducer.IrradiationBiasCorrection = True` in `pxl_BPIX_FPIX_genIBC.py`

## Running the code with CRAB

An example for a CRAB submit file is included. Change following lines:
 
- `config.General.requestName` 
- `config.Data.inputDataset` DAS path to file
- `config.Data.outputDatasetTag`
- `config.Data.outLFNDirBase` Path to output directory
- `config.Site.storageSite` If you have access to the PSI tier 3 this is `T3_CH_PSI`.

In `pxl_BPIX_FPIX_genIBC.py` change the `process.GlobalTag`.

ATTENTION: At the moment the pt cut for BPix and FPix is set to 2 GeV. If this should be changed this has to be done in the `Pixel_BPix_phase1.cc` and `Pixel_FPix_phase1.cc`.



## Downstream code

The plotting code for FPix and BPix is [here](https://gitlab.cern.ch/pbaertsc/pixelresolution)
