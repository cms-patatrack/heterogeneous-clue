#include "Framework/EventSetup.h"
#include "Framework/Event.h"
#include "Framework/PluginFactory.h"
#include "Framework/EDProducer.h"

#include "AlpakaCore/ScopedContext.h"
#include "AlpakaCore/Product.h"

#include "DataFormats/ClusterCollection.h"
#include "AlpakaDataFormats/alpaka/ClusterCollectionAlpaka.h"
#include "CLUE3DAlgoAlpaka.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class CLUEAlpakaTracksterizer : public edm::EDProducer {
  public:
    explicit CLUEAlpakaTracksterizer(edm::ProductRegistry& reg);
    ~CLUEAlpakaTracksterizer() override = default;

  private:
    void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

    edm::EDGetTokenT<ClusterCollection> clusterCollectionToken_;
    edm::EDPutTokenT<cms::alpakatools::Product<Queue, ClusterCollectionAlpaka>> tracksterToken_;

    // CLUE3DAlgoAlpaka* algo_;
  };

  CLUEAlpakaTracksterizer::CLUEAlpakaTracksterizer(edm::ProductRegistry& reg)
      : clusterCollectionToken_{reg.consumes<ClusterCollection>()},
        tracksterToken_{reg.produces<cms::alpakatools::Product<Queue, ClusterCollectionAlpaka>>()} {}
  // {    algo_ = new CLUE3DAlgoAlpaka;
  //   }

  void CLUEAlpakaTracksterizer::produce(edm::Event& event, const edm::EventSetup& eventSetup) {
    auto const& pc = event.get(clusterCollectionToken_);
    cms::alpakatools::ScopedContextProduce<Queue> ctx(event.streamID());
    auto stream = ctx.stream();
    // stream to be passed to makeTracksters?
    CLUE3DAlgoAlpaka algo_(stream);
    // algo_->makeTracksters(pc);
    algo_.makeTracksters(pc);

    // ctx.emplace(event, tracksterToken_, std::move(algo_->d_clusters));
    ctx.emplace(event, tracksterToken_, std::move(algo_.d_clusters));
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

DEFINE_FWK_ALPAKA_MODULE(CLUEAlpakaTracksterizer);
