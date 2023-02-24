ROOTINC    = $(shell root-config --incdir)
ROOTCFLAGS = $(shell root-config --cflags)
ROOTLIBS   = $(shell root-config --libs)

BOOSTINC = -I/usr/include/boost
BOOSTLIBS = -L/cvmfs/sft.cern.ch/lcg/views/LCG_101/x86_64-centos7-gcc10-opt/lib/ -lboost_filesystem
#-lboost_program_options

CFLAGS=-g -Wall -O3 -std=c++1z -fPIC -DDEBUG

all: libTrend.so Overlap Zmm Barycentre DMRs APUs VertexSplit PrimaryVertex BPIX_LA FPIX_LA

lib%.so: %.h %.cc 
	g++ ${CFLAGS} $(ROOTCFLAGS) -shared $^ $(ROOTLIBS) -o $@

%: %.cc
	g++ $(CFLAGS) $(ROOTCFLAGS) $^ $(ROOTLIBS) $(BOOSTLIBS) -o $@ -L. -Wl,-rpath,. -lTrend -lrt

clean: 
	@rm -f *.pdf Overlap Zmm Barycentre DMRs APUs VertexSplit PrimaryVertex *.so ?PIX_LA

.PHONY: all clean
