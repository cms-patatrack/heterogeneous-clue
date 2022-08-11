#ifndef CLUEAlgoSYCL_h
#define CLUEAlgoSYCL_h

#include <optional>

#include <CL/sycl.hpp>

#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "SYCLDataFormats/LayerTilesSYCL.h"

class CLUEAlgoSYCL {
public:
  // constructor
  CLUEAlgoSYCL(float const &dc,
               float const &rhoc,
               float const &outlierDeltaFactor,
               sycl::queue const &stream,
               int const &numberOfPoints);

  ~CLUEAlgoSYCL();

  void makeClusters(PointsCloud const &host_pc);

  PointsCloudSYCL d_points;

private:
  float dc_;
  float rhoc_;
  float outlierDeltaFactor_;

  LayerTilesSYCL *d_hist;
  cms::sycltools::VecArray<int, maxNSeeds> *d_seeds;
  cms::sycltools::VecArray<int, maxNFollowers> *d_followers;

  std::optional<sycl::queue> queue_;

  // private methods
  void init_device();

  void free_device();

  void setup(PointsCloud const &host_pc);

  /*   void copy_tohost() {
    // result variables
    queue_.memcpy(points_.clusterIndex.data(), d_points.clusterIndex, sizeof(int) * points_.n).wait();

    if (verbose_)  // other variables, copy only when verbose_==True
    {
      queue_.memcpy(points_.rho.data(), d_points.rho, sizeof(float) * points_.n);
      queue_.memcpy(points_.delta.data(), d_points.delta, sizeof(float) * points_.n);
      queue_.memcpy(points_.nearestHigher.data(), d_points.nearestHigher, sizeof(int) * points_.n);
      queue_.memcpy(points_.isSeed.data(), d_points.isSeed, sizeof(int) * points_.n).wait();
    }
  } */
};

#endif