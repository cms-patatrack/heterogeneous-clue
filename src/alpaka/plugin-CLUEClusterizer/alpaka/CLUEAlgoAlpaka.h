#ifndef CLUEAlgo_Alpaka_h
#define CLUEAlgo_Alpaka_h

// #include <optional>

#include "AlpakaCore/alpakaConfig.h"
#include "AlpakaCore/alpakaMemory.h"
#include "AlpakaDataFormats/alpaka/PointsCloudAlpaka.h"
#include "AlpakaDataFormats/LayerTilesAlpaka.h"

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  class CLUEAlgoAlpaka {
  public:
    // constructor
    CLUEAlgoAlpaka() = delete;
    explicit CLUEAlgoAlpaka(float const &dc,
                            float const &rhoc,
                            float const &outlierDeltaFactor,
                            Queue stream,
                            uint32_t const &numberOfPoints)
        : d_points{stream, numberOfPoints},
          queue_{std::move(stream)},
          dc_{dc},
          rhoc_{rhoc},
          outlierDeltaFactor_{outlierDeltaFactor} {
      init_device();
    }

    ~CLUEAlgoAlpaka() = default;

    void makeClusters(PointsCloud const &host_pc);

    PointsCloudAlpaka d_points;

    LayerTilesAlpaka<Acc1D> *hist_;
    cms::alpakatools::VecArray<int, maxNSeeds> *seeds_;
    cms::alpakatools::VecArray<int, maxNFollowers> *followers_;

  private:
    Queue queue_;
    float dc_;
    float rhoc_;
    float outlierDeltaFactor_;

    std::optional<cms::alpakatools::device_buffer<Device, LayerTilesAlpaka<Acc1D>[]>> d_hist;
    std::optional<cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<int, maxNSeeds>>> d_seeds;
    std::optional<cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<int, maxNFollowers>[]>> d_followers;

    // private methods
    void init_device();

    void setup(PointsCloud const &host_pc);
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif