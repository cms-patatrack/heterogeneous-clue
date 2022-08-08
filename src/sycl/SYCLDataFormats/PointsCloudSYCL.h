#ifndef POINTS_CLOUD_SYCL_H
#define POINTS_CLOUD_SYCL_H

#include "SYCLCore/device_unique_ptr.h"

class PointsCloudSYCL {
public:
  PointsCloudSYCL() = default;
  PointsCloudSYCL(sycl::queue stream);
  PointsCloudSYCL(PointsCloudSYCL const &) = delete;
  PointsCloudSYCL(PointsCloudSYCL &&) = default;
  PointsCloudSYCL &operator=(PointsCloudSYCL const &) = delete;
  PointsCloudSYCL &operator=(PointsCloudSYCL &&) = default;

  ~PointsCloudSYCL() = default;

  std::unique_ptr<float[], cms::sycltools::device::impl::DeviceDeleter> x;
  std::unique_ptr<float[], cms::sycltools::device::impl::DeviceDeleter> y;
  std::unique_ptr<int[], cms::sycltools::device::impl::DeviceDeleter> layer;
  std::unique_ptr<float[], cms::sycltools::device::impl::DeviceDeleter> weight;

  std::unique_ptr<float[], cms::sycltools::device::impl::DeviceDeleter> rho;
  std::unique_ptr<float[], cms::sycltools::device::impl::DeviceDeleter> delta;
  std::unique_ptr<int[], cms::sycltools::device::impl::DeviceDeleter> nearestHigher;
  std::unique_ptr<int[], cms::sycltools::device::impl::DeviceDeleter> clusterIndex;
  std::unique_ptr<int[], cms::sycltools::device::impl::DeviceDeleter> isSeed;
  int n;
};

PointsCloudSYCL::PointsCloudSYCL(sycl::queue stream) {
  unsigned int reserve = 1000000;

  //input variables
  x = cms::sycltools::make_device_unique<float[]>(reserve, stream);
  y = cms::sycltools::make_device_unique<float[]>(reserve, stream);
  layer = cms::sycltools::make_device_unique<int[]>(reserve, stream);
  weight = cms::sycltools::make_device_unique<float[]>(reserve, stream);
  //result variables
  rho = cms::sycltools::make_device_unique<float[]>(reserve, stream);
  delta = cms::sycltools::make_device_unique<float[]>(reserve, stream);
  nearestHigher = cms::sycltools::make_device_unique<int[]>(reserve, stream);
  clusterIndex = cms::sycltools::make_device_unique<int[]>(reserve, stream);
  isSeed = cms::sycltools::make_device_unique<int[]>(reserve, stream);
}

#endif