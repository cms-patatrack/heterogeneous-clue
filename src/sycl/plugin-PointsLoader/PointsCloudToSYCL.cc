#include <iostream>

#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"

#include "SYCLCore/Product.h"
#include "SYCLCore/ScopedContext.h"
#include "SYCLCore/device_unique_ptr.h"

#include "DataFormats/PointsCloud.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"

class PointsCloudToSYCL : public edm::EDProducer {
public:
  explicit PointsCloudToSYCL(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;

  edm::EDPutTokenT<cms::sycltools::Product<PointsCloudSYCL>> pcPutToken_;
};

PointsCloudToSYCL::PointsCloudToSYCL(edm::ProductRegistry& reg)
    : pcPutToken_(reg.produces<cms::sycltools::Product<PointsCloudSYCL>>()) {}

void PointsCloudToSYCL::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  cms::sycltools::ScopedContextProduce ctx(event.streamID());
  auto pcHost = std::make_unique<PointsCloud>();
  *pcHost = eventSetup.get<PointsCloud>();
  sycl::queue stream = ctx.stream();
  PointsCloudSYCL pcDevice(stream, pcHost->n);
  stream.memcpy(pcDevice.x.get(), pcHost->x.data(), sizeof(float) * pcHost->n);
  stream.memcpy(pcDevice.y.get(), pcHost->y.data(), sizeof(float) * pcHost->n);
  stream.memcpy(pcDevice.layer.get(), pcHost->layer.data(), sizeof(int) * pcHost->n);
  stream.memcpy(pcDevice.weight.get(), pcHost->weight.data(), sizeof(float) * pcHost->n);
  stream.memset(pcDevice.rho.get(), 0x00, sizeof(float) * pcHost->n);
  stream.memset(pcDevice.delta.get(), 0x00, sizeof(float) * pcHost->n);
  stream.memset(pcDevice.nearestHigher.get(), 0x00, sizeof(int) * pcHost->n);
  stream.memset(pcDevice.clusterIndex.get(), 0x00, sizeof(int) * pcHost->n);
  stream.memset(pcDevice.isSeed.get(), 0x00, sizeof(int) * pcHost->n).wait();

  ctx.emplace(event, pcPutToken_, std::move(pcDevice));
}

DEFINE_FWK_MODULE(PointsCloudToSYCL);
