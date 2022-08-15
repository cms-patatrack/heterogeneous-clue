#include <iostream>

#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"

#include "SYCLCore/Product.h"
#include "SYCLCore/ScopedContext.h"

#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "ValidatorData.h"

class ValidatorPointsCloudToSYCL : public edm::EDProducer {
public:
  explicit ValidatorPointsCloudToSYCL(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
  template <class T>
  bool arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n, sycl::queue stream);
  edm::EDGetTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenPC_;
};

ValidatorPointsCloudToSYCL::ValidatorPointsCloudToSYCL(edm::ProductRegistry& reg)
    : tokenPC_(reg.consumes<cms::sycltools::Product<PointsCloudSYCL>>()) {}

template <class T>
bool ValidatorPointsCloudToSYCL::arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n, sycl::queue stream) {
  bool sameValue = true;

  T* host = new T[n];
  stream.memcpy(host, devicePtr, sizeof(T) * n).wait();

  for (int i = 0; i < n; i++) {
    if (host[i] != trueDataArr[i]) {
      sameValue = false;
      break;
    }
  }

  delete[] host;

  return sameValue;
}

void ValidatorPointsCloudToSYCL::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  auto pcTrueData = std::make_unique<ValidatorPointsCloud>();
  *pcTrueData = eventSetup.get<ValidatorPointsCloud>();

  auto const& pcDeviceProduct = event.get(tokenPC_);
  cms::sycltools::ScopedContextProduce ctx{pcDeviceProduct};
  auto const& pcDevice = ctx.get(pcDeviceProduct);

  std::cout << "Checking input data on device" << '\n';

  assert(pcDevice.n == pcTrueData->n);
  std::cout << "Number of points -> Ok" << '\n';
  assert(arraysAreEqual(pcDevice.x.get(), pcTrueData->x, pcDevice.n, ctx.stream()));
  std::cout << "x -> Ok" << '\n';
  assert(arraysAreEqual(pcDevice.y.get(), pcTrueData->y, pcDevice.n, ctx.stream()));
  std::cout << "y -> Ok" << '\n';
  assert(arraysAreEqual(pcDevice.layer.get(), pcTrueData->layer, pcDevice.n, ctx.stream()));
  std::cout << "layer -> Ok" << '\n';
  assert(arraysAreEqual(pcDevice.weight.get(), pcTrueData->weight, pcDevice.n, ctx.stream()));
  std::cout << "weight -> Ok" << '\n';

  std::cout << "Input data copied correctly from host to device" << std::endl;
}
DEFINE_FWK_MODULE(ValidatorPointsCloudToSYCL);
