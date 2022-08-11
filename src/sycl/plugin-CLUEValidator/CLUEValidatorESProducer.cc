#include <fstream>
#include <memory>
#include "Framework/ESProducer.h"
#include "Framework/EventSetup.h"
#include "Framework/ESPluginFactory.h"
#include "CLUEValidatorTypes.h"
// #include "DataFormats/LayerTilesConstants.h"

class CLUEValidatorESProducer : public edm::ESProducer {
public:
  CLUEValidatorESProducer(std::filesystem::path const& datadir) : data_{datadir} {}
  void produce(edm::EventSetup& eventSetup);

private:
  std::filesystem::path data_;
};

void CLUEValidatorESProducer::produce(edm::EventSetup& eventSetup) {
  auto outDir = std::make_unique<OutputDirPath>(OutputDirPath{data_ / "output/"});
  eventSetup.put(std::move(outDir));
  // Create empty PointsCloud
  // ValidatorPointsCloud cloud;
  // for (int l = 0; l < NLAYERS; l++) {
  //   // open csv file
  //   std::ifstream iFile(data_ / "output/toyDetector_1000.csv");
  //   std::string value = "";

  //   // Get and ignore first header line
    
  //   // Iterate through each line and split the content using delimeter
  //   while (getline(iFile, value, ',')) {
  //     int pointId = std::stoi(value);
  //     getline(iFile, value, ',')
  //     cloud.x.push_back(std::stof(value));
  //     getline(iFile, value, ',');
  //     cloud.y.push_back(std::stof(value));
  //     getline(iFile, value, ',');
  //     cloud.layer.push_back(std::stoi(value) + l);
  //     getline(iFile, value);
  //     cloud.weight.push_back(std::stof(value));

  //     getline(iFile, value);
  //     cloud.rho.push_back(std::stof(value));
  //     getline(iFile, value);
  //     cloud.delta.push_back(std::stof(value));
  //     getline(iFile, value);
  //     cloud.nearestHigher.push_back(std::stoi(value));
  //     getline(iFile, value);
  //     cloud.isSeed.push_back(std::stoi(value));
  //     getline(iFile, value);
  //     cloud.clusterIndex.push_back(std::stoo(value));
  //   }
  //   iFile.close();
  // }
  // cloud.n = cloud.x.size();
  // auto pc = std::make_unique<ValidatorPointsCloud>(cloud);
  // eventSetup.put(std::move(pc));
}

DEFINE_FWK_EVENTSETUP_MODULE(CLUEValidatorESProducer);