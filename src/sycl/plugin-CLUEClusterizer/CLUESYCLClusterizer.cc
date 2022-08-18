#include <CL/sycl.hpp>

#include "Framework/EventSetup.h"
#include "Framework/Event.h"
#include "Framework/PluginFactory.h"
#include "Framework/EDProducer.h"

#include "SYCLCore/ScopedContext.h"
#include "SYCLCore/Product.h"

#include "DataFormats/PointsCloud.h"
#include "DataFormats/CLUE_config.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "CLUEAlgoSYCL.h"

class CLUESYCLClusterizer : public edm::EDProducer {
public:
  explicit CLUESYCLClusterizer(edm::ProductRegistry& reg);
  ~CLUESYCLClusterizer() override = default;

private:
  void produce(edm::Event& iEvent, const edm::EventSetup& iSetup) override;

  edm::EDGetTokenT<cms::sycltools::Product<PointsCloud>> tokenPointsCloudSYCL_;
  edm::EDPutTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenClusters_;
};

CLUESYCLClusterizer::CLUESYCLClusterizer(edm::ProductRegistry& reg)
    : tokenPointsCloudSYCL_{reg.consumes<cms::sycltools::Product<PointsCloud>>()},
      tokenClusters_{reg.produces<cms::sycltools::Product<PointsCloudSYCL>>()} {}

void CLUESYCLClusterizer::produce(edm::Event& event, const edm::EventSetup& eventSetup) {
  auto const& pcProduct = event.get(tokenPointsCloudSYCL_);
  cms::sycltools::ScopedContextProduce ctx(pcProduct);
  PointsCloud const& pc = ctx.get(pcProduct);
  Parameters const& par = eventSetup.get<Parameters>();
  auto stream = ctx.stream();
  CLUEAlgoSYCL clueAlgo(par.dc, par.rhoc, par.outlierDeltaFactor, stream, pc.n);
  clueAlgo.makeClusters(pc);

  ctx.emplace(event, tokenClusters_, std::move(clueAlgo.d_points));
}

DEFINE_FWK_MODULE(CLUESYCLClusterizer);