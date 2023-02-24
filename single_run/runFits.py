import os, glob
import ROOT
import argparse
import json
from os.path import exists

def runFits(inputFile, fitType, region,allResults,outfile):
    inputname = inputFile.split("/")[0]
    run = inputFile.split("/")[1].split(".")[0]
    outpath = "plots/"+inputname+"/"+run
    if not exists(outpath):
        os.makedirs(outpath)
    else:
        if region=="BPix" and fitType=="Generic": #only remove files at start of loop
            files = glob.glob(outpath+"/*")
            for f in files:
                os.remove(f)
    dirName = region+"Resolution_"+fitType+"/Run"
    ROOT.FitAndPlot(inputFile, dirName, run, outpath, fitType,region,inputname)
    if outfile=="": return
    with open(outpath+"/Param_"+fitType+"_"+run+".txt", "r") as f:
            results = f.read().split("\n")
   
    # Parse the text file the script saves anyways
    allResults[run] = {}
    for result in results:
        if result == "":
            continue
        legend, numbers = result.split(" : ")
        legend = legend.split(",")
        legend = [x for x in legend if x != ""]
        disk, direction, legend = legend[0], legend[1], legend[2::]
        legend = [x.replace(" ","") for x in legend]
        numbers = numbers.split(",")
        numbers = [x for x in numbers if x != ""]
        numbers = [x.replace(" ","") for x in numbers]
        print(disk, direction, legend, "==============",numbers)
        disk = disk.strip() #var have whitespace which needs to be removed
        direction = direction.strip()
        filename_mu = outfile+"/mu_%s_%s_%s.txt"%(direction,disk,fitType)
        filename_sigma = outfile+"/sigma_%s_%s_%s.txt"%(direction,disk,fitType)
        file_mu = open(filename_mu,"a")
        file_sigma = open(filename_sigma,"a")
        file_mu.write(run+" "+numbers[2]+" "+numbers[3]+"\n")
        file_sigma.write(run+" "+numbers[0]+" "+numbers[1]+"\n")
        file_mu.close()
        file_sigma.close()
        assert len(legend) == len(numbers)
        if disk not in allResults[run].keys():
            allResults[run][disk] = {}
        if direction not in allResults[run][disk].keys():
            allResults[run][disk][direction] = {}
        for il, l in enumerate(legend):
            allResults[run][disk][direction][l] = numbers[il]
    
   

        

if __name__ == "__main__":
    argumentparser = argparse.ArgumentParser(description='Python wrapper for FitAndPlot.C',formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    argumentparser.add_argument('-i', '--input', action='store', type=str, help="path to input file", default ="", required=False)
    argumentparser.add_argument('-r', '--run', action='store', type=str, default="")
    
    ROOT.gROOT.SetBatch(ROOT.kTRUE)

    args = argumentparser.parse_args()
    input = args.input
    run = args.run
    outfile = ""
    allResults = {}

    if run!="":
        files = [input+"/"+run+".root"]
    else:
        listfiles = os.listdir(input)
        listfiles.sort()
        files = []
        for rootfile in listfiles:
            files.append(input+"/"+rootfile)
  
    ROOT.gROOT.LoadMacro("FitAndPlot.C")    
    for region in []: #fill in regions
        for fitType in []:# fill in the two reconstruction types
            if len(files)==1:
                runFits(files[0],fitType,region,allResults,outfile)
            else:
                outfile = "/work/pbaertsc/pixel/trend-plotter/inputs/%s"%input.split("/")[-1]
                if region=="FPix":
                    for disk_loop in ["D1","D2","D3"]:
                        for direction_loop in ["x","z"]:
                            file_mu = open(outfile+"/mu_%s_%s_%s.txt"%(direction_loop,disk_loop,fitType),"w")
                            file_mu.close()
                            file_sigma = open(outfile+"/sigma_%s_%s_%s.txt"%(direction_loop,disk_loop,fitType),"w")
                            file_sigma.close()
                elif region=="BPix":
                    for layer_loop in ["L1","L2","L3","L4"]:
                        for direction_loop in ["x","z"]:
                            file_mu = open(outfile+"/mu_%s_%s_%s.txt"%(direction_loop,layer_loop,fitType),"w")
                            file_mu.close()
                            file_sigma = open(outfile+"/sigma_%s_%s_%s.txt"%(direction_loop,layer_loop,fitType),"w")
                            file_sigma.close()
                for file in files:
                    runFits(file, fitType, region,allResults,outfile)
                if exists(outfile+"/fit_results_"+region+"_"+fitType+".json"):
                    os.remove(outfile+"/fit_results_"+region+"_"+fitType+".json")
                with open(outfile+"/fit_results_"+region+"_"+fitType+".json","w") as fj:
                    json.dump(allResults, fj, sort_keys=True, indent=4, separators=(',', ': '))
