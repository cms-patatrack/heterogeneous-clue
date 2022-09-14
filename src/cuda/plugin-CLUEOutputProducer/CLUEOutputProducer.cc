#include <iostream>
#include <memory>
#include <filesystem>
#include <fstream>

#include "DataFormats/FEDRawDataCollection.h"
#include "Framework/EDProducer.h"
#include "Framework/Event.h"
#include "Framework/EventSetup.h"
#include "Framework/PluginFactory.h"

#include "DataFormats/CLUE_config.h"
#include "DataFormats/PointsCloud.h"

#include "CUDACore/Product.h"
#include "CUDACore/cudaCheck.h"
//#include "CUDACore/PluginWrapper.h"
#include "CUDACore/ScopedContext.h"

#include "CUDADataFormats/PointsCloudCUDA.h"

  class CLUEOutputProducer : public edm::EDProducer {
  public:
    explicit CLUEOutputProducer(edm::ProductRegistry& reg);

  private:
    void produce(edm::Event& event, edm::EventSetup const& eventSetup) override;
    edm::EDGetTokenT<cms::cuda::Product<PointsCloudCUDA>> deviceClustersToken_;
    edm::EDPutTokenT<cms::cuda::Product<PointsCloud>> resultsToken_;
    edm::EDGetTokenT<PointsCloud> pointsCloudToken_;
  };

  CLUEOutputProducer::CLUEOutputProducer(edm::ProductRegistry& reg)
      : deviceClustersToken_(reg.consumes<cms::cuda::Product<PointsCloudCUDA>>()),
        resultsToken_(reg.produces<cms::cuda::Product<PointsCloud>>()),
        pointsCloudToken_(reg.consumes<PointsCloud>()) {}

  void CLUEOutputProducer::produce(edm::Event& event, edm::EventSetup const& eventSetup) {
		std::cout << "YOOO " << std::endl;
    auto outDir = eventSetup.get<std::filesystem::path>();
    auto results = event.get(pointsCloudToken_);
    auto const& pcProduct = event.get(deviceClustersToken_);
    cms::cuda::ScopedContextProduce ctx{pcProduct};
    auto const& device_clusters = ctx.get(pcProduct);
    auto stream = ctx.stream();
    cms::cuda::host::unique_ptr<PointsCloud> m_soa;
    results.outResize(device_clusters.n);
		cudaCheck(cudaMemcpy(results.rho.data(), &(device_clusters.rho), sizeof(float)*device_clusters.n, cudaMemcpyDefault));
		cudaCheck(cudaMemcpy(results.delta.data(), &(device_clusters.delta), sizeof(results.delta) , cudaMemcpyDefault));
		cudaCheck(cudaMemcpy(results.nearestHigher.data(), &(device_clusters.nearestHigher), sizeof(results.nearestHigher),  cudaMemcpyDefault));
		cudaCheck(cudaMemcpy(results.isSeed.data(), &(device_clusters.isSeed), sizeof(results.isSeed) , cudaMemcpyDefault));
		cudaCheck(cudaMemcpy(results.clusterIndex.data(), &(device_clusters.clusterIndex), sizeof(results.clusterIndex), cudaMemcpyDefault));
    cudaStreamSynchronize(stream);

    std::cout << "Data transferred back to host" << std::endl;

    Parameters par;
    par = eventSetup.get<Parameters>();
    if (true) {
      auto temp_outDir = eventSetup.get<std::filesystem::path>();
      std::string input_file_name = temp_outDir.filename();
      std::string output_file_name = create_outputfileName(input_file_name, par.dc, par.rhoc, par.outlierDeltaFactor);
      std::filesystem::path outDir = temp_outDir.parent_path() / output_file_name;

      std::ofstream clueOut(outDir);

      clueOut << "index,x,y,layer,weight,rho,delta,nh,isSeed,clusterId\n";
      for (unsigned int i = 0; i < device_clusters.n; i++) {
        clueOut << i << "," << results.x[i] << "," << results.y[i] << "," << results.layer[i] << ","
                << results.weight[i] << "," << results.rho[i] << ","
                << (results.delta[i] > 999 ? 999 : results.delta[i]) << "," << results.nearestHigher[i] << ","
                << results.isSeed[i] << "," << results.clusterIndex[i] << "\n";
      }

      clueOut.close();

      std::cout << "Ouput was saved in " << outDir << std::endl;
    }
		
    ctx.emplace(event, resultsToken_, std::move(results));
  }

DEFINE_FWK_MODULE(CLUEOutputProducer);
