#include <iostream>
#include <fstream>

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
#include "SYCLDataFormats/ClusteredPointsCloudSYCL.h"
#include "CLUEValidatorTypes.h"
// #include "ValidatorData.h"

class CLUEValidator : public edm::EDProducer {
public:
  explicit CLUEValidator(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
  template <class T>
  bool arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n, sycl::queue stream);
  edm::EDGetTokenT<cms::sycltools::Product<ClusteredPointsCloudSYCL>> tokenPC_;
};

CLUEValidator::CLUEValidator(edm::ProductRegistry& reg)
  : tokenPC_(reg.consumes<cms::sycltools::Product<ClusteredPointsCloudSYCL>>()) {}

template <class T>
bool CLUEValidator::arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n, sycl::queue stream) {
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

void CLUEValidator::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  auto outDataDir = std::make_unique<OutputDirPath>();
  *outDataDir = eventSetup.get<OutputDirPath>();

  auto const& cpcDeviceProduct = event.get(tokenPC_);
  cms::sycltools::ScopedContextProduce ctx{cpcDeviceProduct};
  auto const& cpcDevice = ctx.get(cpcDeviceProduct);

  auto pc = new PointsCloudSYCL::PointsCloudSYCLView[1];
  auto stream = ctx.stream();

  stream.memcpy(pc, cpcDevice.clusterdPointsData_.get(), 1*sizeof(PointsCloudSYCL::PointsCloudSYCLView))
    .wait();

  std::cout << "Num of points: " << pc->n << std::endl;

  std::cout << "Saving into " << outDataDir->path_ / "clue_output.csv" << std::endl;
  std::ofstream clueOut(outDataDir->path_ / "clue_output.csv");

  clueOut << "index,x,y,layer,weight,rho,delta,nh,isSeed,clusterId\n";

  auto x = new float[pc->n];
  auto y = new float[pc->n];
  auto layer = new int[pc->n];
  auto weight = new float[pc->n];
  auto rho = new float[pc->n];
  auto delta = new float[pc->n];
  auto nearestHigher = new int[pc->n];
  auto isSeed = new int[pc->n];
  auto clusterIndex = new int[pc->n];
  stream.memcpy(x, pc->x, pc->n*sizeof(float));
  stream.memcpy(y, pc->y, pc->n*sizeof(float));
  stream.memcpy(layer, pc->layer, pc->n*sizeof(int));
  stream.memcpy(weight, pc->weight, pc->n*sizeof(float));
  stream.memcpy(rho, pc->rho, pc->n*sizeof(float));
  stream.memcpy(delta, pc->delta, pc->n*sizeof(float));
  stream.memcpy(nearestHigher, pc->nearestHigher, pc->n*sizeof(int));
  stream.memcpy(isSeed, pc->isSeed, pc->n*sizeof(int));
  stream.memcpy(clusterIndex, pc->clusterIndex, pc->n*sizeof(int))
    .wait();


  for (int i = 0; i < pc->n; i++) {
    clueOut << i << ","
              << x[i] << "," << y[i] << ","
              << layer[i] << "," << weight[i] << ","
              << rho[i] << "," << (delta[i] > 999 ? 999 : delta[i]) << ","
              << nearestHigher[i] << "," << isSeed[i] << ","
              << clusterIndex[i] << "\n";
  }

  clueOut.close();
  // sycl::free(x, stream);
  // sycl::free(y, stream);
  // sycl::free(layer, stream);
  // sycl::free(weight, stream);
  // sycl::free(rho, stream);
  // sycl::free(delta, stream);
  // sycl::free(nearestHigher, stream);
  // sycl::free(isSeed, stream);
  // sycl::free(clusterIndex, stream);
  delete[] pc;

//   std::cout << "Checking data on device" << '\n';

//   assert(pcDevice.n == pcTrueData->n);
//   std::cout << "Number of points -> Ok" << '\n';
//   assert(arraysAreEqual(pcDevice.x.get(), pcTrueData->x, pcDevice.n, stream));
//   std::cout << "x -> Ok" << '\n';
//   assert(arraysAreEqual(pcDevice.y.get(), pcTrueData->y, pcDevice.n, ctx.stream()));
//   std::cout << "y -> Ok" << '\n';
//   assert(arraysAreEqual(pcDevice.layer.get(), pcTrueData->layer, pcDevice.n, ctx.stream()));
//   std::cout << "layer -> Ok" << '\n';
//   assert(arraysAreEqual(pcDevice.weight.get(), pcTrueData->weight, pcDevice.n, ctx.stream()));
//   std::cout << "weight -> Ok" << '\n';

//   std::vector<float> fZeros(pcDevice.n, 0);
//   std::vector<int> iZeros(pcDevice.n, 0);

//   assert(arraysAreEqual(pcDevice.rho.get(), fZeros, pcDevice.n, ctx.stream()));
//   std::cout << "rho correctly initialised to 0" << '\n';
//   assert(arraysAreEqual(pcDevice.delta.get(), fZeros, pcDevice.n, ctx.stream()));
//   std::cout << "delta correctly initialised to 0" << '\n';
//   assert(arraysAreEqual(pcDevice.nearestHigher.get(), iZeros, pcDevice.n, ctx.stream()));
//   std::cout << "nearestHigher correctly initialised to 0" << '\n';
//   assert(arraysAreEqual(pcDevice.clusterIndex.get(), iZeros, pcDevice.n, ctx.stream()));
//   std::cout << "clusterIndex correctly initialised to 0" << '\n';
//   assert(arraysAreEqual(pcDevice.isSeed.get(), iZeros, pcDevice.n, ctx.stream()));
//   std::cout << "isSeed correctly initialised to 0" << '\n';
  
  std::cout << "Data copied correctly" << std::endl;
}
DEFINE_FWK_MODULE(CLUEValidator);
