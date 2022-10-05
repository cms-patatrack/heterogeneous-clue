#include "Framework/EventSetup.h"
#include "Framework/Event.h"
#include "Framework/PluginFactory.h"
#include "Framework/EDProducer.h"

#include "DataFormats/ClusterCollection.h"
#include "CLUE3DAlgoSerial.h"
#include <optional>

class CLUESerialTracksterizer : public edm::EDProducer {
public:
  explicit CLUESerialTracksterizer(edm::ProductRegistry& reg);
  ~CLUESerialTracksterizer() override = default;

private:
  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  edm::EDGetTokenT<ClusterCollection> clusterCollectionToken_;
  edm::EDPutTokenT<ClusterCollectionSerialOnLayers> tracksterToken_;

  std::unique_ptr<CLUE3DAlgoSerial> algo_;
};

CLUESerialTracksterizer::CLUESerialTracksterizer(edm::ProductRegistry& reg)
    : clusterCollectionToken_{reg.consumes<ClusterCollection>()},
      tracksterToken_{reg.produces<ClusterCollectionSerialOnLayers>()},
      algo_{std::make_unique<CLUE3DAlgoSerial>()} {}

void CLUESerialTracksterizer::produce(edm::Event& event, const edm::EventSetup& eventSetup) {
  auto const& pc = event.get(clusterCollectionToken_);

  algo_->makeTracksters(pc);

  event.emplace(tracksterToken_, std::move(algo_->d_clusters));
}

DEFINE_FWK_MODULE(CLUESerialTracksterizer);