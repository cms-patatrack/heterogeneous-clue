#include <iostream>

#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"

#include "SYCLCore/Product.h"
#include "SYCLCore/ScopedContext.h"

#include "DataFormats/PointsCloud.h"

class PointsCloudProducer : public edm::EDProducer {
public:
  explicit PointsCloudProducer(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;

  edm::EDPutTokenT<cms::sycltools::Product<PointsCloud>> pcPutToken_;
};

PointsCloudProducer::PointsCloudProducer(edm::ProductRegistry& reg)
    : pcPutToken_(reg.produces<cms::sycltools::Product<PointsCloud>>()) {}

void PointsCloudProducer::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  cms::sycltools::ScopedContextProduce ctx(event.streamID());
  auto pcHost = eventSetup.get<PointsCloud>();
  ctx.emplace(event, pcPutToken_, std::move(pcHost));
}

DEFINE_FWK_MODULE(PointsCloudProducer);
