#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"
#include "SYCLCore/Product.h"
#include "SYCLCore/ScopedContext.h"
#include "DataFormats/PointsCloud.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "CLUEValidatorTypes.h"

std::vector<float> CLAMPED(std::vector<float> in, float upperLimit) {
  std::vector<float> out(in);
  for (size_t i = 0; i < out.size(); i++)
    if (out[i] > upperLimit) out[i] = upperLimit;
  return out;
}

class CLUEValidator : public edm::EDProducer {
public:
  explicit CLUEValidator(edm::ProductRegistry& reg);

private:
  void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
  template <class T>
  bool arraysAreEqual(std::vector<T>, std::vector<T> trueDataArr);
  bool arraysClustersEqual(const PointsCloud& devicePC, const PointsCloud& truePC);
  void transferToHost(const PointsCloudSYCL& pcDevice, PointsCloud& pc, sycl::queue stream);
  void saveDeviceToOutputFile(const PointsCloud& pc, std::string filePath);
  void validateOutput(const PointsCloud& pc, std::string trueOutFilePath);
  edm::EDGetTokenT<cms::sycltools::Product<PointsCloudSYCL>> tokenPC_;
};

CLUEValidator::CLUEValidator(edm::ProductRegistry& reg)
  : tokenPC_(reg.consumes<cms::sycltools::Product<PointsCloudSYCL>>()) {}

template <class T>
bool CLUEValidator::arraysAreEqual(std::vector<T> devicePtr, std::vector<T> trueDataArr) {
  bool sameValue = true;
  for (size_t i = 0; i < devicePtr.size(); i++) {
    if (std::is_same<T, int>::value) {
      sameValue = devicePtr[i] == trueDataArr[i];
    }
    else {
      const float TOLERANCE = 0.001;
      sameValue = std::abs(devicePtr[i] - trueDataArr[i]) <= TOLERANCE;
    }

    if (!sameValue) {
      std::cout << "failed comparison for i=" << i << ", " << devicePtr[i] << " /= " << trueDataArr[i] << std::endl;
      break;
    }
  }
  return sameValue;
}

bool CLUEValidator::arraysClustersEqual(const PointsCloud& devicePC, const PointsCloud& truePC) {

  std::unordered_map<int, int> clusterIdMap;

  int n = (int)devicePC.x.size();

  for (int i = 0; i < n; i++) {
    if (devicePC.isSeed[i]) {
      clusterIdMap[devicePC.clusterIndex[i]] = truePC.clusterIndex[i];
    }
  }

  bool sameValue = true;
  for (int i = 0; i < n; i++) {
    int originalClusterId = devicePC.clusterIndex[i];
    int mappedClusterId = clusterIdMap[originalClusterId];
    if (originalClusterId == -1)
      mappedClusterId = -1;

    sameValue = (mappedClusterId == truePC.clusterIndex[i]);

    if (!sameValue) {
      std::cout << "failed comparison for i=" << i << ", original=" << originalClusterId << ", mapped= " << mappedClusterId << " /= " << truePC.clusterIndex[i] << std::endl;
      break;
    }
  }

  return sameValue;
}

void CLUEValidator::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
  auto outDataDir = std::make_unique<OutputDirPath>();
  *outDataDir = eventSetup.get<OutputDirPath>();

  auto const& pcProduct = event.get(tokenPC_);
  cms::sycltools::ScopedContextProduce ctx{pcProduct};
  auto const& pcDevice = ctx.get(pcProduct);

  auto stream = ctx.stream();

  PointsCloud pc(pcDevice.n);
  transferToHost(pcDevice, pc, stream);
  std::cout << "Num of points: " << pcDevice.n << std::endl;

  std::cout << "Saving into " << outDataDir->path_ / "clue_output.csv" << std::endl;
  saveDeviceToOutputFile(pc, outDataDir->path_ / "clue_output.csv");
  std::cout << "Results were saved!" << std::endl;

  std::cout << "Validating CLUE output..." << '\n';
  std::cout << "Validating from " << outDataDir->path_ / "toyDetector_10000.csv" << std::endl;
  validateOutput(pc, outDataDir->path_ / "toyDetector_10000.csv");
  std::cout << "CLUE output is correct!" << '\n';
}

void CLUEValidator::transferToHost(const PointsCloudSYCL& pcDevice, PointsCloud& pc, sycl::queue stream) {
  pc.n = pcDevice.n;
  stream.memcpy(pc.x.data(), pcDevice.x.get(), pcDevice.n*sizeof(float));
  stream.memcpy(pc.y.data(), pcDevice.y.get(), pcDevice.n*sizeof(float));
  stream.memcpy(pc.layer.data(), pcDevice.layer.get(), pcDevice.n*sizeof(int));
  stream.memcpy(pc.weight.data(), pcDevice.weight.get(), pcDevice.n*sizeof(float));
  stream.memcpy(pc.rho.data(), pcDevice.rho.get(), pcDevice.n*sizeof(float));
  stream.memcpy(pc.delta.data(), pcDevice.delta.get(), pcDevice.n*sizeof(float));
  stream.memcpy(pc.nearestHigher.data(), pcDevice.nearestHigher.get(), pcDevice.n*sizeof(int));
  stream.memcpy(pc.isSeed.data(), pcDevice.isSeed.get(), pcDevice.n*sizeof(int));
  stream.memcpy(pc.clusterIndex.data(), pcDevice.clusterIndex.get(), pcDevice.n*sizeof(int))
    .wait();
}

void CLUEValidator::saveDeviceToOutputFile(const PointsCloud& pc, std::string filePath) {
  std::ofstream clueOut(filePath);
  clueOut << "index,x,y,layer,weight,rho,delta,nh,isSeed,clusterId\n";
  for (int i = 0; i < pc.n; i++) {
    clueOut << i << ","
              << pc.x[i] << "," << pc.y[i] << ","
              << pc.layer[i] << "," << pc.weight[i] << ","
              << pc.rho[i] << "," << (pc.delta[i] > 999 ? 999 : pc.delta[i]) << ","
              << pc.nearestHigher[i] << "," << pc.isSeed[i] << ","
              << pc.clusterIndex[i] << "\n";
  }

  clueOut.close();
}

void CLUEValidator::validateOutput(const PointsCloud& pc, std::string trueOutFilePath) {
  PointsCloud truePC;
  std::ifstream iTrueDataFile(trueOutFilePath);
  std::string value = "";
  // Get Header Line
  getline(iTrueDataFile, value);
  std::cout << "Read header: " << value << std::endl;
  // Iterate through each line and split the content using delimeter
  int n = 1;
  while (getline(iTrueDataFile, value, ',')) {
    // int id = std::stoi(value);
    try {
      getline(iTrueDataFile, value, ',');
      truePC.x.push_back(std::stof(value));
      getline(iTrueDataFile, value, ',');
      truePC.y.push_back(std::stof(value));
      getline(iTrueDataFile, value, ',');
      truePC.layer.push_back(std::stoi(value));
      getline(iTrueDataFile, value, ',');
      truePC.weight.push_back(std::stof(value));
      getline(iTrueDataFile, value, ',');
      truePC.rho.push_back(std::stof(value));
      getline(iTrueDataFile, value, ',');
      truePC.delta.push_back(std::stof(value));
      getline(iTrueDataFile, value, ',');
      truePC.nearestHigher.push_back(std::stoi(value));
      getline(iTrueDataFile, value, ',');
      truePC.isSeed.push_back(std::stoi(value));
      getline(iTrueDataFile, value);
      truePC.clusterIndex.push_back(std::stoi(value));
    } catch(std::exception& e) {
      std::cout << e.what() << std::endl;
      std::cout << "Bad Input: '" << value << "' in line " << n << std::endl;
      break;
    }
    n++;
  }
  std::cout << "Read true points!\n";
  iTrueDataFile.close();

  std::cout << "0: " << truePC.x[0] << "," << truePC.y[0] << ","
                    << truePC.layer[0] << "," << truePC.weight[0] << ","
                    << truePC.rho[0] << "," << truePC.delta[0] << ","
                    << truePC.nearestHigher[0] << "," << truePC.isSeed[0] << ","
                    << truePC.clusterIndex[0] << std::endl;

  assert(arraysAreEqual(pc.rho, truePC.rho));
  assert(arraysAreEqual(CLAMPED(pc.delta, 999), truePC.delta));
  assert(arraysAreEqual(pc.nearestHigher, truePC.nearestHigher));
  assert(arraysAreEqual(pc.isSeed, truePC.isSeed));
  assert(arraysClustersEqual(pc, truePC));
}

DEFINE_FWK_MODULE(CLUEValidator);