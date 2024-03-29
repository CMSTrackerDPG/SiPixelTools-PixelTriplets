// -*- C++ -*-dz%
//
// Package:  Pixel
// Class:    Pixel
//
// my/Pixel/src/Pixel.cc
//
// Pixel (and strip) triplet residuals
//
// Author: Valere Lambert, UZH 2015
//
// Based off of original Triplet Author:  Daniel Pitzl, DESY
//         Created:  Sat Feb 12 12:12:42 CET 2011
// 
//
// system include files:
#include <memory>
#include <iostream>
#include <iomanip>
#include <cmath>
// ROOT:
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TFile.h"
#include "TTree.h"

// CMS and user include files:
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include <FWCore/Framework/interface/EventSetup.h>
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include <DataFormats/BeamSpot/interface/BeamSpot.h>

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETFwd.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include <DataFormats/TrackReco/interface/HitPattern.h>

#include <MagneticField/Engine/interface/MagneticField.h>
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonTopologies/interface/Topology.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"

#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"

#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "RecoTracker/TransientTrackingRecHit/interface/TkTransientTrackingRecHitBuilder.h"


#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajectoryBuilder.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryFitter.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
#include <TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h>
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "DataFormats/TrackerRecHit2D/interface/TkCloner.h"

#include "TrackingTools/DetLayers/interface/DetLayer.h"
//#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "CLHEP/Random/RandFlat.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelLorentzAngle.h"
#include "CondFormats/DataRecord/interface/SiPixelLorentzAngleRcd.h"

// Helix script from Morris Schwartz
#include "SimpleHelix.h"

// Flag for new tracking rechis, has to be ON for pre7 and later   
#define NEW_TRACKINGRECHITS  // For V71X_pre7 and later 

float BPIX_ptcut = 12.;
float FPIX_ptcut = 4.;

struct Histos{
  TH1D *hclusprob_fpix;

  TProfile *h420f2_123_eta, *h421f2_123_eta, *h420f3_234_eta, *h421f3_234_eta, *h420f4_234_eta, *h421f4_234_eta;

  TProfile *h043, *h044;
  TH1D     *h030, *h031, *h032, *h035, *h036, *h036_1, *h037, *h038, *h037_1, *h039, *h040, *h041, *h042;

  TH1D *h420f1_123, *h421f1_123, *h420f2_123, *h421f2_123, *h420f3_123, *h421f3_123;
  TH1D *h420f2_123_r2, *h421f2_123_r2, *h420f2_123_r1, *h421f2_123_r1;
  TH1D *h077f1_123, *h078f1_123, *h079f1_123, *h069f1_123;
  TH1D *h077f2_123, *h078f2_123, *h079f2_123, *h069f2_123;
  TH1D *h077f3_123, *h078f3_123, *h079f3_123, *h069f3_123;

  TH1D *h420b1_123, *h421b1_123, *h420b2_123, *h421b2_123, *h420b3_123, *h421b3_123;
  TH1D *h077b1_123, *h078b1_123, *h079b1_123, *h069b1_123;
  TH1D *h077b2_123, *h078b2_123, *h079b2_123, *h069b2_123;
  TH1D *h077b3_123, *h078b3_123, *h079b3_123, *h069b3_123;

  TH1D *h420f1_124, *h421f1_124, *h420f2_124, *h421f2_124, *h420f4_124, *h421f4_124;
  TH1D *h077f1_124, *h078f1_124, *h079f1_124, *h069f1_124;
  TH1D *h077f2_124, *h078f2_124, *h079f2_124, *h069f2_124;
  TH1D *h077f4_124, *h078f4_124, *h079f4_124, *h069f4_124;

  TH1D *h420b1_124, *h421b1_124, *h420b2_124, *h421b2_124, *h420b4_124, *h421b4_124;
  TH1D *h077b1_124, *h078b1_124, *h079b1_124, *h069b1_124;
  TH1D *h077b2_124, *h078b2_124, *h079b2_124, *h069b2_124;
  TH1D *h077b4_124, *h078b4_124, *h079b4_124, *h069b4_124;

  TH1D *h420f1_134, *h421f1_134, *h420f3_134, *h421f3_134, *h420f4_134, *h421f4_134;
  TH1D *h077f1_134, *h078f1_134, *h079f1_134, *h069f1_134;
  TH1D *h077f3_134, *h078f3_134, *h079f3_134, *h069f3_134;
  TH1D *h077f4_134, *h078f4_134, *h079f4_134, *h069f4_134;

  TH1D *h420b1_134, *h421b1_134, *h420b3_134, *h421b3_134, *h420b4_134, *h421b4_134;
  TH1D *h077b1_134, *h078b1_134, *h079b1_134, *h069b1_134;
  TH1D *h077b3_134, *h078b3_134, *h079b3_134, *h069b3_134;
  TH1D *h077b4_134, *h078b4_134, *h079b4_134, *h069b4_134;

  TH1D *h420f2_234, *h421f2_234, *h420f3_234, *h421f3_234, *h420f4_234, *h421f4_234;
  TH1D *h421f3_234_r1, *h420f3_234_r1, *h421f4_234_r1, *h420f4_234_r1;
  TH1D *h421f3_234_r2, *h420f3_234_r2, *h421f4_234_r2, *h420f4_234_r2;
  TH1D *h077f2_234, *h078f2_234, *h079f2_234, *h069f2_234;
  TH1D *h077f3_234, *h078f3_234, *h079f3_234, *h069f3_234;
  TH1D *h077f4_234, *h078f4_234, *h079f4_234, *h069f4_234;

  TH1D *h420b2_234, *h421b2_234, *h420b3_234, *h421b3_234, *h420b4_234, *h421b4_234;
  TH1D *h077b2_234, *h078b2_234, *h079b2_234, *h069b2_234;
  TH1D *h077b3_234, *h078b3_234, *h079b3_234, *h069b3_234;
  TH1D *h077b4_234, *h078b4_234, *h079b4_234, *h069b4_234;


  void InitBPix_phase1(TFileDirectory* fs);
  void InitFPix_phase1(TFileDirectory* fs);
};

class Pixel_FPix_phase1 : public edm::one::EDAnalyzer<edm::one::SharedResources, edm::one::WatchRuns>, public Histos {
public:
  explicit Pixel_FPix_phase1(const edm::ParameterSet&);
  ~Pixel_FPix_phase1();

private:
  virtual void beginJob() override;
  virtual void beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endRun(const edm::Run& iRun, const edm::EventSetup& iSetup) override {}
  virtual void endJob() override;
  void getResiduals(const edm::Event&, const edm::EventSetup&, std::string detTag);
  std::vector<double> getIntersection(std::vector<double> p1, std::vector<double> p2, double rho, const GeomDet *detHit, std::vector<double> intersection);

  edm::InputTag _triggerSrc;
  std::string _ttrhBuilder;
  std::string _track_collection;
  HLTConfigProvider HLTConfig;
  bool doBPix;
  bool doFPix;
  bool Template;
  bool Generic;

  float dx_res_1 = -999;
  float dz_res_1 = -999;
  float dx_res_2 = -999;
  float dz_res_2 = -999;
  float dx_res_3 = -999;
  float dz_res_3 = -999;

  float pt_res;
  float pt_res_refit;
  int cluster_size_res_init;
  int cluster_size_res;
  int hits_track;
  int hits_barrel;
  int hits_endcap;
  int ls_with_measure;
  int ds_with_measure;
  int runNumber_res;
  int lumiBlock_res;

  bool isTriplet;
  TTree * tree;
  std::vector<float> dx_resolution_study_1;
  std::vector<float> dz_resolution_study_1;
  std::vector<float> dx_resolution_study_2;
  std::vector<float> dz_resolution_study_2;
  std::vector<float> dx_resolution_study_3;
  std::vector<float> dz_resolution_study_3;
  std::vector<float> pt_resolution_study;
  std::vector<float> pt_resolution_study_refit;
  int number_of_tracks;
  std::vector<float> pt_all_tracks;
  std::vector<float> pt_4_tracks;
  std::vector<float> pt_12_tracks;
  std::vector<float> pt_highlayerswithmeasurements;
  std::vector<float> pt_lowlayerswithmeasurements;
  std::vector<float> TrackerLayersWithMeasurements;
  std::vector<int> cluster_size_all_tracks;
  std::vector<int> hits_on_track_barrel;
  std::vector<int> hits_on_track_endcap;
  std::vector<int> hits_on_track_tracker;
  std::vector<int> cluster_size_resolution_study;
  std::vector<float> layers_with_measurement;
  std::vector<float> disks_with_measurement;
  int numberOfTracksCount = 0;
  int numberOfTracksCount123 = 0;
  int numberOfTracksCount124 = 0;
  int numberOfTracksCount234 = 0;
  int numberOfTracksCount134 = 0;
  
  edm::EDGetTokenT<reco::BeamSpot>  t_offlineBeamSpot_;
  edm::EDGetTokenT<reco::VertexCollection> t_offlinePrimaryVertices_ ;
  edm::EDGetTokenT <reco::TrackCollection>  t_generalTracks_;
  edm::EDGetTokenT< edm::View<reco::PFMET>> t_pfMet_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> esTokenTTopo_;
  edm::ESGetToken<TrajectoryFitter, TrajectoryFitter::Record> fitterToken_;
  edm::ESGetToken<TransientTrackBuilder,TransientTrackRecord> theBToken_;
  edm::ESGetToken<TransientTrackingRecHitBuilder,TransientRecHitRecord> hitBuilderToken_;
  edm::ESGetToken<Propagator,TrackingComponentsRecord> propToken_;
  // ----------member data ---------------------------
  std::map<int, Histos> runmap;

  std::string processName_;
  int run_num = -999;
  int lumi_block = -999;

  //Cluster Probability
  int clusSize_X = -999;
  int clusSize_Y = -999;
  float clusProb_FPix_phase1 = -999;
  
  // Rechit coordinates in local - 3 hits (xpx1_l, xpy1_l), (xpx2_l, xpy2_l), and (xpx3_l, xpy3_l)
  double xpx1_l = -999;
  double xpy1_l = -999;

  double xpx2_l = -999;
  double xpy2_l = -999;
  
  double xpx3_l = -999;
  double xpy3_l = -999;

  double xpx4_l = -999;
  double xpy4_l = -999;

  //**

  double xpx1_l_r1 = -999;
  double xpy1_l_r1 = -999;

  double xpx2_l_r1 = -999;
  double xpy2_l_r1 = -999;
  
  double xpx3_l_r1 = -999;
  double xpy3_l_r1 = -999;

  double xpx4_l_r1 = -999;
  double xpy4_l_r1 = -999;

  //**

  double xpx1_l_r2 = -999;
  double xpy1_l_r2 = -999;

  double xpx2_l_r2 = -999;
  double xpy2_l_r2 = -999;
  
  double xpx3_l_r2 = -999;
  double xpy3_l_r2 = -999;

  double xpx4_l_r2 = -999;
  double xpy4_l_r2 = -999;

  // Estimated coordinates in local 
  double xl_ideal_1 = -999;
  double yl_ideal_1 = -999;

  double xl_ideal_2 = -999;
  double yl_ideal_2 = -999;

  double xl_ideal_3 = -999;
  double yl_ideal_3 = -999;

  double xl_ideal_4 = -999;
  double yl_ideal_4 = -999;

  double xl_ideal_2_r1 = -999;
  double yl_ideal_2_r1 = -999;

  double xl_ideal_3_r1 = -999;
  double yl_ideal_3_r1 = -999;

  double xl_ideal_4_r1 = -999;
  double yl_ideal_4_r1 = -999;

  double xl_ideal_2_r2 = -999;
  double yl_ideal_2_r2 = -999;

  double xl_ideal_3_r2 = -999;
  double yl_ideal_3_r2 = -999;

  double xl_ideal_4_r2 = -999;
  double yl_ideal_4_r2 = -999;

  // Residuals
  double residual_x_1 = -999;
  double residual_x_2 = -999;
  double residual_x_3 = -999;
  double residual_x_4 = -999;

  double residual_x_2_r1 = -999;
  double residual_x_3_r1 = -999;
  double residual_x_4_r1 = -999;
  double residual_x_2_r2 = -999;
  double residual_x_3_r2 = -999;
  double residual_x_4_r2 = -999;

  double residual_y_1 = -999;
  double residual_y_2 = -999;
  double residual_y_3 = -999;
  double residual_y_4 = -999;

  double residual_y_2_r1 = -999;
  double residual_y_3_r1 = -999;
  double residual_y_4_r1 = -999;
  double residual_y_2_r2 = -999;
  double residual_y_3_r2 = -999;
  double residual_y_4_r2 = -999;

  // Errors on Rechit coordinates in local
  double x_local_error_1 = -999;
  double y_local_error_1 = -999;

  double x_local_error_2 = -999;
  double y_local_error_2 = -999;

  double x_local_error_3 = -999;
  double y_local_error_3 = -999;

  double x_local_error_4 = -999;
  double y_local_error_4 = -999;

  // Helix parameters
  int Pass = -99;
  int nloops = -99;
  int insideD = -99;
  double radius = -99;
  double xcenter = -99;
  double ycenter = -99;
  double dzdphi = -99;
  double z0 = -99;
  double rho = -99;

};

class Counter_Template{
   public:
      static int neve;
      static unsigned int prevrun;
};

class Counter_Generic{
   public:
      static int neve;
      static unsigned int prevrun;
};
int Counter_Template::neve = 0;
unsigned int Counter_Template::prevrun = 0;
int Counter_Generic::neve = 0;
unsigned int Counter_Generic::prevrun = 0;


Pixel_FPix_phase1::Pixel_FPix_phase1(const edm::ParameterSet& iConfig)
{
  usesResource("TFileService");
  std::cout << "PxlFPix constructed\n";
  _triggerSrc = iConfig.getParameter<edm::InputTag>("triggerSource");
  _ttrhBuilder = iConfig.getParameter<std::string>("ttrhBuilder");
  doBPix=iConfig.getParameter<bool>("doBPix");
  doFPix=iConfig.getParameter<bool>("doFPix");
  Template=iConfig.getParameter<bool>("Template");
  Generic=iConfig.getParameter<bool>("Generic");

  esTokenTTopo_ = esConsumes(),
  fitterToken_ = esConsumes(edm::ESInputTag("","KFFittingSmootherWithOutliersRejectionAndRK")),
  theBToken_ = esConsumes(edm::ESInputTag("","TransientTrackBuilder")),
  hitBuilderToken_ = esConsumes(edm::ESInputTag("",_ttrhBuilder)),  
  propToken_ = esConsumes(edm::ESInputTag("","PropagatorWithMaterial")),
  //std::cout<<_triggerSrc<<" "<<_triggerSrc.label()<<" "<<_triggerSrc.process()<<" "
  //	   <<_triggerSrc.instance()<<" "<<std::endl;

  t_offlineBeamSpot_ =    consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));
  t_offlinePrimaryVertices_ =   consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices"));
  _track_collection = iConfig.getParameter<std::string>("track_collection");
  t_generalTracks_= consumes<reco::TrackCollection> (edm::InputTag(_track_collection));
  
  t_pfMet_= consumes< edm::View<reco::PFMET>>(edm::InputTag("pfMet"));
  edm::Service<TFileService> fsT;
  tree = fsT->make<TTree>("tree", "tree");
  tree->Branch("number_of_tracks", &number_of_tracks);
  tree->Branch("pt_all_tracks", &pt_all_tracks);
  tree->Branch("cluster_size_all_tracks", &cluster_size_all_tracks);
  tree->Branch("cluster_size_resolution_study", &cluster_size_resolution_study);
  tree->Branch("pt_resolution_study_refit", &pt_resolution_study_refit);
  tree->Branch("dx_resolution_study_1", &dx_resolution_study_1);
  tree->Branch("dz_resolution_study_1", &dz_resolution_study_1);
  tree->Branch("dx_resolution_study_2", &dx_resolution_study_2);
  tree->Branch("dz_resolution_study_2", &dz_resolution_study_2);
  tree->Branch("dx_resolution_study_3", &dx_resolution_study_3);
  tree->Branch("dz_resolution_study_3", &dz_resolution_study_3);
  tree->Branch("pt_resolution_study", &pt_resolution_study);
  tree->Branch("hits_on_track_barrel", &hits_on_track_barrel);
  tree->Branch("hits_on_track_endcap", &hits_on_track_endcap);
  tree->Branch("hits_on_track_tracker", &hits_on_track_tracker);
  tree->Branch("runNumber_res", &runNumber_res);
  tree->Branch("lumiBlock_res", &lumiBlock_res);
  tree->Branch("pt_4_tracks", &pt_4_tracks);
  tree->Branch("pt_12_tracks", &pt_12_tracks);
  tree->Branch("pt_highlayerswithmeasurements", &pt_highlayerswithmeasurements);
  tree->Branch("pt_lowlayerswithmeasurements", &pt_lowlayerswithmeasurements);
  tree->Branch("TrackerLayersWithMeasurements", &TrackerLayersWithMeasurements);
}
Pixel_FPix_phase1::~Pixel_FPix_phase1()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}


void Histos::InitFPix_phase1(TFileDirectory* fs)
{
  /*         Initialize histograms for FPIX residuals         */
  /*h040 = fs->make<TH1D>( "h040", "b_{xy}, p_{t} > 8, FXD2;b_{xy} [#mum];tracks", 100, -250, 250 );
  h041 = fs->make<TH1D>( "h041", "#sigma(d_{xy}), p_{t} > 8, FXD2;#sigma(d_{xy}) [#mum];tracks", 100, 0, 100 );
  h042 = fs->make<TH1D>( "h042", "b_{xy}/#sigma, p_{t} > 8, FXD2;b_{xy}/#sigma;tracks", 100, -10, 10 );
  h043 = fs->make<TProfile>( "h043", "#sigma(b_{xy}) vs logp_{t} with FXD2;log(p_{t} [GeV]);<#sigma(b_{xy})> [#mum]", 40, 0, 2, 0, 5000 );
  h044 = fs->make<TProfile>( "h044", "rms(b_{xy}) vs logp_{t} with FXD2;log(p_{t} [GeV]);rms(b_{xy}) [#mum]", 40, 0, 2, 0, 5000 );
  */

  h030 = fs->make<TH1D>( "h030", "number of tracks;tracks", 200, 0, 2000 );
  h031 = fs->make<TH1D>( "h031", "track charge;charge", 11, -5.5, 5.5 );
  h032 = fs->make<TH1D>( "h032", "p_{t};p_{t} [GeV]", 1000, 0, 100 );

  h035 = fs->make<TH1D>( "h035", "valid tracker hits", 41, -0.5, 40.5 );
  h036 = fs->make<TH1D>( "h036", "valid pixel barrel hits", 7, -0.5, 6.5 );
  h036_1 = fs->make<TH1D>( "h036_1", "valid pixel endcap hits", 7, -0.5, 6.5 );
  h037 = fs->make<TH1D>( "h037", "tracker layers", 31, -0.5, 30.5 );
  h037_1 = fs->make<TH1D>( "h037_1", "tracker layers; tracks pt>4", 31, -0.5, 30.5 );
  h038 = fs->make<TH1D>( "h038", "pixel barrel layers", 6, -0.5, 5.5 );
  h039 = fs->make<TH1D>( "h039", "pixel endcap layers", 6, -0.5, 5.5 );
  h040 = fs->make<TH1D>( "h040", "normalised chis2", 100, 0., 10. );
  h041 = fs->make<TH1D>( "h041", "pt error", 100, 0, 10);
  h042 = fs->make<TH1D>( "h042", "track quality", 20, 0, 20 );

  h043 = fs->make<TProfile>( "h043", "chis vs layers", 20, 0, 20, 0, 100 );
  h044 = fs->make<TProfile>( "h044", "ptErrpr quality vs layers", 20, 0, 20, 0, 100 );

  hclusprob_fpix = fs->make<TH1D>( "hclusprob_fpix", "FPix Cluster Probability;", 120, 0, 1.2 );

  h420f2_123_eta = fs->make<TProfile>( "h420f2_123_eta", "PXF1 residuals #Deltax, p_{t} > 4;PXF1 #Deltax [#mum]; Track eta;PXF1 rms(#Deltax) [#mum]", 100, -4., 4., 0, 199 );
  h420f3_234_eta = fs->make<TProfile>( "h420f3_234_eta", "PXF2 residuals #Deltax, p_{t} > 4;PXF2 #Deltax [#mum]; Track eta;PXF2 rms(#Deltax) [#mum]", 100, -4., 4., 0, 199 );
  h420f4_234_eta = fs->make<TProfile>( "h420f4_234_eta", "PXF3 residuals #Deltax, p_{t} > 4;PXF3 #Deltax [#mum]; Track eta;PXF3 rms(#Deltax) [#mum]", 100, -4., 4., 0, 199 );
  h421f2_123_eta = fs->make<TProfile>( "h421f2_123_eta", "PXF1 residuals #Deltay, p_{t} > 4;PXF1 #Deltay [#mum]; Track eta;PXF1 rms(#Deltay) [#mum]", 100, -4., 4., 0, 199 );
  h421f3_234_eta = fs->make<TProfile>( "h421f3_234_eta", "PXF2 residuals #Deltay, p_{t} > 4;PXF2 #Deltay [#mum]; Track eta;PXF2 rms(#Deltay) [#mum]", 100, -4., 4., 0, 199 );
  h421f4_234_eta = fs->make<TProfile>( "h421f4_234_eta", "PXF3 residuals #Deltay, p_{t} > 4;PXF3 #Deltay [#mum]; Track eta;PXF3 rms(#Deltay) [#mum]", 100, -4., 4., 0, 199 );

  h420f1_123 = fs->make<TH1D>( "h420f1_123", "PXB1 residuals #Deltax, p_{t} > 4;PXB1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f2_123 = fs->make<TH1D>( "h420f2_123", "PXF1 residuals #Deltax, p_{t} > 4;PXF1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f2_123_r1 = fs->make<TH1D>( "h420f2_123_r1", "PXF1 residuals #Deltax on inner ring, p_{t} > 4;PXF1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f2_123_r2 = fs->make<TH1D>( "h420f2_123_r2", "PXF1 residuals #Deltax on outer ring, p_{t} > 4;PXF1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f3_123 = fs->make<TH1D>( "h420f3_123", "PXF2 residuals #Deltax, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );

  h421f1_123 = fs->make<TH1D>( "h421f1_123", "PXB1 residuals #Deltay, p_{t} > 4;PXB1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f2_123_r2 = fs->make<TH1D>( "h421f2_123_r2", "PXF1 residuals #Deltay on outer ring, p_{t} > 4;PXF1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f2_123_r1 = fs->make<TH1D>( "h421f2_123_r1", "PXF1 residuals #Deltay on inner ring, p_{t} > 4;PXF1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f2_123 = fs->make<TH1D>( "h421f2_123", "PXF1 residuals #Deltay, p_{t} > 4;PXF1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f3_123 = fs->make<TH1D>( "h421f3_123", "PXF2 residuals #Deltay, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );

  h077f1_123 = fs->make<TH1D>( "h077f1_123", "PXB1 x error ", 100, 0., 100. );
  h077f2_123 = fs->make<TH1D>( "h077f2_123", "PXF1 x error ", 100, 0., 100. );
  h077f3_123 = fs->make<TH1D>( "h077f3_123", "PXF2 x error ", 100, 0., 100. );

  h078f1_123 = fs->make<TH1D>( "h078f1_123", "PXB1 y error ", 100, 0., 100. );
  h078f2_123 = fs->make<TH1D>( "h078f2_123", "PXF1 y error ", 100, 0., 100. );
  h078f3_123 = fs->make<TH1D>( "h078f3_123", "PXF2 y error ", 100, 0., 100. );

  h079f1_123 = fs->make<TH1D>( "h079f1_123", "PXB1 x pull ", 100, -10., 10. );
  h079f2_123 = fs->make<TH1D>( "h079f2_123", "PXF1 x pull ", 100, -10., 10. );
  h079f3_123 = fs->make<TH1D>( "h079f3_123", "PXF2 x pull ", 100, -10., 10. );

  h069f1_123 = fs->make<TH1D>( "h069f1_123", "PXB1 y pull ", 100, -10., 10. );
  h069f2_123 = fs->make<TH1D>( "h069f2_123", "PXF1 y pull ", 100, -10., 10. );
  h069f3_123 = fs->make<TH1D>( "h069f3_123", "PXF2 y pull ", 100, -10., 10. );

  h420f1_124 = fs->make<TH1D>( "h420f1_124", "PXB1 residuals #Deltax, p_{t} > 4;PXB1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f2_124 = fs->make<TH1D>( "h420f2_124", "PXF1 residuals #Deltax, p_{t} > 4;PXF1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f4_124 = fs->make<TH1D>( "h420f4_124", "PXF2 residuals #Deltax, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );

  h421f1_124 = fs->make<TH1D>( "h421f1_124", "PXB1 residuals #Deltay, p_{t} > 4;PXB1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f2_124 = fs->make<TH1D>( "h421f2_124", "PXF1 residuals #Deltay, p_{t} > 4;PXF1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f4_124 = fs->make<TH1D>( "h421f4_124", "PXF2 residuals #Deltay, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );

  h077f1_124 = fs->make<TH1D>( "h077f1_124", "PXB1 x error ", 100, 0., 100. );
  h077f2_124 = fs->make<TH1D>( "h077f2_124", "PXF1 x error ", 100, 0., 100. );
  h077f4_124 = fs->make<TH1D>( "h077f4_124", "PXF2 x error ", 100, 0., 100. );

  h078f1_124 = fs->make<TH1D>( "h078f1_124", "PXB1 y error ", 100, 0., 100. );
  h078f2_124 = fs->make<TH1D>( "h078f2_124", "PXF1 y error ", 100, 0., 100. );
  h078f4_124 = fs->make<TH1D>( "h078f4_124", "PXF2 y error ", 100, 0., 100. );

  h079f1_124 = fs->make<TH1D>( "h079f1_124", "PXB1 x pull ", 100, -10., 10. );
  h079f2_124 = fs->make<TH1D>( "h079f2_124", "PXF1 x pull ", 100, -10., 10. );
  h079f4_124 = fs->make<TH1D>( "h079f4_124", "PXF2 x pull ", 100, -10., 10. );

  h069f1_124 = fs->make<TH1D>( "h069f1_124", "PXB1 y pull ", 100, -10., 10. );
  h069f2_124 = fs->make<TH1D>( "h069f2_124", "PXF1 y pull ", 100, -10., 10. );
  h069f4_124 = fs->make<TH1D>( "h069f4_124", "PXF2 y pull ", 100, -10., 10. );

  h420f1_134 = fs->make<TH1D>( "h420f1_134", "PXB1 residuals #Deltax, p_{t} > 4;PXB1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f3_134 = fs->make<TH1D>( "h420f3_134", "PXF1 residuals #Deltax, p_{t} > 4;PXF1 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f4_134 = fs->make<TH1D>( "h420f4_134", "PXF2 residuals #Deltax, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );

  h421f1_134 = fs->make<TH1D>( "h421f1_134", "PXB1 residuals #Deltay, p_{t} > 4;PXB1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f3_134 = fs->make<TH1D>( "h421f3_134", "PXF1 residuals #Deltay, p_{t} > 4;PXF1 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f4_134 = fs->make<TH1D>( "h421f4_134", "PXF2 residuals #Deltay, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );

  h077f1_134 = fs->make<TH1D>( "h077f1_134", "PXB1 x error ", 100, 0., 100. );
  h077f3_134 = fs->make<TH1D>( "h077f3_134", "PXF1 x error ", 100, 0., 100. );
  h077f4_134 = fs->make<TH1D>( "h077f4_134", "PXF2 x error ", 100, 0., 100. );

  h078f1_134 = fs->make<TH1D>( "h078f1_134", "PXB1 y error ", 100, 0., 100. );
  h078f3_134 = fs->make<TH1D>( "h078f3_134", "PXF1 y error ", 100, 0., 100. );
  h078f4_134 = fs->make<TH1D>( "h078f4_134", "PXF2 y error ", 100, 0., 100. );

  h079f1_134 = fs->make<TH1D>( "h079f1_134", "PXB1 x pull ", 100, -10., 10. );
  h079f3_134 = fs->make<TH1D>( "h079f3_134", "PXF1 x pull ", 100, -10., 10. );
  h079f4_134 = fs->make<TH1D>( "h079f4_134", "PXF2 x pull ", 100, -10., 10. );

  h069f1_134 = fs->make<TH1D>( "h069f1_134", "PXB1 y pull ", 100, -10., 10. );
  h069f3_134 = fs->make<TH1D>( "h069f3_134", "PXF1 y pull ", 100, -10., 10. );
  h069f4_134 = fs->make<TH1D>( "h069f4_134", "PXF2 y pull ", 100, -10., 10. );

  h420f2_234 = fs->make<TH1D>( "h420f2_234", "PXB2 residuals #Deltax, p_{t} > 4;PXB2 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f3_234 = fs->make<TH1D>( "h420f3_234", "PXF2 residuals #Deltax, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f4_234 = fs->make<TH1D>( "h420f4_234", "PXF3 residuals #Deltax, p_{t} > 4;PXF3 #Deltax [#mum];hits", 3000, -150, 150 );

  h420f3_234_r1 = fs->make<TH1D>( "h420f3_234_r1", "PXF2 residuals #Deltax on inner ring, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f4_234_r1 = fs->make<TH1D>( "h420f4_234_r1", "PXF3 residuals #Deltax on inner ring, p_{t} > 4;PXF3 #Deltax [#mum];hits", 3000, -150, 150 );

  h420f3_234_r2 = fs->make<TH1D>( "h420f3_234_r2", "PXF2 residuals #Deltax on outer ring, p_{t} > 4;PXF2 #Deltax [#mum];hits", 3000, -150, 150 );
  h420f4_234_r2 = fs->make<TH1D>( "h420f4_234_r2", "PXF3 residuals #Deltax on outer ring, p_{t} > 4;PXF3 #Deltax [#mum];hits", 3000, -150, 150 );

  h421f2_234 = fs->make<TH1D>( "h421f2_234", "PXB2 residuals #Deltay, p_{t} > 4;PXB2 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f3_234 = fs->make<TH1D>( "h421f3_234", "PXF2 residuals #Deltay, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f4_234 = fs->make<TH1D>( "h421f4_234", "PXF3 residuals #Deltay, p_{t} > 4;PXF3 #Deltay [#mum];hits", 3000, -300, 300 );

  h421f3_234_r1 = fs->make<TH1D>( "h421f3_234_r1", "PXF2 residuals #Deltay on inner ring, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f4_234_r1 = fs->make<TH1D>( "h421f4_234_r1", "PXF3 residuals #Deltay on inner ring, p_{t} > 4;PXF3 #Deltay [#mum];hits", 3000, -300, 300 );

  h421f3_234_r2 = fs->make<TH1D>( "h421f3_234_r2", "PXF2 residuals #Deltay on outer ring, p_{t} > 4;PXF2 #Deltay [#mum];hits", 3000, -300, 300 );
  h421f4_234_r2 = fs->make<TH1D>( "h421f4_234_r2", "PXF3 residuals #Deltay on outer ring, p_{t} > 4;PXF3 #Deltay [#mum];hits", 3000, -300, 300 );

  h077f2_234 = fs->make<TH1D>( "h077f2_234", "PXB2 x error ", 100, 0., 100. );
  h077f3_234 = fs->make<TH1D>( "h077f3_234", "PXF2 x error ", 100, 0., 100. );
  h077f4_234 = fs->make<TH1D>( "h077f4_234", "PXF3 x error ", 100, 0., 100. );

  h078f2_234 = fs->make<TH1D>( "h078f2_234", "PXB2 y error ", 100, 0., 100. );
  h078f3_234 = fs->make<TH1D>( "h078f3_234", "PXF2 y error ", 100, 0., 100. );
  h078f4_234 = fs->make<TH1D>( "h078f4_234", "PXF3 y error ", 100, 0., 100. );

  h079f2_234 = fs->make<TH1D>( "h079f2_234", "PXB2 x pull ", 100, -10., 10. );
  h079f3_234 = fs->make<TH1D>( "h079f3_234", "PXF2 x pull ", 100, -10., 10. );
  h079f4_234 = fs->make<TH1D>( "h079f4_234", "PXF3 x pull ", 100, -10., 10. );

  h069f2_234 = fs->make<TH1D>( "h069f2_234", "PXB2 y pull ", 100, -10., 10. );
  h069f3_234 = fs->make<TH1D>( "h069f3_234", "PXF2 y pull ", 100, -10., 10. );
  h069f4_234 = fs->make<TH1D>( "h069f4_234", "PXF3 y pull ", 100, -10., 10. );

}

void Histos::InitBPix_phase1(TFileDirectory* fs)
{
  /*         Initialize histograms for BPIX residuals         */
  
  h420b1_123 = fs->make<TH1D>( "h420b1_123", "PXB1 residuals #Deltax, p_{t} > 12;PXB1 #Deltax [#mum];hits", 100, -150, 150 );
  h420b2_123 = fs->make<TH1D>( "h420b2_123", "PXB2 residuals #Deltax, p_{t} > 12;PXB2 #Deltax [#mum];hits", 100, -150, 150 );
  h420b3_123 = fs->make<TH1D>( "h420b3_123", "PXB3 residuals #Deltax, p_{t} > 12;PXB3 #Deltax [#mum];hits", 100, -150, 150 );

  h421b1_123 = fs->make<TH1D>( "h421b1_123", "PXB1 residuals #Deltay, p_{t} > 12;PXB1 #Deltay [#mum];hits", 100, -300, 300 );
  h421b2_123 = fs->make<TH1D>( "h421b2_123", "PXB2 residuals #Deltay, p_{t} > 12;PXB2 #Deltay [#mum];hits", 100, -300, 300 );
  h421b3_123 = fs->make<TH1D>( "h421b3_123", "PXB3 residuals #Deltay, p_{t} > 12;PXB3 #Deltay [#mum];hits", 100, -300, 300 );

  h077b1_123 = fs->make<TH1D>( "h077b1_123", "PXB1 x error ", 100, 0., 100. );
  h077b2_123 = fs->make<TH1D>( "h077b2_123", "PXB2 x error ", 100, 0., 100. );
  h077b3_123 = fs->make<TH1D>( "h077b3_123", "PXB3 x error ", 100, 0., 100. );

  h078b1_123 = fs->make<TH1D>( "h078b1_123", "PXB1 y error ", 100, 0., 100. );
  h078b2_123 = fs->make<TH1D>( "h078b2_123", "PXB2 y error ", 100, 0., 100. );
  h078b3_123 = fs->make<TH1D>( "h078b3_123", "PXB3 y error ", 100, 0., 100. );

  h079b1_123 = fs->make<TH1D>( "h079b1_123", "PXB1 x pull ", 100, -10., 10. );
  h079b2_123 = fs->make<TH1D>( "h079b2_123", "PXB2 x pull ", 100, -10., 10. );
  h079b3_123 = fs->make<TH1D>( "h079b3_123", "PXB3 x pull ", 100, -10., 10. );

  h069b1_123 = fs->make<TH1D>( "h069b1_123", "PXB1 y pull ", 100, -10., 10. );
  h069b2_123 = fs->make<TH1D>( "h069b2_123", "PXB2 y pull ", 100, -10., 10. );
  h069b3_123 = fs->make<TH1D>( "h069b3_123", "PXB3 y pull ", 100, -10., 10. );
  
  h420b1_124 = fs->make<TH1D>( "h420b1_124", "PXB1 residuals #Deltax, p_{t} > 12;PXB1 #Deltax [#mum];hits", 100, -150, 150 );
  h420b2_124 = fs->make<TH1D>( "h420b2_124", "PXB2 residuals #Deltax, p_{t} > 12;PXB2 #Deltax [#mum];hits", 100, -150, 150 );
  h420b4_124 = fs->make<TH1D>( "h420b4_124", "PXB3 residuals #Deltax, p_{t} > 12;PXB3 #Deltax [#mum];hits", 100, -150, 150 );

  h421b1_124 = fs->make<TH1D>( "h421b1_124", "PXB1 residuals #Deltay, p_{t} > 12;PXB1 #Deltay [#mum];hits", 100, -300, 300 );
  h421b2_124 = fs->make<TH1D>( "h421b2_124", "PXB2 residuals #Deltay, p_{t} > 12;PXB2 #Deltay [#mum];hits", 100, -300, 300 );
  h421b4_124 = fs->make<TH1D>( "h421b4_124", "PXB3 residuals #Deltay, p_{t} > 12;PXB3 #Deltay [#mum];hits", 100, -300, 300 );

  h077b1_124 = fs->make<TH1D>( "h077b1_124", "PXB1 x error ", 100, 0., 100. );
  h077b2_124 = fs->make<TH1D>( "h077b2_124", "PXB2 x error ", 100, 0., 100. );
  h077b4_124 = fs->make<TH1D>( "h077b4_124", "PXB3 x error ", 100, 0., 100. );

  h078b1_124 = fs->make<TH1D>( "h078b1_124", "PXB1 y error ", 100, 0., 100. );
  h078b2_124 = fs->make<TH1D>( "h078b2_124", "PXB2 y error ", 100, 0., 100. );
  h078b4_124 = fs->make<TH1D>( "h078b4_124", "PXB3 y error ", 100, 0., 100. );

  h079b1_124 = fs->make<TH1D>( "h079b1_124", "PXB1 x pull ", 100, -10., 10. );
  h079b2_124 = fs->make<TH1D>( "h079b2_124", "PXB2 x pull ", 100, -10., 10. );
  h079b4_124 = fs->make<TH1D>( "h079b4_124", "PXB3 x pull ", 100, -10., 10. );

  h069b1_124 = fs->make<TH1D>( "h069b1_124", "PXB1 y pull ", 100, -10., 10. );
  h069b2_124 = fs->make<TH1D>( "h069b2_124", "PXB2 y pull ", 100, -10., 10. );
  h069b4_124 = fs->make<TH1D>( "h069b4_124", "PXB3 y pull ", 100, -10., 10. );

  h420b1_134 = fs->make<TH1D>( "h420b1_134", "PXB1 residuals #Deltax, p_{t} > 12;PXB1 #Deltax [#mum];hits", 100, -150, 150 );
  h420b3_134 = fs->make<TH1D>( "h420b3_134", "PXB3 residuals #Deltax, p_{t} > 12;PXB3 #Deltax [#mum];hits", 100, -150, 150 );
  h420b4_134 = fs->make<TH1D>( "h420b4_134", "PXB3 residuals #Deltax, p_{t} > 12;PXB3 #Deltax [#mum];hits", 100, -150, 150 );

  h421b1_134 = fs->make<TH1D>( "h421b1_134", "PXB1 residuals #Deltay, p_{t} > 12;PXB1 #Deltay [#mum];hits", 100, -300, 300 );
  h421b3_134 = fs->make<TH1D>( "h421b3_134", "PXB3 residuals #Deltay, p_{t} > 12;PXB3 #Deltay [#mum];hits", 100, -300, 300 );
  h421b4_134 = fs->make<TH1D>( "h421b4_134", "PXB3 residuals #Deltay, p_{t} > 12;PXB3 #Deltay [#mum];hits", 100, -300, 300 );

  h077b1_134 = fs->make<TH1D>( "h077b1_134", "PXB1 x error ", 100, 0., 100. );
  h077b3_134 = fs->make<TH1D>( "h077b3_134", "PXB3 x error ", 100, 0., 100. );
  h077b4_134 = fs->make<TH1D>( "h077b4_134", "PXB3 x error ", 100, 0., 100. );

  h078b1_134 = fs->make<TH1D>( "h078b1_134", "PXB1 y error ", 100, 0., 100. );
  h078b3_134 = fs->make<TH1D>( "h078b3_134", "PXB3 y error ", 100, 0., 100. );
  h078b4_134 = fs->make<TH1D>( "h078b4_134", "PXB3 y error ", 100, 0., 100. );

  h079b1_134 = fs->make<TH1D>( "h079b1_134", "PXB1 x pull ", 100, -10., 10. );
  h079b3_134 = fs->make<TH1D>( "h079b3_134", "PXB3 x pull ", 100, -10., 10. );
  h079b4_134 = fs->make<TH1D>( "h079b4_134", "PXB3 x pull ", 100, -10., 10. );

  h069b1_134 = fs->make<TH1D>( "h069b1_134", "PXB1 y pull ", 100, -10., 10. );
  h069b3_134 = fs->make<TH1D>( "h069b3_134", "PXB3 y pull ", 100, -10., 10. );
  h069b4_134 = fs->make<TH1D>( "h069b4_134", "PXB3 y pull ", 100, -10., 10. );

}

// member functions:
// method called once each job just before starting event loop

void Pixel_FPix_phase1::beginJob()
{
}

//----------------------------------------------------------------------
// method called for each event:

void Pixel_FPix_phase1::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
  const int run = iRun.run();

  std::map<int, Histos>::iterator iter = runmap.find(run);
  if(iter != runmap.end())
    {

      static_cast<Histos&>(*this) = iter->second;
    }
  else
    {
      edm::Service<TFileService> fs;

      std::stringstream runstr;
      runstr << "Run" << run;
      TFileDirectory subdir = fs->mkdir(runstr.str().c_str());

      if(doBPix){
	runmap[run].InitBPix_phase1(&subdir);
      }
      if(doFPix){
	runmap[run].InitFPix_phase1(&subdir);
      }
      static_cast<Histos&>(*this) = runmap[run];
    }
  
  //bool hltSetupChanged = false;
  /*
  if(_triggerSrc.label()!="") {
    if(!HLTConfig.init(iRun, iSetup, _triggerSrc.process(), hltSetupChanged))
      throw cms::Exception("Failed to init HLT config");
  }
  */
}

std::vector<double> Pixel_FPix_phase1::getIntersection(std::vector<double> point1, std::vector<double> point2, double rho,  const GeomDet *detHit, std::vector<double> intersection){
  /* 
         Takes two points and the curvature to create a helix, then finds the intersection with a detector plane
	 returns the (x,y,z) local coordinates for the intersection point
  */


  // Create helix from p1, p2 and rho
  SimpleHelix Hel = SimpleHelix(point1, point2, fabs(rho), &Pass);
  
  // Information about the detector plane
  double x_0 = detHit->position().x();
  double y_0 = detHit->position().y();
  double z_0 = detHit->position().z();

  double nX = detHit->surface().normalVector().x();
  double nY = detHit->surface().normalVector().y();
  double nZ = detHit->surface().normalVector().z();

  std::vector<double> plane = {x_0, y_0, z_0, nX, nY, nZ};

  // Find the intersection of the detector plane and helix
  nloops = Hel.SimpleHelix::pposdir(plane, intersection);

  // Convert global intersection point to local
  Surface::GlobalPoint EstimatedPoint(intersection[0],intersection[1],intersection[2]);
  Surface::LocalPoint LocalEstimate = detHit->toLocal(EstimatedPoint);

  // boolean of if the intersection is within the detector plane bounds - currently not used
  insideD = detHit->surface().bounds().inside(LocalEstimate);

  // Get Helix Parameters - currently not used
  Hel.SimpleHelix::parameters(radius, xcenter, ycenter, dzdphi, z0);
  
  std::vector<double> EstLocalCoordinates = {LocalEstimate.x(), LocalEstimate.y(), LocalEstimate.z()};

  return EstLocalCoordinates;
}



void Pixel_FPix_phase1::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  /*  Run for either/both FPix or BPix residuals  */
  dx_resolution_study_1.clear();
  dz_resolution_study_1.clear();
  dx_resolution_study_2.clear();
  dz_resolution_study_2.clear();
  dx_resolution_study_3.clear();
  dz_resolution_study_3.clear();
  pt_resolution_study.clear();
  pt_resolution_study_refit.clear();
  pt_all_tracks.clear();
  pt_4_tracks.clear();
  pt_12_tracks.clear();
  pt_highlayerswithmeasurements.clear();
  pt_lowlayerswithmeasurements.clear();
  TrackerLayersWithMeasurements.clear();
  cluster_size_all_tracks.clear();
  hits_on_track_barrel.clear();
  hits_on_track_endcap.clear();
  hits_on_track_tracker.clear();
  cluster_size_resolution_study.clear();
  layers_with_measurement.clear();
  disks_with_measurement.clear();
  //bool isTriplet;

  pt_res = -9999.;
  pt_res_refit = -9999.;
  cluster_size_res_init = 0;
  cluster_size_res = 0;
  hits_track = 0;
  hits_barrel = 0;
  hits_endcap = 0;
  ls_with_measure = 0;
  number_of_tracks = 0;

  if(doFPix ){
    std::string detTag = "fpix";
    Pixel_FPix_phase1::getResiduals(iEvent, iSetup, detTag);
  }
  if(doBPix){
    std::string detTag = "bpix";
    Pixel_FPix_phase1::getResiduals(iEvent, iSetup, detTag);
  }

}

void Pixel_FPix_phase1::getResiduals(const edm::Event & iEvent, const edm::EventSetup& iSetup, std::string detTag){

  using namespace std;
  using namespace edm;
  using namespace reco;
  using namespace math;
  
  //const double wt = 180/pi;  // Unused
  //const double pi = 4*atan(1); // Ditto 
  
  pt_res = -9999.;
  pt_res_refit = -9999.;
  cluster_size_res_init = 0;
  cluster_size_res = 0;
  hits_track = 0;
  hits_barrel = 0;
  ls_with_measure = 0;
  number_of_tracks = 0;
  runNumber_res = -1;
  lumiBlock_res = -1;

  int idbg = 0;

  if (Template) {
    Counter_Template::neve++;
    if( Counter_Template::prevrun != iEvent.run() ){
      time_t unixZeit = iEvent.time().unixTime();
      cout << "new run " << iEvent.run();
      cout << ", LumiBlock " << iEvent.luminosityBlock();
      cout << " taken " << ctime(&unixZeit); // ctime has endline
      Counter_Template::prevrun = iEvent.run();
    }// new run
    if( Counter_Template::neve < 2 ) idbg = 1;
  };
  if (Generic) {
    Counter_Generic::neve++;
    if( Counter_Generic::prevrun != iEvent.run() ){
      time_t unixZeit = iEvent.time().unixTime();
      cout << "new run " << iEvent.run();
      cout << ", LumiBlock " << iEvent.luminosityBlock();
      cout << " taken " << ctime(&unixZeit); // ctime has endline
      Counter_Generic::prevrun = iEvent.run();
    }// new run
    if( Counter_Generic::neve < 2 ) idbg = 1;
  };

  int jdbg = 0;
  if( idbg ) {
    cout << endl;
    cout << "run " << iEvent.run();
    cout << ", LumiBlock " << iEvent.luminosityBlock();
    cout << ", event " << iEvent.eventAuxiliary().event();
    time_t unixZeit = iEvent.time().unixTime();
    cout << ", taken " << ctime(&unixZeit); // ctime has endline
  }
  
  run_num=iEvent.run();
  lumi_block=iEvent.luminosityBlock();   
  
  //--------------------------------------------------------------------
  // beam spot:
  
  edm::Handle<reco::BeamSpot> rbs;
  //iEvent.getByLabel( "offlineBeamSpot", rbs );
  iEvent.getByToken( t_offlineBeamSpot_, rbs );

  XYZPoint bsP = XYZPoint(0,0,0);        // beam spot point
  //int ibs = 0;

  if( rbs.failedToGet() ) return;
  if( ! rbs.isValid() ) return;

  //ibs = 1;

  bsP = XYZPoint( rbs->x0(), rbs->y0(), rbs->z0() );

  //double bx = rbs->BeamWidthX();
  //double by = rbs->BeamWidthY();

  if( idbg ){
    cout << "beam spot x " << rbs->x0();
    cout << ", y " << rbs->y0();
    cout << ", z " << rbs->z0();
    cout << endl;
  }

  //Retrieve tracker topology from geometry
  //edm::ESHandle<TrackerTopology> tTopoH;
  //iSetup.get<TrackerTopologyRcd>().get(tTopoH);
  //const TrackerTopology *tTopo=tTopoH.product();
  const TrackerTopology* const tTopo = &iSetup.getData(esTokenTTopo_);
  //--------------------------------------------------------------------
  // primary vertices:
  Handle<VertexCollection> vertices;
  //iEvent.getByLabel( "offlinePrimaryVertices", vertices );
  iEvent.getByToken( t_offlinePrimaryVertices_,vertices );
  if( vertices.failedToGet() ) return;
  if( !vertices.isValid() ) return;

  // need vertex global point for tracks
  // from #include "DataFormats/GeometryVector/interface/GlobalPoint.h"
  // Global points are three-dimensional by default
  // typedef Global3DPoint  GlobalPoint;
  // typedef Point3DBase< float, GlobalTag> Global3DPoint;
  
  //XYZPoint vtxN = XYZPoint(0,0,0);    // Not sure of the purpose for vtxN
  XYZPoint vtxP = XYZPoint(0,0,0);    //
  Vertex bestPvx;

  
  //--------------------------------------------------------------------
  // MET:
  
  edm::Handle< edm::View<reco::PFMET> > pfMEThandle;
  //iEvent.getByLabel( "pfMet", pfMEThandle );
  iEvent.getByToken(t_pfMet_, pfMEThandle );
  //--------------------------------------------------------------------
  // get a fitter to refit TrackCandidates, the same fitter as used in standard reconstruction:
  // Fitter = cms.string('KFFittingSmootherWithOutliersRejectionAndRK'),
  // KalmanFilter
  // RungeKutta
  
  
#ifdef NEW_TRACKINGRECHITS

  // Fitter                                                                                                                                                                         
  //edm::ESHandle<TrajectoryFitter> aFitter;
  //iSetup.get<TrajectoryFitter::Record>().get("KFFittingSmootherWithOutliersRejectionAndRK",aFitter);
  auto const& aFitter = &iSetup.getData(fitterToken_);
  std::unique_ptr<TrajectoryFitter> theFitter = aFitter->clone();         // pointer which destroys object when pointer out of scope
  
  //----------------------------------------------------------------------------                                                                                                                        
  // Transient Rechit Builders                                                                                                                                                                          
  //edm::ESHandle<TransientTrackBuilder> theB;
  //iSetup.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );
  auto const& theB = &iSetup.getData(theBToken_);

  // Transient rec hits:                                                                                                                                                                                
  //ESHandle<TransientTrackingRecHitBuilder> hitBuilder;
  //iSetup.get<TransientRecHitRecord>().get( _ttrhBuilder, hitBuilder );
  auto const& hitBuilder = &iSetup.getData(hitBuilderToken_);

  // Cloner, New from 71Xpre7                                                                                                                                                                           
  const TkTransientTrackingRecHitBuilder * builder = static_cast<TkTransientTrackingRecHitBuilder const *>(hitBuilder);
  auto hitCloner = builder->cloner();
  theFitter->setHitCloner(&hitCloner);

#else
  //old
  ESHandle<TrajectoryFitter> TF;
  iSetup.get<TrajectoryFitter::Record>().get( "KFFittingSmootherWithOutliersRejectionAndRK", TF );
  std::unique_ptr<TrajectoryFitter> theFitter = TF->clone();
  
  edm::ESHandle<TransientTrackBuilder> theB;
  iSetup.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );
  
  // transient rec hits:                                                                                                                                                                                
  ESHandle<TransientTrackingRecHitBuilder> hitBuilder;
  iSetup.get<TransientRecHitRecord>().get( _ttrhBuilder, hitBuilder );

#endif
  //-------------------------------------------------------------------- 
  // Trackpropagator:
  //edm::ESHandle<Propagator> prop;
  //iSetup.get<TrackingComponentsRecord>().get( "PropagatorWithMaterial", prop );
  auto const& prop = &iSetup.getData(propToken_);
  const Propagator* thePropagator = prop;

  //--------------------------------------------------------------------
  // tracks:
  Handle<TrackCollection> tracks;
  //iEvent.getByLabel( "generalTracks", tracks );
  iEvent.getByToken( t_generalTracks_, tracks );

  if( tracks.failedToGet() ) return;

  if( !tracks.isValid() ) return;

  if( idbg ){
    cout << "  tracks " << tracks->size();
    cout << endl;
  }

  h030->Fill( tracks->size() );

  
  //----------------------------------------------------------------------------
  // transient track builder, needs B-field from data base (global tag in .py)
  
  
#if 0
  edm::ESHandle<SiPixelLorentzAngle> SiPixelLorentzAngle_; 
  iSetup.get<SiPixelLorentzAngleRcd>().get(SiPixelLorentzAngle_);
  std::map<unsigned int,float> detid_la= SiPixelLorentzAngle_->getLorentzAngles();  // get Lorentz angles for subdetectors
  std::map<unsigned int,float>::const_iterator it;
  for (it=detid_la.begin();it!=detid_la.end();it++)
    {
      unsigned int subdet   = DetId(it->first).subdetId();
      if(subdet == static_cast<int>(PixelSubdetector::PixelBarrel))   
	//std::cout << it->first << ": " << it->second << std::endl;
    }
#endif
  
  //----------------------------------------------------------------------------
  //                           Loop over tracks
  //----------------------------------------------------------------------------   

  double sumpt = 0;     // total pt of tracks from vtx
  double sumq = 0;      // total charge from vtx
  // Surface::GlobalPoint origin = Surface::GlobalPoint(0,0,0);
  number_of_tracks = tracks->size();
  runNumber_res = iEvent.run();
  lumiBlock_res = iEvent.luminosityBlock();
  for( TrackCollection::const_iterator iTrack = tracks->begin();
       iTrack != tracks->end(); ++iTrack ) {
    // cpt = cqRB = 0.3*R[m]*B[T] = 1.14*R[m] for B=3.8T
    // D = 2R = 2*pt/1.14
    // calo: D = 1.3 m => pt = 0.74 GeV/c

    dx_res_1 = -999.;
    dz_res_1 = -999.;
    dx_res_2 = -999.;
    dz_res_2 = -999.;
    dx_res_3 = -999.;
    dz_res_3 = -999.;
    pt_res = -9999.;
    pt_res_refit = -9999.;
    hits_barrel = -1;
    hits_track = -1;
    ls_with_measure = -1;
    
    isTriplet = true;
    double pt = iTrack->pt();

    numberOfTracksCount++;
    pt_all_tracks.push_back(pt);
    pt_12_tracks.push_back(pt);
   
    if( abs( iTrack->dxy(vtxP) ) > 5*iTrack->dxyError() ) continue; // if trans. IP > 5x its error, skip
    sumpt += pt;
    sumq += iTrack->charge();
    //double logpt = log(pt) / log(10);
    const reco::HitPattern& hp = iTrack->hitPattern();
    //double phi = iTrack->phi(); // Not used
    //double eta = iTrack->eta(); // ditto 
    
    h031->Fill( iTrack->charge() );
    h032->Fill( pt );

    h035->Fill( hp.numberOfValidTrackerHits() );
    h036->Fill( hp.numberOfValidPixelBarrelHits() );
    h036_1->Fill( hp.numberOfValidPixelEndcapHits() );
    h037->Fill( hp.trackerLayersWithMeasurement() );
    h038->Fill( hp.pixelBarrelLayersWithMeasurement() );
    h039->Fill( hp.pixelEndcapLayersWithMeasurement() );
    if(pt>FPIX_ptcut)     {
      h037_1->Fill( hp.trackerLayersWithMeasurement() );
      h040->Fill( iTrack->normalizedChi2());
      h041->Fill( iTrack->ptError());
      h042->Fill( iTrack->qualityMask());
      h043->Fill( hp.trackerLayersWithMeasurement(), iTrack->normalizedChi2());
      h044->Fill( hp.trackerLayersWithMeasurement(), iTrack->ptError());

    }
    TrackerLayersWithMeasurements.push_back(hp.trackerLayersWithMeasurement());
    if( hp.trackerLayersWithMeasurement() < 7 ){
      pt_lowlayerswithmeasurements.push_back(pt);
    };
    if( hp.trackerLayersWithMeasurement() < 7 ) continue;
    
    pt_highlayerswithmeasurements.push_back(pt);
    // transient track:    
    TransientTrack tTrack = theB->build(*iTrack);
    TrajectoryStateOnSurface initialTSOS = tTrack.innermostMeasurementState();
    double kap = tTrack.initialFreeState().transverseCurvature();                          // curvature of track
    rho = 1/kap;  
    
  
    //--------------------------------------------------------------------------
    //   Get the Pixel Hits from the track for the triplet
    //--------------------------------------------------------------------------   

    // rec hits from track extra:
    if( iTrack->extra().isNull() ) continue;//next track
    if( ! iTrack->extra().isAvailable() ) continue;//next track
    uint32_t innerDetId = 0;
    double xPX1 = 0;      // global x hit 1
    double yPX1 = 0;      // global y hit 1
    double zPX1 = 0;      // global z hit 1
    double ePX1 = 0;      // sqrt(covariance of x)
    double fPX1 = 0;      // sqrt(covariance of y)
    
    double xPX2 = 0;
    double yPX2 = 0;
    double zPX2 = 0;
    double ePX2 = 0;
    double fPX2 = 0;
    
    double xPX3 = 0;
    double yPX3 = 0;
    double zPX3 = 0;
    double ePX3 = 0;
    double fPX3 = 0;

    double xPX4 = 0;
    double yPX4 = 0;
    double zPX4 = 0;
    double ePX4 = 0;
    double fPX4 = 0;

    int n1 = 0;
    int n2 = 0;
    int n3 = 0;
    int n4 = 0;
    const GeomDet * det1 = NULL;  // Detector for first hit
    const GeomDet * det2 = NULL;
    const GeomDet * det3 = NULL;
    const GeomDet * det4 = NULL;    

    //**********

    double xPX2_r1 = 0;
    double yPX2_r1 = 0;
    double zPX2_r1 = 0;
    
    double xPX3_r1 = 0;
    double yPX3_r1 = 0;
    double zPX3_r1 = 0;

    double xPX4_r1 = 0;
    double yPX4_r1 = 0;
    double zPX4_r1 = 0;

    int n2_r1 = 0;
    int n3_r1 = 0;
    int n4_r1 = 0;

    const GeomDet * det2_r1 = NULL;
    const GeomDet * det3_r1 = NULL;
    const GeomDet * det4_r1 = NULL;    

    //**********
    
    double xPX2_r2 = 0;
    double yPX2_r2 = 0;
    double zPX2_r2 = 0;
    
    double xPX3_r2 = 0;
    double yPX3_r2 = 0;
    double zPX3_r2 = 0;

    double xPX4_r2 = 0;
    double yPX4_r2 = 0;
    double zPX4_r2 = 0;

    int n2_r2 = 0;
    int n3_r2 = 0;
    int n4_r2 = 0;

    const GeomDet * det2_r2 = NULL;
    const GeomDet * det3_r2 = NULL;
    const GeomDet * det4_r2 = NULL;    

    edm::OwnVector<TrackingRecHit> recHitVector;                     // for seed
    std::vector<TransientTrackingRecHit::RecHitPointer> myTTRHvec;
    Trajectory::RecHitContainer coTTRHvec;                           // for fit, constant
    
    // loop over recHits on this track:
    for( trackingRecHit_iterator irecHit = iTrack->recHitsBegin();
	 irecHit != iTrack->recHitsEnd(); ++irecHit ) {
      DetId detId = (*irecHit)->geographicalId();                          // get detector 
      uint32_t subDet = detId.subdetId();                                  // get subdetector
      // enum Detector { Tracker=1, Muon=2, Ecal=3, Hcal=4, Calo=5 };
      if( detId.det() != 1 ){
	cout << "rec hit ID = " << detId.det() << " not in tracker!?!?\n";
	continue;
      }
      
      recHitVector.push_back( (*irecHit)->clone() );
            
      // build transient hit: 
      
#ifdef NEW_TRACKINGRECHITS
      auto tmprh = (*irecHit)->cloneForFit(*builder->geometry()->idToDet((**irecHit).geographicalId()));
      auto transRecHit = hitCloner.makeShared(tmprh, initialTSOS);
      
#else
      TransientTrackingRecHit::RecHitPointer transRecHit = hitBuilder->build( &*(*irecHit), initialTSOS);
#endif	 
      
      myTTRHvec.push_back( transRecHit );
      coTTRHvec.push_back( transRecHit );
      
      if( ! (*irecHit)->isValid() ) continue;
      
      double xloc = transRecHit->localPosition().x();       // 1st meas coord
      double yloc = transRecHit->localPosition().y();       // 2nd meas coord or zero
      double vxloc = transRecHit->localPositionError().xx();//covariance
      double vyloc = transRecHit->localPositionError().yy();//covariance
      double gX = transRecHit->globalPosition().x();
      double gY = transRecHit->globalPosition().y();
      double gZ = transRecHit->globalPosition().z();
      if( transRecHit->canImproveWithTrack() ) {//use z from track to apply alignment
	//if( idbg ) cout << "  try to improve\n";
	TrajectoryStateOnSurface propTSOS = thePropagator->propagate( initialTSOS, transRecHit->det()->surface() );
	if( propTSOS.isValid() ){

#ifdef NEW_TRACKINGRECHITS
	  auto preciseHit = hitCloner.makeShared(tmprh,propTSOS); //pre7  
#else
	  TransientTrackingRecHit::RecHitPointer preciseHit = transRecHit->clone(propTSOS);   // adding in trajetory with detector effects
#endif
	  //if( idbg ) cout << "  have preciseHit\n";
	  xloc = preciseHit->localPosition().x();// 1st meas coord
	  yloc = preciseHit->localPosition().y();// 2nd meas coord or zero

	  vxloc = preciseHit->localPositionError().xx();//covariance
	  vyloc = preciseHit->localPositionError().yy();//covariance
	  if( idbg ) {
	    //cout << "  improved hit in "   << subDet;
	    //cout << setprecision(4);
	    //cout << ", xloc from "         << transRecHit->localPosition().x();
	    //cout << " to "                 << preciseHit->localPosition().x();
	    //cout << ", yloc from "         << transRecHit->localPosition().y();
	    //cout << " to "                 << preciseHit->localPosition().y();
	    //cout << endl;
	  }
	  
	  gX = preciseHit->globalPosition().x();
	  gY = preciseHit->globalPosition().y();
	  gZ = preciseHit->globalPosition().z();
	  
	}//valid propTSOS
	else{
	  if( idbg ) cout << "  propTSOS not valid\n";
	}
      }//canImprove
      
      // ======================================================================
      //  Fill BPIX points
      // ======================================================================
      if(detTag == "bpix"){
      
	// PXB:      
	if( subDet == PixelSubdetector::PixelBarrel ) {
	  
	  int ilay=tTopo->pxbLayer(detId);
	  
	  if( ilay == 1 ) {
	    
	    n1++;
	    xPX1 = gX;
	    yPX1 = gY;
	    zPX1 = gZ;
	    xpx1_l = xloc;
	    xpy1_l = yloc;
	    ePX1 = sqrt( vxloc );
	    fPX1 = sqrt( vyloc );

	    det1 = transRecHit->det();	    
	  }//PXB1
	  
	  if( ilay == 2){
	    n2++;
	    xPX2 = gX;    // precise hit in CMS global coordinates
	    yPX2 = gY;
	    zPX2 = gZ;
	    xpx2_l = xloc;  // precise hit in local coordinates (w.r.t. sensor center)
	    xpy2_l = yloc;
	    ePX2 = sqrt( vxloc );
	    fPX2 = sqrt( vyloc );
	    
	    det2 = transRecHit->det();
	  
	    
	  }//PXB2
	  
	  if( ilay == 3 ){
	    
	    n3++;
	    xPX3 = gX;
	    yPX3 = gY;
	    zPX3 = gZ;
	    xpx3_l = xloc;
	    xpy3_l = yloc;
	    ePX3 = sqrt( vxloc );
	    fPX3 = sqrt( vyloc );

	    det3 = transRecHit->det();
	    
	  }//PXB3
	  if( ilay == 4 ){
	    
	    n4++;
	    xPX4 = gX;
	    yPX4 = gY;
	    zPX4 = gZ;
	    xpx4_l = xloc;
	    xpy4_l = yloc;
	    ePX4 = sqrt( vxloc );
	    fPX4 = sqrt( vyloc );

	    det4 = transRecHit->det();
	    
	    }//PXB4	  
	}// Pixel

      }// doBPIX

      // ==============================================================
      //  Fill FPIX points
      // ============================================================== 
      else if(detTag == "fpix"){
	
	// PXB:      
	if( subDet == PixelSubdetector::PixelBarrel ) {

	  int ilay=tTopo->pxbLayer(detId);
	  
	  if( ilay == 2 ) {
	    
	    n1++;
	    xPX1 = gX;
	    yPX1 = gY;
	    zPX1 = gZ;
	    xpx1_l = xloc;
	    xpy1_l = yloc;	        
  	    ePX1 = sqrt( vxloc );
	    fPX1 = sqrt( vyloc );

	    det1 = transRecHit->det();

	  }// BPIX1
	  
	}//PXB1
    
	if( subDet == PixelSubdetector::PixelEndcap) {

	  int idisk=tTopo->pxfDisk(detId); 
	  int blade  = tTopo->pxfBlade(detId);     // Phase 1: Inner blades 1-22, Outer blades 23-56
	  int ring = 1 + (blade>22);               // Phase 1: Inner: 1, Outer: 2
	  
	  if( idisk == 1 ){
	    n2++;
	    xPX2 = gX; // precise hit in CMS global coordinates
	    yPX2 = gY;
	    zPX2 = gZ;
	    xpx2_l = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	    xpy2_l = yloc;
	    ePX2 = sqrt( vxloc );
	    fPX2 = sqrt( vyloc );
	        	        
	    det2 = transRecHit->det();
	        
	    const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	    
	    if( pixhit->hasFilledProb() ){
	      clusProb_FPix_phase1 = pixhit->clusterProbability(0);                                      
	    }
	   
	    edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();

	    if( clust.isNonnull() ) {
	      clusSize_Y = clust->sizeY();
	      clusSize_X = clust->sizeX();
	    }
	    
	    if( ring == 2 ){
	      n2_r2++;
	      xPX2_r2 = gX; // precise hit in CMS global coordinates
	      yPX2_r2 = gY;
	      zPX2_r2 = gZ;
	      xpx2_l_r2 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy2_l_r2 = yloc;
	      //ePX2_r2 = sqrt( vxloc );
	      //fPX2_r2 = sqrt( vyloc );
	      
	      det2_r2 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }


	    if( ring == 1 ){
	      n2_r1++;
	      xPX2_r1 = gX; // precise hit in CMS global coordinates
	      yPX2_r1 = gY;
	      zPX2_r1 = gZ;
	      xpx2_l_r1 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy2_l_r1 = yloc;
	      
	      det2_r1 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }


	  }//PXF1
	    
	  if( idisk == 2 ){
	        
	    n3++;
	    xPX3 = gX;
	    yPX3 = gY;
	    zPX3 = gZ;
	    xpx3_l = xloc;
	    xpy3_l = yloc;
	    ePX3 = sqrt( vxloc );
	    fPX3 = sqrt( vyloc );

	    det3 = transRecHit->det();


	    if( ring == 2 ){
	      n3_r2++;
	      xPX3_r2 = gX; // precise hit in CMS global coordinates
	      yPX3_r2 = gY;
	      zPX3_r2 = gZ;
	      xpx3_l_r2 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy3_l_r2 = yloc;
	      
	      det3_r2 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }


	    if( ring == 1 ){
	      n3_r1++;
	      xPX3_r1 = gX; // precise hit in CMS global coordinates
	      yPX3_r1 = gY;
	      zPX3_r1 = gZ;
	      xpx3_l_r1 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy3_l_r1 = yloc;
	      
	      det3_r1 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }


	  }//PXF2
	  	  
	  if( idisk == 3 ){
	        
	    n4++;
	    xPX4 = gX;
	    yPX4 = gY;
	    zPX4 = gZ;
	    xpx4_l = xloc;
	    xpy4_l = yloc;
	    ePX4 = sqrt( vxloc );
	    fPX4 = sqrt( vyloc );

	    det4 = transRecHit->det();

	    if( ring == 2 ){
	      n4_r2++;
	      xPX4_r2 = gX; // precise hit in CMS global coordinates
	      yPX4_r2 = gY;
	      zPX4_r2 = gZ;
	      xpx4_l_r2 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy4_l_r2 = yloc;
	      
	      det4_r2 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }

	    if( ring == 1 ){
	      n4_r1++;
	      xPX4_r1 = gX; // precise hit in CMS global coordinates
	      yPX4_r1 = gY;
	      zPX4_r1 = gZ;
	      xpx4_l_r1 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	      xpy4_l_r1 = yloc;

	      det4_r1 = transRecHit->det();
	      
	      const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );
	      edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();
	    }

	    }//PXF3

	}//PXF
	
      }//doFPix
      else{
	cout << "detector tag not specified"<< endl;
      }
    
    }//loop rechits

    //------------------------------------------------------------------------
    // refit the track:
    PTrajectoryStateOnDet PTraj = trajectoryStateTransform::persistentState( initialTSOS, innerDetId );
    const TrajectorySeed seed( PTraj, recHitVector, alongMomentum );
    
    //if( idbg ) cout << "  have seed\n";

    std::vector<Trajectory> refitTrajectoryCollection = theFitter->fit( seed, coTTRHvec, initialTSOS );
    
    if( refitTrajectoryCollection.size() > 0 ) { // should be either 0 or 1            
      const Trajectory& refitTrajectory = refitTrajectoryCollection.front();
      // Trajectory.measurements:
      Trajectory::DataContainer refitTMs = refitTrajectory.measurements();
      if( idbg ) {
	//cout << "  refitTrajectory has " << refitTMs.size() <<" hits in subdet";
      }

      //pt_res_refit = refitTrajectory.geometricalInnermostState().globalMomentum().perp();

      // hits in subDet:
      
      if( idbg ) {
	for( Trajectory::DataContainer::iterator iTM = refitTMs.begin();
	     iTM != refitTMs.end(); iTM++ ) {
	  
	  TransientTrackingRecHit::ConstRecHitPointer iTTRH = iTM->recHit();
	  if( iTTRH->hit()->isValid() ){
	    //cout << "  " << iTTRH->geographicalId().subdetId();
	  }
	}
	//cout << endl;
	
	//cout << "         pt " << refitTrajectory.geometricalInnermostState().globalMomentum().perp();
	//cout << ", eta " << refitTrajectory.geometricalInnermostState().globalMomentum().eta();
	//cout << ", phi " << refitTrajectory.geometricalInnermostState().globalMomentum().barePhi()*wt;
	//cout << ", at R " << refitTrajectory.geometricalInnermostState().globalPosition().perp();
	//cout << ", z " << refitTrajectory.geometricalInnermostState().globalPosition().z();
	//cout << ", phi " << refitTrajectory.geometricalInnermostState().globalPosition().barePhi()*wt;
	//cout << endl;
	
      }//dbg
      
      // trajectory residuals:
      
      for( Trajectory::DataContainer::iterator iTM = refitTMs.begin(); iTM != refitTMs.end(); iTM++ ) {
	if( ! iTM->recHit()->isValid() ) continue;
	DetId detId = iTM->recHit()->geographicalId();
	uint32_t subDet = detId.subdetId();
	
	// enum SubDetector{ PixelBarrel=1, PixelEndcap=2 };
	// enum SubDetector{ TIB=3, TID=4, TOB=5, TEC=6 };
	
	double xHit = iTM->recHit()->localPosition().x(); // primary measurement direction
	double yHit = iTM->recHit()->localPosition().y(); // always 0 in strips
	
	double dx = xHit - iTM->predictedState().localPosition().x();
	double dy = yHit - iTM->predictedState().localPosition().y();
	//double vxh = iTM->recHit()->localPositionError().xx();//covariance Not used
	//double vxt = iTM->predictedState().localError().positionError().xx();// ditto
	
	//if( subDet == 1 && idbg ){//1=PXB
	//if( subDet == 4 && idbg ){4=TID
	  //cout << "  predictdStateResid = " << dx*1E4 << " um";
	  //cout << ", eh = " << sqrt(vxh)*1E4 << " um";
	  //cout << ", et = " << sqrt(vxt)*1E4 << " um";
	  //cout << endl;
	//}

	TrajectoryStateOnSurface combinedPredictedState =
	  TrajectoryStateCombiner().combine( iTM->forwardPredictedState(), iTM->backwardPredictedState() );
	
	if( ! combinedPredictedState.isValid() ) continue;//skip hit
	
	if( jdbg ) cout << "  have combinedPredictedState\n";
	
	double xptch;
	double yptch;	
	
	if( subDet <  3 ){//1,2=pixel
	  PixelTopology & pixelTopol = (PixelTopology&) iTM->recHit()->detUnit()->topology();
	  xptch = pixelTopol.pitch().first;
	  yptch = pixelTopol.pitch().second;
	}
	else {//strip
	  StripTopology & stripTopol = (StripTopology&) iTM->recHit()->detUnit()->topology();
	  xptch = stripTopol.localPitch( combinedPredictedState.localPosition() );
	  yptch = stripTopol.localStripLength( combinedPredictedState.localPosition() );
	}
      
	dx = xHit - combinedPredictedState.localPosition().x(); //x = primary measurement
	dy = yHit - combinedPredictedState.localPosition().y(); //
	//vxh = iTM->recHit()->localPositionError().xx();//covariance Unused
	//vxt = combinedPredictedState.localError().positionError().xx();// ditto
	
	// angles of incidence:
	// local z = upwards = normal vector
	// local x = primary measurement direction
	// local y = secondary measurement direction
	
	// use Topology. no effect in PXB, essential in TID, TEC
	
	const Topology* theTopology = &(iTM->recHit()->detUnit()->topology() );
	    
	// MeasurementPoint [pitch] (like channel number)
	
	// TODO: Use the measurementPosition(point, trackdir) version of this function in order to take bows into account!
	MeasurementPoint hitMeasurement = theTopology->measurementPosition( iTM->recHit()->localPosition() );
	
	// TID and TEC have trapezoidal detectors:
	// translation from channel number into local x depends on local y
	// track prediction has local x,y => can convert into proper channel number MeasurementPoint:
	
	// TODO: Use the measurementPosition(point, trackdir) version of this function in order to take bows into account!
	MeasurementPoint combinedPredictedMeasurement = theTopology->measurementPosition( combinedPredictedState.localPosition() );

	dx = hitMeasurement.x() - combinedPredictedMeasurement.x(); //in units of pitch  (pitch = size of pixel or strip)
	dy = hitMeasurement.y() - combinedPredictedMeasurement.y(); //in units of pitch
	dx = dx * xptch;//convert back into [cm] using local pitch
	dy = dy * yptch;//[cm]
	
	// use topology: needed for TEC	
	double xx = hitMeasurement.x();
	double yy;
	if( subDet < 3 ) // pixel is 2D
	  yy = hitMeasurement.y();
	else // strips are 1D
	  yy = combinedPredictedMeasurement.y();
	
	MeasurementPoint mp( xx, yy );
	
	//2012 StripTopology & stripTopol = (StripTopology&) iTM->recHit()->detUnit()->topology();
	
	Surface::LocalPoint lp = theTopology->localPosition( mp );
	const GeomDet * myGeomDet = iTM->recHit()->det(); // makes no difference in TEC
	Surface::GlobalPoint gp = myGeomDet->toGlobal( lp );
	
	double gX = gp.x();
	double gY = gp.y();
	double gZ = gp.z();
	
	double lX = lp.x();
	double lY = lp.y();


	//overwrite PXB global coordinates once more, using topology:

	// =================================================
	// Modify BPIX Points with topology refit
	// =================================================
	if(detTag == "bpix"){
	  if( subDet == PixelSubdetector::PixelBarrel ) {
	        
	    int ilay=tTopo->pxbLayer(detId);
 
	    if( ilay == 1 ) {
	      xPX1 = gX;
	      yPX1 = gY;
	      zPX1 = gZ;

	      xpx1_l=lX;
	      xpy1_l=lY;
	      det1 = iTM->recHit()->det();
	    }// layer 1
	    else if( ilay == 2 ) {
	      xPX2 = gX;
	      yPX2 = gY;
	      zPX2 = gZ;
	            
	      xpx2_l=lX;
	      xpy2_l=lY;
	      det2 = iTM->recHit()->det();
	    }// layer 2
	    else if( ilay == 3 ) {
	      xPX3 = gX;
	      yPX3 = gY;
	      zPX3 = gZ;
	            
	      xpx3_l=lX;
	      xpy3_l=lY;
	      det3 = iTM->recHit()->det();
	    }// layer 3
	    else if( ilay == 4) {
	      xPX4 = gX;
	      yPX4 = gY;
	      zPX4 = gZ;
	      
	      xpx4_l=lX;
	      xpy4_l=lY;
	      det4 = iTM->recHit()->det();
	      }// layer 4
	      
	        
	  }// barrel
	  }// doBPix

	// ================================================
	// Modify FPIX Points with topology refit
	// ================================================
	else if(detTag =="fpix"){

	  if( subDet == PixelSubdetector::PixelBarrel ) {

	    int ilay=tTopo->pxbLayer(detId);

	    if( ilay == 2 ) {
	      xPX1 = gX;
	      yPX1 = gY;
	      zPX1 = gZ;

	      xpx1_l=lX;
	      xpy1_l=lY;
	      det1 = iTM->recHit()->det();
	    }// layer 1
	  }// barrel	  
	  else if( subDet == PixelSubdetector::PixelEndcap) {
	    
	    int idisk=tTopo->pxfDisk(detId);
	    int blade  = tTopo->pxfBlade(detId);     // Phase 1: Inner blades 1-22, Outer blades 23-56
	    int ring = 1 + (blade>22);               // Phase 1: Inner: 1, Outer: 2

	    if( idisk == 1 ) {
	      xPX2 = gX;
	      yPX2 = gY;
	      zPX2 = gZ;
	      
	      xpx2_l=lX;
	      xpy2_l=lY;
	      det2 = iTM->recHit()->det();
	    
	      if( ring == 2 ){
		xpx2_l_r2 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy2_l_r2 = lY;
	      }

	      if( ring == 1 ){
		xpx2_l_r1 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy2_l_r1 = lY;
	    }

	    }// disk 1
	    else if( idisk == 2 ) {
	      xPX3 = gX;
	      yPX3 = gY;
	      zPX3 = gZ;
	      
	      xpx3_l=lX;
	      xpy3_l=lY;
	      det3 = iTM->recHit()->det();


	      if( ring == 2 ){
		xPX3_r2 = gX; // precise hit in CMS global coordinates
		yPX3_r2 = gY;
		zPX3_r2 = gZ;
		xpx3_l_r2 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy3_l_r2 = lY;
		
		det3_r2 = iTM->recHit()->det();
	      }

	      if( ring == 1 ){
		xPX3_r1 = gX; // precise hit in CMS global coordinates
		yPX3_r1 = gY;
		zPX3_r1 = gZ;
		xpx3_l_r1 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy3_l_r1 = lY;
		
		det3_r1 = iTM->recHit()->det(); 
	    }


	    }// disk 2
	    else if( idisk == 3 ) {
	      xPX4 = gX;
	      yPX4 = gY;
	      zPX4 = gZ;
	      
	      xpx4_l=lX;
	      xpy4_l=lY;
	      det4 = iTM->recHit()->det();

	      if( ring == 2 ){
		xPX4_r2 = gX; // precise hit in CMS global coordinates
		yPX4_r2 = gY;
		zPX4_r2 = gZ;
		xpx4_l_r2 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy4_l_r2 = lY;
		
		det4_r2 = iTM->recHit()->det();
	      }

	      if( ring == 1 ){
		xPX4_r1 = gX; // precise hit in CMS global coordinates
		yPX4_r1 = gY;
		zPX4_r1 = gZ;
		xpx4_l_r1 = lX; // precise hit in local coordinates (w.r.t. sensor center)
		xpy4_l_r1 = lY;
		
		det4_r1 = iTM->recHit()->det();
	    }


	      }// disk 3
	    
	  }// endcaps
	}// doFPix
	else {
	  cout << "detTag not specified"<<endl;
	}
	
      }//loop iTM
      
    }//refitted trajectory

    //------------------------------------------------------------------------
    // 1-2-3 pixel triplet:
    if( n1*n2*n3 > 0 ) {

      {// let's open a scope, so we can redefine the variables further down
	
	// create points to feed helix
	std::vector<double> p1 = {xPX1, yPX1, zPX1};
	std::vector<double> p2 = {xPX2, yPX2, zPX2};
	std::vector<double> p3 = {xPX3, yPX3, zPX3};
	
	std::vector<double> intersection1 = {};
	std::vector<double> intersection2 = {};
	std::vector<double> intersection3 = {};
	
	// Create helix from two points and curvature, return the intersection point in local coordinates
	std::vector<double> IntersectionPointLocal_1 = Pixel_FPix_phase1::getIntersection(p2, p3, rho, det1, intersection1); 
	std::vector<double> IntersectionPointLocal_2 = Pixel_FPix_phase1::getIntersection(p1, p3, rho, det2, intersection2);
	std::vector<double> IntersectionPointLocal_3 = Pixel_FPix_phase1::getIntersection(p1, p2, rho, det3, intersection3);
	
	// Intersection point in local coordinates
	xl_ideal_1 = IntersectionPointLocal_1[0];
	yl_ideal_1 = IntersectionPointLocal_1[1];
	
	xl_ideal_2 = IntersectionPointLocal_2[0];
	yl_ideal_2 = IntersectionPointLocal_2[1];
	
	xl_ideal_3 = IntersectionPointLocal_3[0];
	yl_ideal_3 = IntersectionPointLocal_3[1];
	
	// Residuals with rechit and intersection point
	residual_x_1= (xpx1_l - xl_ideal_1)*1E4;
	residual_y_1= (xpy1_l - yl_ideal_1)*1E4;
	
	residual_x_2= (xpx2_l - xl_ideal_2)*1E4;
	residual_y_2= (xpy2_l - yl_ideal_2)*1E4;
	
	residual_x_3= (xpx3_l - xl_ideal_3)*1E4;
	residual_y_3= (xpy3_l - yl_ideal_3)*1E4;
	
	if(n2_r1>0){
	  std::vector<double> p2_r1 = {xPX2_r1, yPX2_r1, zPX2_r1};
	  std::vector<double> intersection2_r1 = {};
	  std::vector<double> IntersectionPointLocal_2_r1 = Pixel_FPix_phase1::getIntersection(p1, p3, rho, det2_r1, intersection2_r1);
          xl_ideal_2_r1 = IntersectionPointLocal_2_r1[0];
          yl_ideal_2_r1 = IntersectionPointLocal_2_r1[1];
          residual_x_2_r1= (xpx2_l_r1 - xl_ideal_2_r1)*1E4;
          residual_y_2_r1= (xpy2_l_r1 - yl_ideal_2_r1)*1E4;
        }
	
	if(n2_r2>0){
	  std::vector<double> p2_r2 = {xPX2_r2, yPX2_r2, zPX2_r2};
	  std::vector<double> intersection2_r2 = {};
	  std::vector<double> IntersectionPointLocal_2_r2 = Pixel_FPix_phase1::getIntersection(p1, p3, rho, det2_r2, intersection2_r2);
          xl_ideal_2_r2 = IntersectionPointLocal_2_r2[0];
          yl_ideal_2_r2 = IntersectionPointLocal_2_r2[1];
          residual_x_2_r2= (xpx2_l_r2 - xl_ideal_2_r2)*1E4;
          residual_y_2_r2= (xpy2_l_r2 - yl_ideal_2_r2)*1E4;
        }

	// Local errors for rechit
	x_local_error_1 = ePX1*1E4;
	y_local_error_1 = fPX1*1E4;
	
	x_local_error_2 = ePX2*1E4;
	y_local_error_2 = fPX2*1E4;
	
	x_local_error_3 = ePX3*1E4;
	y_local_error_3 = fPX3*1E4;
      
	// Add errors and pulls
	double pulx1=0.;
	double pulx2=0.;
	double pulx3=0.;
	double puly1=0.;
	double puly2=0.;
	double puly3=0.;
	if(x_local_error_1!=0.0) pulx1 = residual_x_1/x_local_error_1;
	if(x_local_error_2!=0.0) pulx2 = residual_x_2/x_local_error_2;
	if(x_local_error_3!=0.0) pulx3 = residual_x_3/x_local_error_3;

	if(y_local_error_1!=0.0) puly1 = residual_y_1/y_local_error_1;
	if(y_local_error_2!=0.0) puly2 = residual_y_2/y_local_error_2;
	if(y_local_error_3!=0.0) puly3 = residual_y_3/y_local_error_3;

	// Fill Histograms for FPIX
	if(detTag == "fpix"){

	  if( pt > 0.8) {
            isTriplet = true;
            numberOfTracksCount123++;
          }
	  //pt_12_tracks.push_back(pt);
	  if(pt>FPIX_ptcut){
	    pt_4_tracks.push_back(pt);
	    dx_res_1 = residual_x_2;
	    dz_res_1 = residual_y_2;

	    hclusprob_fpix ->Fill(clusProb_FPix_phase1);
	    h420f2_123_eta->Fill(iTrack->eta(), abs(residual_x_2));
	    h421f2_123_eta->Fill(iTrack->eta(), abs(residual_y_2));

	    h420f1_123->Fill( residual_x_1 ); 
	    h421f1_123->Fill( residual_y_1 );
	    
	    h420f2_123->Fill( residual_x_2 );
	    h421f2_123->Fill( residual_y_2 );
	    
	    h420f3_123->Fill( residual_x_3 );
	    h421f3_123->Fill( residual_y_3 );
	    
	    if(n2_r1>0){
	      h420f2_123_r1->Fill( residual_x_2_r1 );
	      h421f2_123_r1->Fill( residual_y_2_r1 );
	    }

	    if(n2_r2>0){
	      h420f2_123_r2->Fill( residual_x_2_r2 );
	      h421f2_123_r2->Fill( residual_y_2_r2 );
	    }

	    // Fill errors
	    h077f1_123->Fill( x_local_error_1 );
	    h078f1_123->Fill( y_local_error_1 );
	    
	    h077f2_123->Fill( x_local_error_2 );
	    h078f2_123->Fill( y_local_error_2 );
	  
	    h077f3_123->Fill( x_local_error_3 );
	    h078f3_123->Fill( y_local_error_3 );

	    // Fill pulls
	    h079f1_123->Fill( pulx1 );
	    h069f1_123->Fill( puly1 );

	    h079f2_123->Fill( pulx2 );
	    h069f2_123->Fill( puly2 );

	    h079f3_123->Fill( pulx3 );
	    h069f3_123->Fill( puly3 );

	  }
	}
      
	// Fill Histograms for BPIX
	else if(detTag == "bpix"){
	  if(pt>BPIX_ptcut){	  
	    //pt_12_tracks.push_back(pt);
	    h420b1_123->Fill( residual_x_1 );
	    h421b1_123->Fill( residual_y_1 );

	    h420b2_123->Fill( residual_x_2 );
	    h421b2_123->Fill( residual_y_2 );

	    h420b3_123->Fill( residual_x_3 );
	    h421b3_123->Fill( residual_y_3 );

	    // Fill errors
	    h077b1_123->Fill( x_local_error_1 );
	    h078b1_123->Fill( y_local_error_1 );

	    h077b2_123->Fill( x_local_error_2 );
	    h078b2_123->Fill( y_local_error_2 );

	    h077b3_123->Fill( x_local_error_3 );
	    h078b3_123->Fill( y_local_error_3 );

	    // Fill pulls
	    h079b1_123->Fill( pulx1 );
	    h069b1_123->Fill( puly1 );

	    h079b2_123->Fill( pulx2 );
	    h069b2_123->Fill( puly2 );

	    h079b3_123->Fill( pulx3 );
	    h069b3_123->Fill( puly3 );
	  }
	}
	else{}

      }//triplet 
    }// three hits: 1-2-3

    // 1-2-4 pixel triplet:
    if( n1*n2*n4 > 0 ) {

      {// let's open a scope, so we can redefine the variables further down
       
	// create points to feed helix
	std::vector<double> p1 = {xPX1, yPX1, zPX1};
	std::vector<double> p2 = {xPX2, yPX2, zPX2};
	std::vector<double> p4 = {xPX4, yPX4, zPX4};
	
	std::vector<double> intersection1 = {};
	std::vector<double> intersection2 = {};
	std::vector<double> intersection4 = {};
	
	// Create helix from two points and curvature, return the intersection point in local coordinates
	std::vector<double> IntersectionPointLocal_1 = Pixel_FPix_phase1::getIntersection(p2, p4, rho, det1, intersection1); 
	std::vector<double> IntersectionPointLocal_2 = Pixel_FPix_phase1::getIntersection(p1, p4, rho, det2, intersection2);
	std::vector<double> IntersectionPointLocal_4 = Pixel_FPix_phase1::getIntersection(p1, p2, rho, det4, intersection4);
	
	// Intersection point in local coordinates
	xl_ideal_1 = IntersectionPointLocal_1[0];
	yl_ideal_1 = IntersectionPointLocal_1[1];
	
	xl_ideal_2 = IntersectionPointLocal_2[0];
	yl_ideal_2 = IntersectionPointLocal_2[1];
	
	xl_ideal_4 = IntersectionPointLocal_4[0];
	yl_ideal_4 = IntersectionPointLocal_4[1];
	
	// Residuals with rechit and intersection point
	residual_x_1= (xpx1_l - xl_ideal_1)*1E4;
	residual_y_1= (xpy1_l - yl_ideal_1)*1E4;
	
	residual_x_2= (xpx2_l - xl_ideal_2)*1E4;
	residual_y_2= (xpy2_l - yl_ideal_2)*1E4;
	
	residual_x_4= (xpx4_l - xl_ideal_4)*1E4;
	residual_y_4= (xpy4_l - yl_ideal_4)*1E4;
	
	// Local errors for rechit
	x_local_error_1 = ePX1*1E4;
	y_local_error_1 = fPX1*1E4;
	
	x_local_error_2 = ePX2*1E4;
	y_local_error_2 = fPX2*1E4;
	
	x_local_error_4 = ePX4*1E4;
	y_local_error_4 = fPX4*1E4;
      
	// Add errors and pulls
	double pulx1=0.;
	double pulx2=0.;
	double pulx4=0.;
	double puly1=0.;
	double puly2=0.;
	double puly4=0.;
	
	if(x_local_error_1!=0.0) pulx1 = residual_x_1/x_local_error_1;
	if(x_local_error_2!=0.0) pulx2 = residual_x_2/x_local_error_2;
	if(x_local_error_4!=0.0) pulx4 = residual_x_4/x_local_error_4;

	if(y_local_error_1!=0.0) puly1 = residual_y_1/y_local_error_1;
	if(y_local_error_2!=0.0) puly2 = residual_y_2/y_local_error_2;
	if(y_local_error_4!=0.0) puly4 = residual_y_4/y_local_error_4;

	// Fill Histograms for FPIX
	if(detTag == "fpix"){
	  
	  if( pt > 0.8) {
            isTriplet = true;
            numberOfTracksCount124++;
	  }
	  if(pt>FPIX_ptcut){

	    hclusprob_fpix ->Fill(clusProb_FPix_phase1);
	    
	    h420f1_124->Fill( residual_x_1 ); 
	    h421f1_124->Fill( residual_y_1 );
	    
	    h420f2_124->Fill( residual_x_2 );
	    h421f2_124->Fill( residual_y_2 );
	    
	    h420f4_124->Fill( residual_x_4 );
	    h421f4_124->Fill( residual_y_4 );
	    
	    // Fill errors
	    h077f1_124->Fill( x_local_error_1 );
	    h078f1_124->Fill( y_local_error_1 );
	    
	    h077f2_124->Fill( x_local_error_2 );
	    h078f2_124->Fill( y_local_error_2 );
	  
	    h077f4_124->Fill( x_local_error_4 );
	    h078f4_124->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079f1_124->Fill( pulx1 );
	    h069f1_124->Fill( puly1 );

	    h079f2_124->Fill( pulx2 );
	    h069f2_124->Fill( puly2 );

	    h079f4_124->Fill( pulx4 );
	    h069f4_124->Fill( puly4 );

	  }
	}
      
	// Fill Histograms for BPIX
	else if(detTag == "bpix"){
	  if(pt>BPIX_ptcut){	  
	    
	    h420b1_124->Fill( residual_x_1 );
	    h421b1_124->Fill( residual_y_1 );

	    h420b2_124->Fill( residual_x_2 );
	    h421b2_124->Fill( residual_y_2 );

	    h420b4_124->Fill( residual_x_4 );
	    h421b4_124->Fill( residual_y_4 );

	    // Fill errors
	    h077b1_124->Fill( x_local_error_1 );
	    h078b1_124->Fill( y_local_error_1 );

	    h077b2_124->Fill( x_local_error_2 );
	    h078b2_124->Fill( y_local_error_2 );

	    h077b4_124->Fill( x_local_error_4 );
	    h078b4_124->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079b1_124->Fill( pulx1 );
	    h069b1_124->Fill( puly1 );

	    h079b2_124->Fill( pulx2 );
	    h069b2_124->Fill( puly2 );

	    h079b4_124->Fill( pulx4 );
	    h069b4_124->Fill( puly4 );
	  }
	}
	else{}

      }//triplet 
      }// three hits: 1-3-4

    if( n1*n3*n4 > 0 ) {

      {// let's open a scope, so we can redefine the variables further down
       
	// create points to feed helix
	std::vector<double> p1 = {xPX1, yPX1, zPX1};
	std::vector<double> p3 = {xPX3, yPX3, zPX3};
	std::vector<double> p4 = {xPX4, yPX4, zPX4};
	
	std::vector<double> intersection1 = {};
	std::vector<double> intersection3 = {};
	std::vector<double> intersection4 = {};
	
	// Create helix from two points and curvature, return the intersection point in local coordinates
	std::vector<double> IntersectionPointLocal_1 = Pixel_FPix_phase1::getIntersection(p3, p4, rho, det1, intersection1); 
	std::vector<double> IntersectionPointLocal_3 = Pixel_FPix_phase1::getIntersection(p1, p4, rho, det3, intersection3);
	std::vector<double> IntersectionPointLocal_4 = Pixel_FPix_phase1::getIntersection(p1, p3, rho, det4, intersection4);
	
	// Intersection point in local coordinates
	xl_ideal_1 = IntersectionPointLocal_1[0];
	yl_ideal_1 = IntersectionPointLocal_1[1];
	
	xl_ideal_3 = IntersectionPointLocal_3[0];
	yl_ideal_3 = IntersectionPointLocal_3[1];
	
	xl_ideal_4 = IntersectionPointLocal_4[0];
	yl_ideal_4 = IntersectionPointLocal_4[1];
	
	// Residuals with rechit and intersection point
	residual_x_1= (xpx1_l - xl_ideal_1)*1E4;
	residual_y_1= (xpy1_l - yl_ideal_1)*1E4;
	
	residual_x_3= (xpx3_l - xl_ideal_3)*1E4;
	residual_y_3= (xpy3_l - yl_ideal_3)*1E4;
	
	residual_x_4= (xpx4_l - xl_ideal_4)*1E4;
	residual_y_4= (xpy4_l - yl_ideal_4)*1E4;
	
	// Local errors for rechit
	x_local_error_1 = ePX1*1E4;
	y_local_error_1 = fPX1*1E4;
	
	x_local_error_3 = ePX3*1E4;
	y_local_error_3 = fPX3*1E4;
	
	x_local_error_4 = ePX4*1E4;
	y_local_error_4 = fPX4*1E4;
      
	// Add errors and pulls
	double pulx1=0.;
	double pulx3=0.;
	double pulx4=0.;
	double puly1=0.;
	double puly3=0.;
	double puly4=0.;
	
	if(x_local_error_1!=0.0) pulx1 = residual_x_1/x_local_error_1;
	if(x_local_error_3!=0.0) pulx3 = residual_x_3/x_local_error_3;
	if(x_local_error_4!=0.0) pulx4 = residual_x_4/x_local_error_4;

	if(y_local_error_1!=0.0) puly1 = residual_y_1/y_local_error_1;
	if(y_local_error_3!=0.0) puly3 = residual_y_3/y_local_error_3;
	if(y_local_error_4!=0.0) puly4 = residual_y_4/y_local_error_4;

	// Fill Histograms for FPIX
	if(detTag == "fpix"){
	  
	  if( pt > 0.8) {
            isTriplet = true;
            numberOfTracksCount134++;
	  }
	  if(pt>FPIX_ptcut){

	    hclusprob_fpix ->Fill(clusProb_FPix_phase1);
	    
	    h420f1_134->Fill( residual_x_1 ); 
	    h421f1_134->Fill( residual_y_1 );
	    
	    h420f3_134->Fill( residual_x_3 );
	    h421f3_134->Fill( residual_y_3 );
	    
	    h420f4_134->Fill( residual_x_4 );
	    h421f4_134->Fill( residual_y_4 );
	    
	    // Fill errors
	    h077f1_134->Fill( x_local_error_1 );
	    h078f1_134->Fill( y_local_error_1 );
	    
	    h077f3_134->Fill( x_local_error_3 );
	    h078f3_134->Fill( y_local_error_3 );
	  
	    h077f4_134->Fill( x_local_error_4 );
	    h078f4_134->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079f1_134->Fill( pulx1 );
	    h069f1_134->Fill( puly1 );

	    h079f3_134->Fill( pulx3 );
	    h069f3_134->Fill( puly3 );

	    h079f4_134->Fill( pulx4 );
	    h069f4_134->Fill( puly4 );

	  }
	}
      
	// Fill Histograms for BPIX
	else if(detTag == "bpix"){
	  if(pt>BPIX_ptcut){	  
	    
	    h420b1_134->Fill( residual_x_1 );
	    h421b1_134->Fill( residual_y_1 );

	    h420b3_134->Fill( residual_x_3 );
	    h421b3_134->Fill( residual_y_3 );

	    h420b4_134->Fill( residual_x_4 );
	    h421b4_134->Fill( residual_y_4 );

	    // Fill errors
	    h077b1_134->Fill( x_local_error_1 );
	    h078b1_134->Fill( y_local_error_1 );

	    h077b3_134->Fill( x_local_error_3 );
	    h078b3_134->Fill( y_local_error_3 );

	    h077b4_134->Fill( x_local_error_4 );
	    h078b4_134->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079b1_134->Fill( pulx1 );
	    h069b1_134->Fill( puly1 );

	    h079b3_134->Fill( pulx3 );
	    h069b3_134->Fill( puly3 );

	    h079b4_134->Fill( pulx4 );
	    h069b4_134->Fill( puly4 );
	  }
	}
	else{}

      }//triplet 
      }// three hits: 1-3-4

    if( n2*n3*n4 > 0 ) {

      {// let's open a scope, so we can redefine the variables further down
       
	// create points to feed helix
	std::vector<double> p2 = {xPX2, yPX2, zPX2};
	std::vector<double> p3 = {xPX3, yPX3, zPX3};
	std::vector<double> p4 = {xPX4, yPX4, zPX4};
	
	std::vector<double> intersection2 = {};
	std::vector<double> intersection3 = {};
	std::vector<double> intersection4 = {};
	
	// Create helix from two points and curvature, return the intersection point in local coordinates
	std::vector<double> IntersectionPointLocal_2 = Pixel_FPix_phase1::getIntersection(p3, p4, rho, det2, intersection2); 
	std::vector<double> IntersectionPointLocal_3 = Pixel_FPix_phase1::getIntersection(p2, p4, rho, det3, intersection3);
	std::vector<double> IntersectionPointLocal_4 = Pixel_FPix_phase1::getIntersection(p2, p3, rho, det4, intersection4);

	if(n3_r1>0){
	  std::vector<double> p3_r1 = {xPX3_r1, yPX3_r1, zPX3_r1};
	  std::vector<double> intersection3_r1 = {};	
	  std::vector<double> IntersectionPointLocal_3_r1 = Pixel_FPix_phase1::getIntersection(p2, p4, rho, det3_r1, intersection3_r1);
	  xl_ideal_3_r1 = IntersectionPointLocal_3_r1[0];
	  yl_ideal_3_r1 = IntersectionPointLocal_3_r1[1];
	  residual_x_3_r1= (xpx3_l_r1 - xl_ideal_3_r1)*1E4;
	  residual_y_3_r1= (xpy3_l_r1 - yl_ideal_3_r1)*1E4;
	}

	if(n4_r1>0){
	  std::vector<double> p4_r1 = {xPX4_r1, yPX4_r1, zPX4_r1};
	  std::vector<double> intersection4_r1 = {};
	  std::vector<double> IntersectionPointLocal_4_r1 = Pixel_FPix_phase1::getIntersection(p2, p3, rho, det4_r1, intersection4_r1);
	  xl_ideal_4_r1 = IntersectionPointLocal_4_r1[0];
	  yl_ideal_4_r1 = IntersectionPointLocal_4_r1[1];
	  residual_x_4_r1= (xpx4_l_r1 - xl_ideal_4_r1)*1E4;
	  residual_y_4_r1= (xpy4_l_r1 - yl_ideal_4_r1)*1E4;
	}


	if(n3_r2>0){
	  std::vector<double> p3_r2 = {xPX3_r2, yPX3_r2, zPX3_r2};
	  std::vector<double> intersection3_r2 = {};	
	  std::vector<double> IntersectionPointLocal_3_r2 = Pixel_FPix_phase1::getIntersection(p2, p4, rho, det3_r2, intersection3_r2);
	  xl_ideal_3_r2 = IntersectionPointLocal_3_r2[0];
	  yl_ideal_3_r2 = IntersectionPointLocal_3_r2[1];
	  residual_x_3_r2= (xpx3_l_r2 - xl_ideal_3_r2)*1E4;
	  residual_y_3_r2= (xpy3_l_r2 - yl_ideal_3_r2)*1E4;
	}

	if(n4_r2>0){
	  std::vector<double> p4_r2 = {xPX4_r2, yPX4_r2, zPX4_r2};
	  std::vector<double> intersection4_r2 = {};
	  std::vector<double> IntersectionPointLocal_4_r2 = Pixel_FPix_phase1::getIntersection(p2, p3, rho, det4_r2, intersection4_r2);
	  xl_ideal_4_r2 = IntersectionPointLocal_4_r2[0];
	  yl_ideal_4_r2 = IntersectionPointLocal_4_r2[1];
	  residual_x_4_r2= (xpx4_l_r2 - xl_ideal_4_r2)*1E4;
	  residual_y_4_r2= (xpy4_l_r2 - yl_ideal_4_r2)*1E4;
	}

	// Intersection point in local coordinates
	xl_ideal_2 = IntersectionPointLocal_2[0];
	yl_ideal_2 = IntersectionPointLocal_2[1];
	
	xl_ideal_3 = IntersectionPointLocal_3[0];
	yl_ideal_3 = IntersectionPointLocal_3[1];
	
	xl_ideal_4 = IntersectionPointLocal_4[0];
	yl_ideal_4 = IntersectionPointLocal_4[1];
	
	// Residuals with rechit and intersection point
	residual_x_2= (xpx2_l - xl_ideal_2)*1E4;
	residual_y_2= (xpy2_l - yl_ideal_2)*1E4;
	
	residual_x_3= (xpx3_l - xl_ideal_3)*1E4;
	residual_y_3= (xpy3_l - yl_ideal_3)*1E4;
	
	residual_x_4= (xpx4_l - xl_ideal_4)*1E4;
	residual_y_4= (xpy4_l - yl_ideal_4)*1E4;
	
	// Local errors for rechit
	x_local_error_2 = ePX2*1E4;
	y_local_error_2 = fPX2*1E4;
	
	x_local_error_3 = ePX3*1E4;
	y_local_error_3 = fPX3*1E4;
	
	x_local_error_4 = ePX4*1E4;
	y_local_error_4 = fPX4*1E4;
      
	// Add errors and pulls
	double pulx2=0.;
	double pulx3=0.;
	double pulx4=0.;
	double puly2=0.;
	double puly3=0.;
	double puly4=0.;
	
	if(x_local_error_2!=0.0) pulx2 = residual_x_2/x_local_error_2;
	if(x_local_error_3!=0.0) pulx3 = residual_x_3/x_local_error_3;
	if(x_local_error_4!=0.0) pulx4 = residual_x_4/x_local_error_4;

	if(y_local_error_2!=0.0) puly2 = residual_y_2/y_local_error_2;
	if(y_local_error_3!=0.0) puly3 = residual_y_3/y_local_error_3;
	if(y_local_error_4!=0.0) puly4 = residual_y_4/y_local_error_4;

	// Fill Histograms for FPIX
	if(detTag == "fpix"){
	  
	  if( refitTrajectoryCollection.size() > 0 ){
            const Trajectory& refitTrajectory = refitTrajectoryCollection.front();
            pt_res_refit = refitTrajectory.geometricalInnermostState().globalMomentum().perp();
          }
          else{
            pt_res_refit = -9999.;
          }

	  pt_res = pt;
          cluster_size_res = cluster_size_res_init;
          hits_track = hp.numberOfValidTrackerHits();
          hits_barrel = hp.numberOfValidPixelBarrelHits();
          hits_endcap = hp.numberOfValidPixelEndcapHits();
          ls_with_measure = hp.pixelBarrelLayersWithMeasurement();
          ds_with_measure = hp.pixelEndcapLayersWithMeasurement();
	  pt_res = pt;
	  hits_barrel = hp.numberOfValidPixelBarrelHits();
	  hits_track  = hp.numberOfValidTrackerHits();
	  ls_with_measure = hp.trackerLayersWithMeasurement();
	  //runNumber_res = iEvent.run();
	  //lumiBlock_res = iEvent.luminosityBlock();

	  if( pt > 0.8) {
            isTriplet = true;
            numberOfTracksCount234++;
          }
	  if(pt>FPIX_ptcut){

	    dx_res_2 = residual_x_3;
	    dz_res_2 = residual_y_3;
	    
	    dx_res_3 = residual_x_4;
	    dz_res_3 = residual_y_4;

	    //cout << residual_x_3 << endl;
	    hclusprob_fpix ->Fill(clusProb_FPix_phase1);
	    
	    h420f3_234_eta->Fill(iTrack->eta(), abs(residual_x_3));
	    h421f3_234_eta->Fill(iTrack->eta(), abs(residual_y_3));

	    h420f4_234_eta->Fill(iTrack->eta(), abs(residual_x_4));
	    h421f4_234_eta->Fill(iTrack->eta(), abs(residual_y_4));

	    h420f2_234->Fill( residual_x_2 ); 
	    h421f2_234->Fill( residual_y_2 );
	    
	    h420f3_234->Fill( residual_x_3 );
	    h421f3_234->Fill( residual_y_3 );
	    
	    h420f4_234->Fill( residual_x_4 );
	    h421f4_234->Fill( residual_y_4 );

	    if(n3_r1>0){
	      h420f3_234_r1->Fill( residual_x_3_r1 );
	      h421f3_234_r1->Fill( residual_y_3_r1 );
	    }
	    
	    if(n4_r1>0){
	      h420f4_234_r1->Fill( residual_x_4_r1 );
	      h421f4_234_r1->Fill( residual_y_4_r1 );
	    }

	    if(n3_r2>0){
	      h420f3_234_r2->Fill( residual_x_3_r2 );
	      h421f3_234_r2->Fill( residual_y_3_r2 );
	    }

	    if(n4_r2>0){
	      h420f4_234_r2->Fill( residual_x_4_r2 );
	      h421f4_234_r2->Fill( residual_y_4_r2 );
	    }

	    // Fill errors
	    h077f2_234->Fill( x_local_error_2 );
	    h078f2_234->Fill( y_local_error_2 );
	    
	    h077f3_234->Fill( x_local_error_3 );
	    h078f3_234->Fill( y_local_error_3 );
	  
	    h077f4_234->Fill( x_local_error_4 );
	    h078f4_234->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079f2_234->Fill( pulx2 );
	    h069f2_234->Fill( puly2 );

	    h079f3_234->Fill( pulx3 );
	    h069f3_234->Fill( puly3 );

	    h079f4_234->Fill( pulx4 );
	    h069f4_234->Fill( puly4 );

	  }
	}
      
	// Fill Histograms for BPIX
	else if(detTag == "bpix"){
	  if(pt>BPIX_ptcut){	  
	    
	    h420b2_234->Fill( residual_x_2 );
	    h421b2_234->Fill( residual_y_2 );

	    h420b3_234->Fill( residual_x_3 );
	    h421b3_234->Fill( residual_y_3 );

	    h420b4_234->Fill( residual_x_4 );
	    h421b4_234->Fill( residual_y_4 );

	    // Fill errors
	    h077b2_234->Fill( x_local_error_2 );
	    h078b2_234->Fill( y_local_error_2 );

	    h077b3_234->Fill( x_local_error_3 );
	    h078b3_234->Fill( y_local_error_3 );

	    h077b4_234->Fill( x_local_error_4 );
	    h078b4_234->Fill( y_local_error_4 );
	    
	    // Fill pulls
	    h079b2_234->Fill( pulx2 );
	    h069b2_234->Fill( puly2 );

	    h079b3_234->Fill( pulx3 );
	    h069b3_234->Fill( puly3 );

	    h079b4_234->Fill( pulx4 );
	    h069b4_234->Fill( puly4 );
	  }
	}
	else{}

      }//triplet 
      }// three hits: 2-3-4

    if(isTriplet) { 
      dx_resolution_study_1.push_back(dx_res_1);
      dz_resolution_study_1.push_back(dz_res_1);
      dx_resolution_study_2.push_back(dx_res_2);
      dz_resolution_study_2.push_back(dz_res_2);
      dx_resolution_study_3.push_back(dx_res_3);
      dz_resolution_study_3.push_back(dz_res_3);
      pt_resolution_study.push_back(pt_res);
      pt_resolution_study_refit.push_back(pt_res_refit);
      cluster_size_resolution_study.push_back(cluster_size_res);
      hits_on_track_barrel.push_back(hits_barrel);
      hits_on_track_barrel.push_back(hits_endcap);
      hits_on_track_tracker.push_back(hits_track);
      layers_with_measurement.push_back(ls_with_measure);
      disks_with_measurement.push_back(ds_with_measure);
    }

  }// loop over tracks
  tree->Fill();


}//event
//----------------------------------------------------------------------
// method called just after ending the event loop:
//
void Pixel_FPix_phase1::endJob() {
  if (Template){
    std::cout << "end of job after " << Counter_Template::neve << " events.\n";
  };
  if (Generic){
    std::cout << "end of job after " << Counter_Generic::neve << " events.\n";
  };
}

//define this as a plug-in
DEFINE_FWK_MODULE(Pixel_FPix_phase1);

