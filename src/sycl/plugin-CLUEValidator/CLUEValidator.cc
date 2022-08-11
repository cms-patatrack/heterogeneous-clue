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
#include "CLUEValidatorTypes.h"
// #include "ValidatorData.h"

class CLUEValidator : public edm::EDProducer {
public:
  explicit CLUEValidator(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
  template <class T>
  bool arraysAreEqual(T* devicePtr, std::vector<T> trueDataArr, int n, sycl::queue stream);
  edm::EDGetTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenPC_;
};

CLUEValidator::CLUEValidator(edm::ProductRegistry& reg)
  : tokenPC_(reg.consumes<cms::sycltools::Product<PointsCloudSYCL>>()) {}

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

  auto const& pcProduct = event.get(tokenPC_);
  cms::sycltools::ScopedContextProduce ctx{pcProduct};
  auto const& pcDevice = ctx.get(pcProduct);

  auto stream = ctx.stream();

  std::cout << "Num of points: " << pcDevice.n << std::endl;

  std::cout << "Saving into " << outDataDir->path_ / "clue_output.csv" << std::endl;
  std::ofstream clueOut(outDataDir->path_ / "clue_output.csv");

  clueOut << "index,x,y,layer,weight,rho,delta,nh,isSeed,clusterId\n";

  auto x = new float[pcDevice.n];
  auto y = new float[pcDevice.n];
  auto layer = new int[pcDevice.n];
  auto weight = new float[pcDevice.n];
  auto rho = new float[pcDevice.n];
  auto delta = new float[pcDevice.n];
  auto nearestHigher = new int[pcDevice.n];
  auto isSeed = new int[pcDevice.n];
  auto clusterIndex = new int[pcDevice.n];
  stream.memcpy(x, pcDevice.x.get(), pcDevice.n*sizeof(float));
  stream.memcpy(y, pcDevice.y.get(), pcDevice.n*sizeof(float));
  stream.memcpy(layer, pcDevice.layer.get(), pcDevice.n*sizeof(int));
  stream.memcpy(weight, pcDevice.weight.get(), pcDevice.n*sizeof(float));
  stream.memcpy(rho, pcDevice.rho.get(), pcDevice.n*sizeof(float));
  stream.memcpy(delta, pcDevice.delta.get(), pcDevice.n*sizeof(float));
  stream.memcpy(nearestHigher, pcDevice.nearestHigher.get(), pcDevice.n*sizeof(int));
  stream.memcpy(isSeed, pcDevice.isSeed.get(), pcDevice.n*sizeof(int));
  stream.memcpy(clusterIndex, pcDevice.clusterIndex.get(), pcDevice.n*sizeof(int))
    .wait();


  for (int i = 0; i < pcDevice.n; i++) {
    clueOut << i << ","
              << x[i] << "," << y[i] << ","
              << layer[i] << "," << weight[i] << ","
              << rho[i] << "," << (delta[i] > 999 ? 999 : delta[i]) << ","
              << nearestHigher[i] << "," << isSeed[i] << ","
              << clusterIndex[i] << "\n";
  }

  clueOut.close();

  delete[] x;
  delete[] y;
  delete[] layer;
  delete[] weight;
  delete[] rho;
  delete[] delta;
  delete[] nearestHigher;
  delete[] isSeed;
  delete[] clusterIndex;

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
  
  std::cout << "Results were saved!" << std::endl;
}
DEFINE_FWK_MODULE(CLUEValidator);
