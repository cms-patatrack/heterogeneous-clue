#ifndef POINTS_CLOUD_SYCL_H
#define POINTS_CLOUD_SYCL_H

#include <memory>

#include "SYCLCore/device_shared_ptr.h"

class ClusteredPointsCloudSYCL;

class PointsCloudSYCL {
public:
  PointsCloudSYCL() = default;
  PointsCloudSYCL(sycl::queue stream, int numberOfPoints) {
    unsigned int reserve = 1000000;

    //input variables
    x = cms::sycltools::make_device_shared<float[]>(reserve, stream);
    y = cms::sycltools::make_device_shared<float[]>(reserve, stream);
    layer = cms::sycltools::make_device_shared<int[]>(reserve, stream);
    weight = cms::sycltools::make_device_shared<float[]>(reserve, stream);
    //result variables
    rho = cms::sycltools::make_device_shared<float[]>(reserve, stream);
    delta = cms::sycltools::make_device_shared<float[]>(reserve, stream);
    nearestHigher = cms::sycltools::make_device_shared<int[]>(reserve, stream);
    clusterIndex = cms::sycltools::make_device_shared<int[]>(reserve, stream);
    isSeed = cms::sycltools::make_device_shared<int[]>(reserve, stream);
    n = numberOfPoints;

    auto view = std::make_shared<PointsCloudSYCLView>();
    view->x = x.get();
    view->y = y.get();
    view->layer = layer.get();
    view->weight = weight.get();
    view->rho = rho.get();
    view->delta = delta.get();
    view->nearestHigher = nearestHigher.get();
    view->clusterIndex = clusterIndex.get();
    view->isSeed = isSeed.get();
    view->n = numberOfPoints;

    view_d = cms::sycltools::make_device_shared<PointsCloudSYCLView>(stream);
    stream.memcpy(view_d.get(), view.get(), sizeof(PointsCloudSYCLView)).wait();
  }
  PointsCloudSYCL(PointsCloudSYCL const &pc) = default;
  PointsCloudSYCL(PointsCloudSYCL &&) = default;
  PointsCloudSYCL &operator=(PointsCloudSYCL const &) = default;
  PointsCloudSYCL &operator=(PointsCloudSYCL &&) = default;

  ~PointsCloudSYCL() = default;

  std::shared_ptr<float[]> x;
  std::shared_ptr<float[]> y;
  std::shared_ptr<int[]> layer;
  std::shared_ptr<float[]> weight;
  std::shared_ptr<float[]> rho;
  std::shared_ptr<float[]> delta;
  std::shared_ptr<int[]> nearestHigher;
  std::shared_ptr<int[]> clusterIndex;
  std::shared_ptr<int[]> isSeed;
  int n;

  class PointsCloudSYCLView {
  public:
    float *x;
    float *y;
    int *layer;
    float *weight;
    float *rho;
    float *delta;
    int *nearestHigher;
    int *clusterIndex;
    int *isSeed;
    int n;
  };

  PointsCloudSYCLView *view() const { return view_d.get(); }

private:
  friend class ClusteredPointsCloudSYCL;
  cms::sycltools::device::shared_ptr<PointsCloudSYCLView> view_d;
};
#endif