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

#include "DataFormats/TrackerRecHit2D/interface/SiStripMatchedRecHit2DCollection.h"
#include "DataFormats/TrackerCommon/interface/TrackerTopology.h"

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
  edm::EDGetTokenT<SiStripMatchedRecHit2DCollection> siMatchHitCollectionToken_;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> theGToken_;
  edm::ESGetToken<TrackerTopology, TrackerTopologyRcd> theTopoToken_;
  TH2D *recHitsXY;
  TH2D *recHitsRZ;
};

RecHitPositionAnalyzer::RecHitPositionAnalyzer(const edm::ParameterSet& iConfig){
  edm::Service<TFileService> fs;
  // Input tag for RecHit collection
  recHitCollectionToken_ = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollection"));
  siMatchHitCollectionToken_ = consumes<SiStripMatchedRecHit2DCollection>(iConfig.getParameter<edm::InputTag>("siMatchHitCollection"));
  
  theGToken_ = esConsumes();
  theTopoToken_ = esConsumes();
  recHitsXY = fs->make<TH2D>("recHitsXY", "X vs Y Position of the RecHits", 3000, -150.0, 150.0, 3000, -150.0, 150.0);
  recHitsRZ = fs->make<TH2D>("recHitsRZ", "R vs Z Position of the RecHits", 6000, -300.0, 300.0, 1200, 0.0, 120.0);
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

  // extract tracker topology
  edm::ESHandle<TrackerTopology> theTopo = iSetup.getHandle(theTopoToken_);

  // Loop over Tracking RecHits and print their positions
  for (const auto& recHit : *recHits) {
    if(recHit.isValid()){
      DetId thedetid = recHit.rawId();
      auto subdetId = thedetid.subdetId();
      // auto GeomDet = theG->idToDet(recHit.geographicalId());
      // auto SubDet = GeomDet->subDetector();
      // std::cout << "\t\t subdetId: " << subdetId
      //               << " SubDet: " << SubDet
      //               << " layer: " << theTopo->layer(thedetid)
      //               << " tidRing: " << theTopo->tidRing(thedetid)
      //               << " tecRing: " << theTopo->tecRing(thedetid)
      //               << " isDoubleSens: " << theTopo->isDoubleSens(thedetid)
      //               << " tibLayer: " << theTopo->tibLayer(thedetid)
      //               << " tibIsStereo: " << theTopo->tibIsStereo(thedetid)
      //               << " isStereo: " << theTopo->isStereo(thedetid)
      //               << " tibIsDoubleSide?: " << theTopo->tibIsDoubleSide(thedetid)
      //               << " tidIsDoubleSide?: " << theTopo->tidIsDoubleSide(thedetid)
      //               << " tobIsDoubleSide?: " << theTopo->tobIsDoubleSide(thedetid)
      //               << " tecIsDoubleSide?: " << theTopo->tecIsDoubleSide(thedetid)
      //               << std::endl;
      // if(theTopo->isStereo(thedetid)==1) continue;

      // The tibIsDoubleSide etc should have worked but did not. Doing it manually:
      // subdetId 3=TIB, 4=TID, 5=TOB, 6=TEC
      if((subdetId==3 && (theTopo->layer(thedetid)==1 || theTopo->layer(thedetid)==2)) ||
          (subdetId==4 && (theTopo->tidRing(thedetid)==1 || theTopo->tidRing(thedetid)==2)) ||
          (subdetId==5 && (theTopo->layer(thedetid)==1 || theTopo->layer(thedetid)==2)) ||
          (subdetId==6 && (theTopo->tecRing(thedetid)==1 || theTopo->tecRing(thedetid)==2 || theTopo->tecRing(thedetid)==5))
        ) continue;

      const auto& gPosition = theG->idToDet((recHit).geographicalId())->surface().toGlobal((recHit).localPosition());
      recHitsXY->Fill(gPosition.x(),gPosition.y());
      recHitsRZ->Fill(gPosition.z(),gPosition.perp());
      // std::cout << "\t\t\tRecHit in GP x y z: " << gPosition.x() << " " << gPosition.y() << " " << gPosition.z() << std::endl;
    }
  }

  // Get the SiStripRecHitConverter collection
  edm::Handle<SiStripMatchedRecHit2DCollection> siMatchHits;
  iEvent.getByToken(siMatchHitCollectionToken_, siMatchHits);
  const SiStripMatchedRecHit2DCollection::DataContainer& stripcollStm = siMatchHits->data();
  SiStripMatchedRecHit2DCollection::DataContainer::const_iterator istripStm;

  for (istripStm = stripcollStm.begin(); istripStm != stripcollStm.end(); ++istripStm) {
    const SiStripMatchedRecHit2D* recHit = &*(istripStm);
    if(recHit->isValid()){
      const auto& gPosition = theG->idToDet(recHit->geographicalId())->surface().toGlobal(recHit->localPosition());
      recHitsXY->Fill(gPosition.x(),gPosition.y());
      recHitsRZ->Fill(gPosition.z(),gPosition.perp());
      // std::cout << "\t\t\tRecHit in GP x y z: " << gPosition.x() << " " << gPosition.y() << " " << gPosition.z() << std::endl;
    }
  }

}

void RecHitPositionAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {}
void RecHitPositionAnalyzer::endRun(const edm::Run& iRun, const edm::EventSetup& ) {}
void RecHitPositionAnalyzer::beginJob(const edm::EventSetup&) {}     
void RecHitPositionAnalyzer::endJob() {}
void RecHitPositionAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {}

DEFINE_FWK_MODULE(RecHitPositionAnalyzer);
