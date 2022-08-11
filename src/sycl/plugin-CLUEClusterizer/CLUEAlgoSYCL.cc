#include <CL/sycl.hpp>
#include "CLUEAlgoSYCL.h"
#include "CLUEAlgoKernels.h"

CLUEAlgoSYCL::CLUEAlgoSYCL(float dc, float rhoc, float outlierDeltaFactor, sycl::queue stream)
    : dc_{dc}, rhoc_{rhoc}, outlierDeltaFactor_{outlierDeltaFactor}, q_{stream} {
  unsigned int reserve = 1000000;
  //allocate memory
  d_hist = cms::sycltools::make_device_shared_uninitialized<LayerTilesSYCL[]>(NLAYERS, stream);
  d_seeds = cms::sycltools::make_device_shared_uninitialized<sycltools::VecArray<int, maxNSeeds>>(stream);
  d_followers =
      cms::sycltools::make_device_shared_uninitialized<sycltools::VecArray<int, maxNFollowers>[]>(reserve, stream);
  //clear set
  stream.memset(d_hist.get(), 0x00, sizeof(LayerTilesSYCL) * NLAYERS);
  stream.memset(d_seeds.get(), 0x00, sizeof(sycltools::VecArray<int, maxNSeeds>));
  stream.memset(d_followers.get(), 0x00, sizeof(sycltools::VecArray<int, maxNFollowers>) * reserve).wait();
}

using view = PointsCloudSYCL::PointsCloudSYCLView;

void CLUEAlgoSYCL::makeClusters(PointsCloudSYCL const& pc) {
  // calculate rho, delta and find seeds
  // 1 point per thread
  auto d_points = pc.view();
  const int numThreadsPerBlock = 256;  // ThreadsPerBlock = work-group size
  const sycl::range<3> blockSize(numThreadsPerBlock, 1, 1);
  const sycl::range<3> gridSize(ceil(pc.n / static_cast<float>(blockSize[0])), 1, 1);

  q_.submit([&](sycl::handler &cgh) {
    //SYCL kernels cannot capture by reference - need to reassign pointers inside the submit to pass by value
    auto d_hist_kernel = d_hist.get();
    cgh.parallel_for(sycl::nd_range<3>(gridSize * blockSize, blockSize),
                     [=](sycl::nd_item<3> item) { kernel_compute_histogram(d_hist_kernel, d_points, item); });
  });

  q_.submit([&](sycl::handler &cgh) {
    auto d_hist_kernel = d_hist.get();
    auto dc_kernel = dc_;
    cgh.parallel_for(sycl::nd_range<3>(gridSize * blockSize, blockSize), [=](sycl::nd_item<3> item) {
      kernel_calculate_density(d_hist_kernel, d_points, dc_kernel, item);
    });
  });

  q_.submit([&](sycl::handler &cgh) {
    auto d_hist_kernel = d_hist.get();
    auto outlierDeltaFactor_kernel = outlierDeltaFactor_;
    auto dc_kernel = dc_;
    cgh.parallel_for(sycl::nd_range<3>(gridSize * blockSize, blockSize), [=](sycl::nd_item<3> item) {
      kernel_calculate_distanceToHigher(d_hist_kernel, d_points, outlierDeltaFactor_kernel, dc_kernel, item);
    });
  });

  q_.submit([&](sycl::handler &cgh) {
    auto d_seeds_kernel = d_seeds.get();
    auto d_followers_kernel = d_followers.get();
    auto outlierDeltaFactor_kernel = outlierDeltaFactor_;
    auto dc_kernel = dc_;
    auto rhoc_kernel = rhoc_;
    cgh.parallel_for(sycl::nd_range<3>(gridSize * blockSize, blockSize), [=](sycl::nd_item<3> item) {
      kernel_find_clusters(
          d_seeds_kernel, d_followers_kernel, d_points, outlierDeltaFactor_kernel, dc_kernel, rhoc_kernel, item);
    });
  });

  // assign clusters
  // 1 point per seeds
  const sycl::range<3> gridSize_nseeds(ceil(maxNSeeds / static_cast<double>(blockSize[0])), 1, 1);

  q_.submit([&](sycl::handler &cgh) {
    auto d_seeds_kernel = d_seeds.get();
    auto d_followers_kernel = d_followers.get();
    cgh.parallel_for(sycl::nd_range<3>(gridSize_nseeds * blockSize, blockSize), [=](sycl::nd_item<3> item) {
      kernel_assign_clusters(d_seeds_kernel, d_followers_kernel, d_points, item);
    });
  })
  .wait();
}