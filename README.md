# Pixel Triplet Code

Measure hit resolution in BPix using the triplet method.


## Software setup

Prepare your working directory with CMSSW

```
export SCRAM_ARCH=slc7_amd64_gcc900
cmsrel CMSSW_13_0_0
cd CMSSW_13_0_0/src
cmsenv
git clone -b Upgrade git@github.com:PascalBaertschi/SiPixelTools-PixelTriplets.git SiPixelTools/PixelTriplets
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




## Downstream code

The plotting code for FPix and BPix is [here](https://gitlab.cern.ch/pbaertsc/pixelresolution)
