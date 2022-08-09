#include <iostream>

#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"

#include "SYCLCore/Product.h"
#include "SYCLCore/ScopedContext.h"
// #include "SYCLCore/device_unique_ptr.h"

// #include "DataFormats/PointsCloud.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "ValidatorData.h"

class ValidatorPointsCloudToSYCL : public edm::EDProducer {
public:
  explicit ValidatorPointsCloudToSYCL(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
  template<class T>
  bool arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n);
  edm::EDGetTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenPC_;
};

ValidatorPointsCloudToSYCL::ValidatorPointsCloudToSYCL(edm::ProductRegistry& reg)
    : tokenPC_(reg.consumes<cms::sycltools::Product<PointsCloudSYCL>>()) {}

template<class T>
bool ValidatorPointsCloudToSYCL::arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n) {
  bool sameValue = true;
    for(int i = 0; i < n; i++) {
      if (devicePtr[i] != trueDataArr[i]) {
        sameValue = false;
        break;
      }
    }

    return sameValue;
}

void ValidatorPointsCloudToSYCL::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  auto pcTrueData = std::make_unique<ValidatorPointsCloud>();
  *pcTrueData = eventSetup.get<ValidatorPointsCloud>();
  
  auto const& pcDeviceProduct = event.get(tokenPC_);
  cms::sycltools::ScopedContextProduce ctx{pcDeviceProduct};
  auto const& pcDevice = ctx.get(pcDeviceProduct);

  std::cout << "Checking pcDevice.n=" << pcDevice.n << "  ==   pcTrueData.n=" << pcTrueData->n << std::endl;
  assert(pcDevice.n == pcTrueData->n);
  
  std::cout << "Are device and trueData x's equal?\n";
  assert(arraysAreEqual(pcDevice.x.get(), pcTrueData->x, pcDevice.n));

  std::cout << "Are device and trueData y's equal?\n";
  assert(arraysAreEqual(pcDevice.y.get(), pcTrueData->y, pcDevice.n));

  std::cout << "Are device and trueData layer's equal?\n";
  assert(arraysAreEqual(pcDevice.layer.get(), pcTrueData->layer, pcDevice.n));

  std::cout << "Are device and trueData weight's equal?\n";
  assert(arraysAreEqual(pcDevice.weight.get(), pcTrueData->weight, pcDevice.n));

  std::vector<float> fZeros(pcDevice.n, 0);
  std::vector<int> iZeros(pcDevice.n, 0);

  std::cout << "Are device  rho initially 0?\n";
  assert(arraysAreEqual(pcDevice.rho.get(), fZeros, pcDevice.n));

  std::cout << "Are device  delta initially 0?\n";
  assert(arraysAreEqual(pcDevice.delta.get(), fZeros, pcDevice.n));

  std::cout << "Are device nearestHigher initially 0?\n";
  assert(arraysAreEqual(pcDevice.nearestHigher.get(), iZeros, pcDevice.n));

  std::cout << "Are device clusterIndex initially 0?\n";
  assert(arraysAreEqual(pcDevice.clusterIndex.get(), iZeros, pcDevice.n));

  std::cout << "Are device clusterIndex initially 0?\n";
  assert(arraysAreEqual(pcDevice.isSeed.get(), iZeros, pcDevice.n));

  // std::cout << "Printing x's\n";
  // for (int i = 0; i < 10; i++) {
  //   std::cout << "x[" << i << "]=" << pcDevice.x[i] << "\n";
  // }

}
DEFINE_FWK_MODULE(ValidatorPointsCloudToSYCL);
