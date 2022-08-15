#include <fstream>
#include <filesystem>
#include <memory>
#include "Framework/ESProducer.h"
#include "Framework/EventSetup.h"
#include "Framework/ESPluginFactory.h"

class CLUEOutputESProducer : public edm::ESProducer {
public:
  CLUEOutputESProducer(std::filesystem::path const& inputFile) : data_{inputFile} {
    outFileName_ = getOutputFileName(inputFile);
  }
  void produce(edm::EventSetup& eventSetup);
  std::string getOutputFileName(std::filesystem::path const& inputFile) {
    std::string fileName = inputFile.filename();
    fileName.erase(fileName.end() - 4, fileName.end());
    fileName.append("_output.csv");
    return fileName;
  }

private:
  std::string outFileName_;
  std::filesystem::path data_;
};

void CLUEOutputESProducer::produce(edm::EventSetup& eventSetup) {
  auto outDir = std::make_unique<std::filesystem::path>(data_.parent_path().parent_path() / "output" / outFileName_);
  eventSetup.put(std::move(outDir));
}

DEFINE_FWK_EVENTSETUP_MODULE(CLUEOutputESProducer);