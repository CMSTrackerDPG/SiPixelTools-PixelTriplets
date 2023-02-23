#!/usr/bin/env python3

import os
import sys
import csv
import json
import argparse

#####################################################################################
#Simple script to merge brilcalc CSV output and export it into more suitable format
#####################################################################################

if __name__ == '__main__':
    #Parse args
    parser = argparse.ArgumentParser()
    parser.add_argument('-i','--inputFiles', dest='inputFiles', help='Comma separated list of input CSV lumifiles.', action='store', default="" )
    parser.add_argument('-o','--outputFile', dest='outputFile', help='Skimmed output lumifile name [".csv",".txt",".json"].', default="lumifile.csv" )
    parser.add_argument('--lumiType', dest='lumiType', help='Define lumi values to export ["delivered","recorded"].', default="delivered" )
    args = parser.parse_args()
    inputFiles = args.inputFiles.split(",") 
    if not len(args.inputFiles) > 0:
        print("No input found!")
        sys.exit(0)
    if args.lumiType not in ["delivered","recorded"]:
        print("Unknown lumi type.") 
        sys.exit(0)
    if args.outputFile.split(".")[-1] not in ["csv","txt","json"]:
        print("Unknown output extension. Allowed are \".csv\",\".txt\",\".json\".")
    else:
        outFormat = args.outputFile.split(".")[-1]      

    #Read raw data
    data = []
    intLumiTotal = {'recorded' : 0.0, 'delivered' : 0.0}
    for inputFile in inputFiles:
        if not os.path.isfile(inputFile): 
            print("Input file "+inputFile+" not found!")
            sys.exit(0) 
        if not inputFile.endswith(".csv"):
            print("Incorrect input format. CSV required.")
            sys.exit(0)

        intLumiPerFile = {'recorded' : 0.0, 'delivered' : 0.0}        
        with open(inputFile, newline='') as csvfile:
            reader = csv.reader(csvfile, delimiter=',')
            idxRun = 0
            idxDelivered = 4
            idxRecorded = 5  
            run = 0
            run_delivered = 0.
            run_recorded = 0.
            for irow,row in enumerate(reader):
                print("run_delivered:",run_delivered)
                if irow == 1:
                    for ih,h in enumerate(row):
                        if "run" in h.lower(): idxRun = ih 
                        if "delivered" in h.lower(): idxDelivered = ih
                        if "recorded" in h.lower(): idxRecorded = ih 
                if "#Summary:" in row:
                    data.append({'run' : run, 'delivered' : run_delivered, 'recorded' : run_recorded})
                if len(row) == 0 or row[0].startswith("#"): continue
                if row[idxRun].split(":")[0] == run:
                    run_delivered += float(row[idxDelivered])
                    run_recorded += float(row[idxRecorded])                    
                else:
                    if run_delivered!=0.:
                        data.append({'run' : run, 'delivered' : run_delivered, 'recorded' : run_recorded})
                    run_delivered = float(row[idxDelivered])
                    run_recorded = float(row[idxRecorded])
                intLumiPerFile['delivered'] += float(row[idxDelivered])
                intLumiPerFile['recorded'] += float(row[idxRecorded])
                run = row[idxRun].split(":")[0]

        print(args.lumiType[:1].upper()+args.lumiType[1:]+" lumi for file "+inputFile+": "+str(intLumiPerFile[args.lumiType]))
        intLumiTotal[args.lumiType] += intLumiPerFile[args.lumiType]

    #Some probably unnecessary crosschecks
    runs = []
    repeatingRuns = []
    for d in data:
        if d['run'] not in runs: 
            runs.append(d['run'])
        else:
            repeatingRuns.append(d['run'])
    if len(repeatingRuns) > 0:
        print("WARNING: Some runs are repeating.")

    #Write skimmed data in required output format
    if outFormat == "csv":
        with open(args.outputFile, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile, delimiter=' ',
                                quotechar='|', quoting=csv.QUOTE_MINIMAL)
            for d in data:
                writer.writerow([d['run'],d[args.lumiType]])
    elif outFormat == "txt":
        with open(args.outputFile, 'w') as txtfile:
            for idx,d in enumerate(data):
                endl = "\n"
                if idx == len(data)-1: endl = "" 
                txtfile.write("%s %s%s" %(d['run'],d[args.lumiType],endl))
    elif outFormat == "json":
        dataJSON = []
        for d in data:
            dataJSON.append({d['run'] : d[args.lumiType]})
        with open(args.outputFile, 'w') as jsonfile:
            json.dump(dataJSON, jsonfile)
        
    print("Total "+args.lumiType[:1].upper()+args.lumiType[1:]+" luminosity: "+str(intLumiTotal[args.lumiType])) 
    print("Skimmed lumifile created: "+args.outputFile)
