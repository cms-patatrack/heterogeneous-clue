#ifndef CLUSTERED_POINTS_CLOUD_SYCL_H
#define CLUSTERED_POINTS_CLOUD_SYCL_H
#include <memory>
#include "SYCLCore/device_shared_ptr.h"
#include "SYCLDataFormats/PointsCloudSYCL.h"

class ClusteredPointsCloudSYCL {
public:
    ClusteredPointsCloudSYCL(ClusteredPointsCloudSYCL&& cpc) = default;
    ClusteredPointsCloudSYCL(const PointsCloudSYCL& pc) : clusterdPointsData_(std::move(pc.view_d)) {}
    cms::sycltools::device::shared_ptr<PointsCloudSYCL::PointsCloudSYCLView> clusterdPointsData_;
};

#endif