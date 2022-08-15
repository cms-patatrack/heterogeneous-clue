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
}

DEFINE_FWK_EVENTSETUP_MODULE(CLUEValidatorESProducer);