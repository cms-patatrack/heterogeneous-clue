#ifndef CLUEAlgoSYCL_h
#define CLUEAlgoSYCL_h

#include <optional>

#include <CL/sycl.hpp>

#include "SYCLCore/device_unique_ptr.h"

#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "SYCLDataFormats/LayerTilesSYCL.h"

class CLUEAlgoSYCL {
public:
  CLUEAlgoSYCL(float const &dc,
               float const &rhoc,
               float const &outlierDeltaFactor,
               sycl::queue const &stream,
               int const &numberOfPoints);

  ~CLUEAlgoSYCL() = default;

  void makeClusters(PointsCloud const &host_pc);

  PointsCloudSYCL d_points;

private:
  float dc_;
  float rhoc_;
  float outlierDeltaFactor_;
  std::optional<sycl::queue> queue_;

  cms::sycltools::device::unique_ptr<LayerTilesSYCL[]> d_hist;
  cms::sycltools::device::unique_ptr<cms::sycltools::VecArray<int, maxNSeeds>> d_seeds;
  cms::sycltools::device::unique_ptr<cms::sycltools::VecArray<int, maxNFollowers>[]> d_followers;

  void setup(PointsCloud const &host_pc);
};

#endif