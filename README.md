# Pixel Triplet Code

Measure hit resolution in BPix using the triplet method.


## Software setup

Prepare your working directory with CMSSW

```
export SCRAM_ARCH=slc7_amd64_gcc820
cmsrel CMSSW_11_1_4
cd CMSSW_11_1_4/src
cmsenv
git clone https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets.git SiPixelTools/PixelTriplets
scram b -j 8
cd SiPixelTools/PixelTriplets/
```

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


## Downstream code

### BPix

The plotting code for BPix is [here](https://github.com/dbrzhechko/BPixResolution)

### FPix

The plotting code for FPix is collected [on GitLab](https://gitlab.cern.ch/koschwei/fpixresolution)

### Batch processing

[Additional code](https://gitlab.cern.ch/koschwei/fpixresolution/-/tree/master/gc) for running the Pixel Triplet code on batch systems. This is implemented using [grid-control](https://github.com/grid-control/grid-control), which can be used with may different batch systems. The linked desciption is for the SLURM cluster used on the T3@PSI.
