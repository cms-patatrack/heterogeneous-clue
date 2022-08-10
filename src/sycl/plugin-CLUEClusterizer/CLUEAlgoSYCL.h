#ifndef CLUEAlgoSYCL_h
#define CLUEAlgoSYCL_h

#include <memory>
#include <CL/sycl.hpp>
#include "DataFormats/LayerTilesSYCL.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"
#include "SYCLCore/device_shared_ptr.h"

class ClustersData {
public:
  ClustersData(PointsCloudSYCL&& pc) : data_{std::move(pc)} {}
  PointsCloudSYCL data_;
};

class CLUEAlgoSYCL {
public:
  CLUEAlgoSYCL(float dc, float rhoc, float outlierDeltaFactor, sycl::queue stream);

  ~CLUEAlgoSYCL() = default;

  using view = PointsCloudSYCL::PointsCloudSYCLView;
  view* makeClusters(PointsCloudSYCL const& pc);

private:
  float dc_;
  float rhoc_;
  float outlierDeltaFactor_;

  sycl::queue q_;

  std::shared_ptr<LayerTilesSYCL[]> d_hist;
  std::shared_ptr<sycltools::VecArray<int, maxNSeeds>> d_seeds;
  std::shared_ptr<sycltools::VecArray<int, maxNFollowers>[]> d_followers;
};

//void copy_tohost() {
//  // result variables
//  queue_.memcpy(points_.clusterIndex.data(), d_points.clusterIndex, sizeof(int) * points_.n).wait();
//
//  if (verbose_)  // other variables, copy only when verbose_==True
//  {
//    queue_.memcpy(points_.rho.data(), d_points.rho, sizeof(float) * points_.n);
//    queue_.memcpy(points_.delta.data(), d_points.delta, sizeof(float) * points_.n);
//    queue_.memcpy(points_.nearestHigher.data(), d_points.nearestHigher, sizeof(int) * points_.n);
//    queue_.memcpy(points_.isSeed.data(), d_points.isSeed, sizeof(int) * points_.n).wait();
//  }
//}

#endif