#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"

#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"

// SWGuideTransientRecHitsUsage:
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

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
  edm::ESGetToken<TransientTrackingRecHitBuilder, TransientRecHitRecord> m_builderToken;
  const TransientTrackingRecHitBuilder* m_builder = nullptr;
  // edm::EDGetTokenT<TrackingRecHitCollection> theTrackerRecHitBuilderName;

  edm::EDGetTokenT<reco::TrackCollection> m_trkcollToken;
  edm::ESGetToken<TrackerGeometry, TrackerDigiGeometryRecord> theGToken_;
  // TFile *theFile;
  TH2D *recHitsXY;
};

RecHitPositionAnalyzer::RecHitPositionAnalyzer(const edm::ParameterSet& iConfig){
  edm::Service<TFileService> fs;
  // Input tag for RecHit collection
  recHitCollectionToken_ = consumes<TrackingRecHitCollection>(iConfig.getParameter<edm::InputTag>("recHitCollection"));
  m_builderToken = esConsumes<edm::Transition::BeginRun>(edm::ESInputTag{"", iConfig.getParameter<std::string>("TTRHBuilder")});
  m_trkcollToken = consumes<reco::TrackCollection>(iConfig.getParameter<edm::InputTag>("trackCollection"));
  theGToken_ = esConsumes();
  // theTrackerRecHitBuilderName = consumes<TrackingRecHitCollection>(iConfig.getParameter<std::string>("TrackerRecHitBuilder"));
  // theFile = new TFile("recHits.root", "RECREATE");
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

  // Get the Track collection
  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByToken(m_trkcollToken, tracks);
  // std::cout << "\ttracks->size(): " << tracks->size() << std::endl;

  // extract tracker geometry
  edm::ESHandle<TrackerGeometry> theG = iSetup.getHandle(theGToken_);

// // From https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideTransientRecHitsUsage 
// // But this really doesnt seem to work: code is too old e.g. no matching function for call to iSetup.get
//   edm::Handle<reco::TrackCollection> tracks;
//   iEvent.getByLabel("TrackRefitter", tracks);
//   // edm::ESHandle<TransientTrackingRecHitBuilder> theTrackerRecHitBuilder;
//   // iSetup.get<TransientRecHitRecord>().get(theTrackerRecHitBuilderName,theTrackerRecHitBuilder);

//   // edm::Handle<TrackingRecHitCollection> recHits;
//   // iEvent.getByToken(recHitCollectionToken_, recHits);

// uint maxTracks = 5; //just 5 tracks for debugging purposes
// uint counter = 0;

// E.g. https://github.com/cms-sw/cmssw/blob/master/DPGAnalysis/SiStripTools/plugins/DuplicateRecHits.cc 
  for(reco::TrackCollection::const_iterator itTrack = tracks->begin(); itTrack != tracks->end(); ++itTrack) {
    // if (counter>maxTracks) break;
    // ++counter;
    // std::cout << "\t\trecHitsSize(): " << itTrack->recHitsSize() << std::endl;
    for (trackingRecHit_iterator rh = itTrack->recHitsBegin(); rh != itTrack->recHitsEnd(); ++rh) {

    // const reco::HitPattern& p = itTrack->hitPattern();
    // for (int i=0; i<p.numberOfAllHits(); i++) { //p.numberOfAllHits() doesn't exist anymore
      // TrackingRecHitRef rhit = itTrack->recHit(i);
      if ((*rh)->isValid()) {
        // std::cout << "\t\t\tRecHit on det " << (*rh)->geographicalId().rawId() << std::endl;
        // std::cout << "\t\t\tRecHit in LP " << (*rh)->localPosition() << std::endl;
        const auto& gPosition = theG->idToDet((*rh)->geographicalId())->surface().toGlobal((*rh)->localPosition());
        // std::cout << "\t\t\tRecHit in GP " << gPosition << std::endl;
        // std::cout << "\t\t\tRecHit in GP x y z: " << gPosition.x() << " " << gPosition.y() << " " << gPosition.z() << std::endl;
        recHitsXY->Fill(gPosition.x(),gPosition.y());
        // // TransientTrackingRecHit::RecHitPointer tthit = theTrackerRecHitBuilder->build(&*rhit);
        // TransientTrackingRecHit::RecHitPointer tthit = m_builder->build(&**rh);
        // GlobalPoint gPosition =  tthit->globalPosition();
        // std::cout << "Hit found with global position = " << gPosition << std::endl;
        }
      //   else {
      //        std::cout << "\t\t\t Invalid Hit On " << (*rh)->geographicalId().rawId() << std::endl;
      // }
    }
  }
  


  // Loop over Tracking RecHits and print their positions
  // for (const auto& recHit : *recHits) {
  //   std::cout << "\trecHit.isValid()?: " << recHit.isValid() << std::endl;
  //   if(recHit.isValid()){
  //     TransientTrackingRecHit::RecHitPointer ttrh = m_builder->build(&recHit);
  //     const SiPixelRecHit* pxrh = dynamic_cast<const SiPixelRecHit*>(ttrh->hit());

  //     if (pxrh) {
  //         std::cout << "\tpxrh exists" << std::endl;
  //         // LogTrace("DuplicateHitFinder") << ttrh->det()->geographicalId() << " " << pxrh->cluster().index();
  //         std::stringstream detidstr;
  //         detidstr << ttrh->det()->geographicalId().rawId();
  //         std::cout << "\tsbFound " << ttrh->det()->geographicalId().rawId() << " "
  //                   << pxrh->cluster().index()
  //                   << std::endl;
  //     }

  //     const auto& position = recHit.localPosition();
  //     std::cout << "\tRecHit hasPositionAndError?: " << recHit.hasPositionAndError() << std::endl;
  //     // const auto& position = recHit.globalPosition(); //This causes a segfault..
  //     std::cout << "\tRecHit Position - X: " << position.x() << ", Y: " << position.y() << ", Z: " << position.z() << std::endl;
  //   }
  // }
}

void RecHitPositionAnalyzer::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup) {
  m_builder = &iSetup.getData(m_builderToken);
  // iSetup.get<TransientRecHitRecord>().get(theTrackerRecHitBuilderName,theTrackerRecHitBuilder);
}

void RecHitPositionAnalyzer::endRun(const edm::Run& iRun, const edm::EventSetup& ) {}
void RecHitPositionAnalyzer::beginJob(const edm::EventSetup&) {}     
void RecHitPositionAnalyzer::endJob() {
      // theFile->Write();
}
void RecHitPositionAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {}

DEFINE_FWK_MODULE(RecHitPositionAnalyzer);
