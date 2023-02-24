Tutorial for producing pixel resolution plots

## PixelTriplet code

CMSSW is required to run the PixelTriplet code, you can follow the setup:
```
export SCRAM_ARCH=slc7_amd64_gcc900
cmsrel CMSSW_12_4_10
cd CMSSW_12_4_10/src
cmsenv
git clone https://github.com/CMSTrackerDPG/SiPixelTools-PixelTriplets.git SiPixelTools-PixelTriplets
cd SiPixelTools-PixelTriplets/
git checkout -b TrackerTraining remotes/origin/TrackerTraining
scram b -j 8
```
The main code is ``pxl_BPIX_FPIX_genIBC.py`` which uses the ``Pixel_FPix_phase1.cc`` and ``Pixel_BPix_phase1.cc`` files in the ``src`` folder. The Triplet method uses tracks with pt>12 GeV for the barrel and pt>4 GeV for the forwards disk with at least 3 hits in the barrel layers/disks. Then the code predicts the position in one layer/disk with the hits from two other layers/disks. There are two reconstruction methods. The generic method is a simple method based on the track angle and position. The template method is based on detailed cluster shape simulations. The difference from the predicted to the actual hit gives the residual distribution, which is fitted by a student t-fit. The width of this fit gives the pixel resolution.

First we will run the code locally. We are going to use the Muon dataset from Run2022F (``/Muon/Run2022F-SiPixelCalSingleMuonTight-PromptReco-v1/ALCARECO``). Add ``root://eoscms.cern.ch///eos/cms/store/data/Run2022F/Muon/ALCARECO/SiPixelCalSingleMuonTight-PromptReco-v1/000/361/512/00000/010e377e-2f80-46b8-9397-5b5bfe7629aa.root`` to ``options.inputFiles`` in ``pxl_BPIX_FPIX_genIBC.py``.

If you check the config of the production in the Data Aggregation System (https://cmsweb.cern.ch/das) you see that the global tag used was ``124X_dataRun3_Prompt_v4``. Changed this at ``process.GlobalTag``.

You can run the code localy with cmsRun:
```
cmsRun pxl_BPIX_FPIX_genIBC.py
```
As default the number of events is restricted to 2000, if you want to run on more events you can change ``options.maxEvents``.

The code is best run with CRAB (https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrab). An example for a CRAB submit file is included (``crab_submit.py``). Change the following lines:
```
config.General.requestName
config.JobType.psetName
config.Data.inputDataset
config.Data.outputDatasetTag
config.Data.lumiMask
config.Data.outLFNDirBase
config.Site.storageSite
```

As input dataset you can use: ``/Muon/Run2022F-SiPixelCalSingleMuonTight-PromptReco-v1/ALCARECO``. ``storageSite`` needs the tier3 you have access to. As lumiMask you can use the ``Cert_Collisions2022_355100_361580_Golden.json`` which is provided.
Because running CRAB jobs is not feasible in this tutorial an example output file is provided in the ``single_run`` directory.


## Producing pixel resolution plots for single run

The fit plots don't need CMSSW. In a clean shell source LCG:
```
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh
```
Switch to the ``single_run`` directory.
The output files from the PixelTriplet code are required. An example run from Run3 is provided for the exercise (``361512.root``).
FitAndPlot.C fits the distributions with a student t-fit and produces the pixel resolution plots. There is a python wrapper available for easier use, ``runFits.py``. It takes as input the folder where the root files with the runs are stored. If you only wish to process one run also give the run number, otherwise the code is run on all runs in the input folder and stores the results in json files which can later be used to produce trend plots.
Insert the regions and fit types in the ``runFits.py`` code at L83 and L84. Because the root file is saved in the same directory as the code you can simply run:

```
python runFits.py --input . --run 361512
```
The code will plot the fits with the residual printed as label for all the layers of the barrel and all the disk of the forward tracker detector. The fits are done separately for the two different reconstruction algorithms.

## Producing trend plots
The trend plots don't need CMSSW. In a clean shell source LCG:
```
source /cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/setup.sh
```
Switch to the ``trend-plotter`` directory.
In order to produce trend plots the fit code from the previous exercise must be run on all runs from the time range which should be shown in the trend plots. An example from Run3 with the points merged with bins of 0.5 finv is provided in trend-plotter/inputs. In the same directory are also json files with runs where the HV, the calibration or the gain was changed. These changes have an impact on the pixel resolution and are therefore visualized as lines in the plots.

The trend plotter (https://gitlab.cern.ch/paconnor/trend-plotter) is used. First compile libTrend:

```
 g++ -g -Wall -O3 -std=c++1z -fPIC -DDEBUG $(root-config --cflags) -shared Trend.cc Trend.h $(root-config --libs) -o libTrend.so
```

Compile the code with:
```
make FPIX_LA
make BPIX_LA
```

Then the trend plots for FPIX or BPIX can be run with:
```
./FPIX_LA
./BPIX_LA
```


optional exercise:

For the trend plots the lumi per run is required. First one needs to get the luminosity of all 2022 runs using brilcalc. If you are not familiar with brilcalc follow the setup here: https://cms-service-lumi.web.cern.ch/cms-service-lumi/brilwsdoc.html

```
brilcalc lumi --amodetag PROTPHYS -u /fb --normtag /cvmfs/cms-bril.cern.ch/cms-lumi-pog/Normtags/normtag_BRIL.json --begin "01/01/22 00:00:00" --end "12/31/22 23:59:59" --output-style csv > stable2022.csv
```
The ``stable2022.csv`` file is also provided. In order to get a more suitable format ``skimLumiFile.py`` can be used:

``
python skimLumiFile.py --inputFiles stable2022.csv --outputFile deliveredLumiRun3.txt`--lumiType delivered
``
