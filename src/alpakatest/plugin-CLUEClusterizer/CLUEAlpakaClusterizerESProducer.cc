#include <fstream>
#include <memory>
#include "Framework/ESProducer.h"
#include "Framework/EventSetup.h"
#include "Framework/ESPluginFactory.h"
#include "DataFormats/CLUE_config.h"

class CLUEAlpakaClusterizerESProducer : public edm::ESProducer {
public:
  CLUEAlpakaClusterizerESProducer(std::filesystem::path const& config_file) : data_{config_file} {}
  void produce(edm::EventSetup& eventSetup);

private:
  std::filesystem::path data_;
};

void CLUEAlpakaClusterizerESProducer::produce(edm::EventSetup& eventSetup) {
  Parameters par;
  std::ifstream iFile(data_);
  std::string value = "";
  while (getline(iFile, value, ',')) {
    par.dc = std::stof(value);
    getline(iFile, value, ',');
    par.rhoc = std::stof(value);
    getline(iFile, value, ',');
    par.outlierDeltaFactor = std::stof(value);
    getline(iFile, value);
    par.verbose = static_cast<bool>(std::stoi(value));
  }
  iFile.close();

  std::cout << "Parameters in ESProducer: \n";
  std::cout << "dc: " << par.dc << '\n';
  std::cout << "rhoc: " << par.rhoc << '\n';
  std::cout << "outlier delta factor: " << par.outlierDeltaFactor << '\n';
  std::cout << "verbose: " << par.verbose << '\n';

  auto parameters = std::make_unique<Parameters>(par);
  eventSetup.put(std::move(parameters));
}

DEFINE_FWK_EVENTSETUP_MODULE(CLUEAlpakaClusterizerESProducer);