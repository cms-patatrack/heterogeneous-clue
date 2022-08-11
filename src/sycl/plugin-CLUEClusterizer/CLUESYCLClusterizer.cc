#include <CL/sycl.hpp>

#include "SYCLCore/Product.h"
#include "Framework/EventSetup.h"
#include "Framework/Event.h"
#include "Framework/PluginFactory.h"
#include "Framework/EDProducer.h"
#include "SYCLCore/ScopedContext.h"

#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "SYCLDataFormats/ClusteredPointsCloudSYCL.h"
#include "CLUEAlgoSYCL.h"

class CLUESYCLClusterizer : public edm::EDProducer {
public:
  explicit CLUESYCLClusterizer(edm::ProductRegistry& reg);
  ~CLUESYCLClusterizer() override = default;

private:
  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  edm::EDGetTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenPointsCloudSYCL_;
  edm::EDPutTokenT<cms::sycltools::Product<ClusteredPointsCloudSYCL>> tokenClusters_;
};

CLUESYCLClusterizer::CLUESYCLClusterizer(edm::ProductRegistry& reg)
    : tokenPointsCloudSYCL_{reg.consumes<cms::sycltools::Product<PointsCloudSYCL>>()},
      tokenClusters_{reg.produces<cms::sycltools::Product<ClusteredPointsCloudSYCL>>()} {}

void CLUESYCLClusterizer::produce(edm::Event& event, const edm::EventSetup& eventSetup) {
  auto const& pcProduct = event.get(tokenPointsCloudSYCL_);
  cms::sycltools::ScopedContextProduce ctx(pcProduct);
  PointsCloudSYCL const& pc = ctx.get(pcProduct);
  auto stream = ctx.stream();
  float dc = 20;
  float rhoc = 25;
  float outlierDeltaFactor = 2;
  CLUEAlgoSYCL clueAlgo(dc, rhoc, outlierDeltaFactor, stream);
  auto data = clueAlgo.makeClusters(pc);

  ClusteredPointsCloudSYCL cpc(pc);

  ctx.emplace(event, tokenClusters_, std::move(cpc));
}

DEFINE_FWK_MODULE(CLUESYCLClusterizer);