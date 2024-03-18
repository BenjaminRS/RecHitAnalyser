#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"

// RecoTracker/​TrackProducer/​test/​TrackAnalyzer.cc:
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH2.h"
#include "TFile.h"

class RecHitPositionAnalyzer : public edm::one::EDAnalyzer<> {
public:
  explicit RecHitPositionAnalyzer(const edm::ParameterSet&);
  ~RecHitPositionAnalyzer() override;
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
private:
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void beginRun(const edm::Run&, const edm::EventSetup&);
  void endRun(const edm::Run&, const edm::EventSetup&);
  virtual void beginJob(const edm::EventSetup&) ;
  virtual void endJob() ;
  // InputTag for RecHit collection
  edm::EDGetTokenT<TrackingRecHitCollection> recHitCollectionToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> theGToken_;
  TH2D *recHitsXY;
};

RecHitPositionAnalyzer::RecHitPositionAnalyzer(const edm::ParameterSet& iConfig){
  edm::Service<TFileService> fs;
  // Input tag for RecHit collection
  recHitCollectionToken_ = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollection"));
  theGToken_ = esConsumes();
  recHitsXY = fs->make<TH2D>("recHitsXY", "X vs Y Position of the RecHits", 3000, -150.0, 150.0, 3000, -150.0, 150.0);
}

RecHitPositionAnalyzer::~RecHitPositionAnalyzer(){
    // delete theFile;
}

void RecHitPositionAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  // Get the RecHit collection
  edm::Handle<TrackingRecHitCollection> recHits;
  iEvent.getByToken(recHitCollectionToken_, recHits);
  // std::cout << "\trecHits->size(): " << recHits->size() << std::endl;

  // extract tracker geometry
  edm::ESHandle<TrackerGeometry> theG = iSetup.getHandle(theGToken_);

  // Loop over Tracking RecHits and print their positions
  for (const auto& recHit : *recHits) {
    if(recHit.isValid()){
      const auto& gPosition = theG->idToDet((recHit).geographicalId())->surface().toGlobal((recHit).localPosition());
      recHitsXY->Fill(gPosition.x(),gPosition.y());
    }
  }

}

void RecHitPositionAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {}
void RecHitPositionAnalyzer::endRun(const edm::Run& iRun, const edm::EventSetup& ) {}
void RecHitPositionAnalyzer::beginJob(const edm::EventSetup&) {}     
void RecHitPositionAnalyzer::endJob() {}
void RecHitPositionAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {}

DEFINE_FWK_MODULE(RecHitPositionAnalyzer);
