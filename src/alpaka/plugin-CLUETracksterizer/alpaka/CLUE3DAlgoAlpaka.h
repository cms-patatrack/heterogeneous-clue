#ifndef CLUE3DAlgo_Alpaka_h
#define CLUE3DAlgo_Alpaka_h

#include "AlpakaCore/alpakaConfig.h"
#include "AlpakaCore/alpakaMemory.h"
#include "AlpakaDataFormats/alpaka/ClusterCollectionAlpaka.h"
#include "AlpakaDataFormats/TICLLayerTilesAlpaka.h"
#include "DataFormats/Common.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class CLUE3DAlgoAlpaka {
  public:
    // constructor
    CLUE3DAlgoAlpaka() = delete;
    explicit CLUE3DAlgoAlpaka(Queue stream, uint32_t const &numberOfPoints)
        : d_clusters{stream, numberOfPoints}, queue_{std::move(stream)} {
      init_device();
    }

    ~CLUE3DAlgoAlpaka() = default;

    // CLUE3DAlgoAlpaka() {
    //   hist_ = new TICLLayerTiles;
    //   histSoA_ = new TICLLayerTiles;
    // }

    // ~CLUE3DAlgoAlpaka() {
    //   delete hist_;
    //   delete histSoA_;
    // };

    void makeTracksters(ClusterCollection const &host_pc);

    ClusterCollectionAlpaka d_clusters;

    TICLLayerTilesAlpaka *hist_;
    cms::alpakatools::VecArray<int, ticl::maxNSeeds> *seeds_;
    cms::alpakatools::VecArray<std::pair<int, int>, ticl::maxNFollowers> *followers_;

  private:
    Queue queue_;

    std::optional<cms::alpakatools::device_buffer<Device, TICLLayerTilesAlpaka[]>> d_hist;
    std::optional<cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<int, ticl::maxNSeeds>>> d_seeds;
    std::optional<
        cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<std::pair<int, int>, ticl::maxNFollowers>[]>>
        d_followers;

    void init_device();
    void setup(ClusterCollection const &pc);
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif
