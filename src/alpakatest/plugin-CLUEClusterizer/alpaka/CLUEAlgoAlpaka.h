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
          outlierDeltaFactor_{outlierDeltaFactor},
          d_hist{cms::alpakatools::make_device_buffer<LayerTilesAlpaka<Acc1D>[]>(stream, NLAYERS)},
          d_seeds{cms::alpakatools::make_device_buffer<cms::alpakatools::VecArray<int, maxNSeeds>[]>(stream, 1)},
          d_followers{
              cms::alpakatools::make_device_buffer<cms::alpakatools::VecArray<int, maxNFollowers>[]>(stream, reserve)} {
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

    cms::alpakatools::device_buffer<Device, LayerTilesAlpaka<Acc1D>[]> d_hist;
    cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<int, maxNSeeds>[]> d_seeds;
    cms::alpakatools::device_buffer<Device, cms::alpakatools::VecArray<int, maxNFollowers>[]> d_followers;

    // private methods
    void init_device();

    void setup(PointsCloud const &host_pc);
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

#endif