#include "DataFormats/ClusterCollection.h"

#include "AlpakaCore/alpakaConfig.h"
#include "AlpakaCore/alpakaWorkDiv.h"
#include "CLUE3DAlgoAlpaka.h"
#include "CLUE3DAlgoKernels.h"
#include "DataFormats/Common.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  void CLUE3DAlgoAlpaka::init_device() {
    d_hist = cms::alpakatools::make_device_buffer<TICLLayerTilesAlpaka[]>(queue_, ticl::TileConstants::nLayers);
    d_seeds = cms::alpakatools::make_device_buffer<cms::alpakatools::VecArray<int, ticl::maxNSeeds>>(queue_);
    d_followers =
        cms::alpakatools::make_device_buffer<cms::alpakatools::VecArray<std::pair<int, int>, ticl::maxNFollowers>[]>(
            queue_, reserve);

    hist_ = (*d_hist).data();
    seeds_ = (*d_seeds).data();
    followers_ = (*d_followers).data();
  }

  void CLUE3DAlgoAlpaka::setup(ClusterCollection const &pc) {
    // copy input variables
    alpaka::memcpy(queue_, d_clusters.x, cms::alpakatools::make_host_view(pc.x.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.y, cms::alpakatools::make_host_view(pc.y.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.z, cms::alpakatools::make_host_view(pc.z.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.eta, cms::alpakatools::make_host_view(pc.eta.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.phi, cms::alpakatools::make_host_view(pc.phi.data(), pc.x.size()));
    alpaka::memcpy(
        queue_, d_clusters.r_over_absz, cms::alpakatools::make_host_view(pc.r_over_absz.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.radius, cms::alpakatools::make_host_view(pc.radius.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.layer, cms::alpakatools::make_host_view(pc.layer.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.energy, cms::alpakatools::make_host_view(pc.energy.data(), pc.x.size()));
    alpaka::memcpy(queue_, d_clusters.isSilicon, cms::alpakatools::make_host_view(pc.isSilicon.data(), pc.x.size()));
    // initialize result and internal variables
    alpaka::memset(queue_, d_clusters.rho, 0x00, static_cast<uint32_t>(pc.x.size()));
    alpaka::memset(queue_, d_clusters.delta, 0x00, static_cast<uint32_t>(pc.x.size()));
    alpaka::memset(queue_, d_clusters.nearestHigher, 0x00, static_cast<uint32_t>(pc.x.size()));
    alpaka::memset(queue_, d_clusters.tracksterIndex, 0x00, static_cast<uint32_t>(pc.x.size()));
    alpaka::memset(queue_, d_clusters.isSeed, 0x00, static_cast<uint32_t>(pc.x.size()));
    alpaka::memset(queue_, (*d_hist), 0x00, static_cast<uint32_t>(ticl::TileConstants::nLayers));
    alpaka::memset(queue_, (*d_seeds), 0x00);
    alpaka::memset(queue_, (*d_followers), 0x00, static_cast<uint32_t>(pc.x.size()));

    alpaka::wait(queue_);
  }

  void CLUE3DAlgoAlpaka::makeTracksters(ClusterCollection const &pc) {
    setup(pc);
    // calculate rho, delta and find seeds
    // 1 point per thread
    const Idx blockSize = 1024;
    const Idx gridSize = ceil(pc.x.size() / static_cast<float>(blockSize));
    auto WorkDiv1D = cms::alpakatools::make_workdiv<Acc1D>(gridSize, blockSize);
    alpaka::enqueue(
        queue_,
        alpaka::createTaskKernel<Acc1D>(WorkDiv1D, KernelComputeHistogram(), hist_, d_clusters.view(), d_clusters.n));
    alpaka::enqueue(
        queue_,
        alpaka::createTaskKernel<Acc1D>(WorkDiv1D, KernelCalculateDensity(), hist_, d_clusters.view(), d_clusters.n));
    alpaka::enqueue(queue_,
                    alpaka::createTaskKernel<Acc1D>(
                        WorkDiv1D, KernelComputeDistanceToHigher(), hist_, d_clusters.view(), d_clusters.n));
    alpaka::enqueue(queue_,
                    alpaka::createTaskKernel<Acc1D>(
                        WorkDiv1D, KernelFindClusters(), seeds_, followers_, d_clusters.view(), d_clusters.n));
    alpaka::enqueue(queue_,
                    alpaka::createTaskKernel<Acc1D>(
                        WorkDiv1D, KernelAssignClusters(), seeds_, followers_, d_clusters.view(), d_clusters.n));

    alpaka::wait(queue_);

    // histSoA_->clear();
  }
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE