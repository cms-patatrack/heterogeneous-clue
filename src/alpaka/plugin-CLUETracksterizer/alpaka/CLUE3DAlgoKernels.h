#ifndef CLUE3DAlgo_Alpaka_Kernels_h
#define CLUE3DAlgo_Alpaka_Kernels_h

#include "DataFormats/Math/deltaR.h"
#include "DataFormats/Math/deltaPhi.h"

#include "AlpakaDataFormats/TICLLayerTilesAlpaka.h"
#include "AlpakaDataFormats/alpaka/ClusterCollectionAlpaka.h"

// #include <assert.h>

namespace ALPAKA_ACCELERATOR_NAMESPACE {

  using pointsView = ClusterCollectionAlpaka::ClusterCollectionAlpakaView;

  struct KernelComputeHistogram {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, TICLLayerTilesAlpaka *d_hist, pointsView *d_points) const {
      // push index of points into tiles
      // auto numberOfPoiints =
      // cms::alpakatools::for_each_element_in_grid(acc, numberOfPoints, [&](uint32_t i) {
      //   d_hist[d_points->layer[i]].fill(acc, d_points->x[i], d_points->y[i], i);
      // });
    }
  };

  struct KernelCalculateDensity {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, TICLLayerTilesAlpaka *d_hist, pointsView *d_points) const {
      // cms::alpakatools::for_each_element_in_grid(acc, numberOfPoints, [&](uint32_t i) {
      //   double rhoi{0.};
      //   int layeri = d_points->layer[i];
      //   float xi = d_points->x[i];
      //   float yi = d_points->y[i];
      //   //get search box
      //   int4 search_box = d_hist[layeri].searchBox(xi - dc, xi + dc, yi - dc, yi + dc);
      //   //loop over bins in the search box
      //   for (int xBin = search_box.x; xBin < search_box.y + 1; xBin++) {
      //     for (int yBin = search_box.z; yBin < search_box.w + 1; yBin++) {
      //       //get the id of this bin
      //       int binId = d_hist[layeri].getGlobalBinByBin(xBin, yBin);
      //       //get the size of this bin
      //       int binSize = d_hist[layeri][binId].size();
      //       //iterate inside this bin
      //       for (int binIter = 0; binIter < binSize; binIter++) {
      //         uint32_t j = d_hist[layeri][binId][binIter];
      //         float xj = d_points->x[j];
      //         float yj = d_points->y[j];
      //         float dist_ij = std::sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
      //         if (dist_ij <= dc) {
      //           rhoi += (i == j ? 1.f : 0.5f) * d_points->weight[j];
      //         }
      //       }  // end of iterate inside this bin
      //     }
      //   }  // end of loop over bins in search box
      //   d_points->rho[i] = rhoi;
      // });
    }
  };

  struct KernelComputeDistanceToHigher {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc, TICLLayerTilesAlpaka *d_hist, pointsView *d_points) const {
      // float dm = outlierDeltaFactor * dc;
      // cms::alpakatools::for_each_element_in_grid(acc, numberOfPoints, [&](uint32_t i) {
      //   int layeri = d_points->layer[i];
      //   float deltai = std::numeric_limits<float>::max();
      //   int nearestHigheri = -1;
      //   float xi = d_points->x[i];
      //   float yi = d_points->y[i];
      //   float rhoi = d_points->rho[i];
      //   // get search box
      //   int4 search_box = d_hist[layeri].searchBox(xi - dm, xi + dm, yi - dm, yi + dm);
      //   // loop over all bins in the search box
      //   for (int xBin = search_box.x; xBin < search_box.y + 1; xBin++) {
      //     for (int yBin = search_box.z; yBin < search_box.w + 1; yBin++) {
      //       // get the id of this bin
      //       int binId = d_hist[layeri].getGlobalBinByBin(xBin, yBin);
      //       // get the size of this bin
      //       int binSize = d_hist[layeri][binId].size();
      //       //iterate inside this bin
      //       for (int binIter = 0; binIter < binSize; binIter++) {
      //         uint32_t j = d_hist[layeri][binId][binIter];
      //         // query N'_{dm}(i)
      //         float xj = d_points->x[j];
      //         float yj = d_points->y[j];
      //         float dist_ij = std::sqrt((xi - xj) * (xi - xj) + (yi - yj) * (yi - yj));
      //         bool foundHigher = (d_points->rho[j] > rhoi);
      //         // in the rare case where rho is the same, use detid
      //         foundHigher = foundHigher || ((d_points->rho[j] == rhoi) && (j > i));
      //         if (foundHigher && dist_ij <= dm) {
      //           // definitio of N'_{dm}(i)
      //           // find the nearest point within N'_{dm}(i)
      //           if (dist_ij < deltai) {
      //             // update deltai and nearestHigheri
      //             deltai = dist_ij;
      //             nearestHigheri = j;
      //           }
      //         }
      //       }  // end of iterate inside this bin
      //     }
      //   }  // end of loop over bins in search box
      //   d_points->delta[i] = deltai;
      //   d_points->nearestHigher[i] = nearestHigheri;
      // });
    }
  };

  struct KernelFindClusters {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc,
                                  cms::alpakatools::VecArray<int, ticl::maxNSeeds> *d_seeds,
                                  cms::alpakatools::VecArray<std::pair<int, int>, ticl::maxNFollowers> *d_followers,
                                  pointsView *d_points) const {
      // cms::alpakatools::for_each_element_in_grid(acc, numberOfPoints, [&](uint32_t i) {
      //   // initialize clusterIndex
      //   d_points->clusterIndex[i] = -1;
      //   // determine seed or outlier
      //   float deltai = d_points->delta[i];
      //   float rhoi = d_points->rho[i];
      //   bool isSeed = (deltai > dc) && (rhoi >= rhoc);
      //   bool isOutlier = (deltai > outlierDeltaFactor * dc) && (rhoi < rhoc);

      //   if (isSeed) {
      //     // set isSeed as 1
      //     d_points->isSeed[i] = 1;
      //     d_seeds[0].push_back(acc, i);  // head of d_seeds
      //   } else {
      //     if (!isOutlier) {
      //       assert(d_points->nearestHigher[i] < static_cast<int>(numberOfPoints));
      //       // register as follower of its nearest higher
      //       d_followers[d_points->nearestHigher[i]].push_back(acc, i);
      //     }
      //   }
      // });
    }
  };

  struct KernelAssignClusters {
    template <typename TAcc>
    ALPAKA_FN_ACC void operator()(const TAcc &acc,
                                  cms::alpakatools::VecArray<int, ticl::maxNSeeds> *d_seeds,
                                  cms::alpakatools::VecArray<std::pair<int, int>, ticl::maxNFollowers> *d_followers,
                                  pointsView *d_points) const {
      // const auto &seeds = d_seeds[0];
      // const auto nSeeds = seeds.size();
      // cms::alpakatools::for_each_element_in_grid(acc, nSeeds, [&](uint32_t idxCls) {
      //   int localStack[localStackSizePerSeed] = {-1};
      //   int localStackSize = 0;

      //   // assign cluster to seed[idxCls]
      //   int idxThisSeed = seeds[idxCls];
      //   d_points->clusterIndex[idxThisSeed] = idxCls;
      //   // push_back idThisSeed to localStack
      //   assert((localStackSize < localStackSizePerSeed));

      //   localStack[localStackSize] = idxThisSeed;
      //   localStackSize++;
      //   // process all elements in localStack
      //   while (localStackSize > 0) {
      //     // get last element of localStack
      //     assert((localStackSize - 1 < localStackSizePerSeed));
      //     int idxEndOfLocalStack = localStack[localStackSize - 1];
      //     int temp_clusterIndex = d_points->clusterIndex[idxEndOfLocalStack];
      //     // pop_back last element of localStack
      //     assert((localStackSize - 1 < localStackSizePerSeed));
      //     localStack[localStackSize - 1] = -1;
      //     localStackSize--;

      //     // loop over followers of last element of localStack
      //     for (int j : d_followers[idxEndOfLocalStack]) {
      //       // pass id to follower
      //       d_points->clusterIndex[j] = temp_clusterIndex;
      //       // push_back follower to localStack
      //       assert((localStackSize < localStackSizePerSeed));
      //       localStack[localStackSize] = j;
      //       localStackSize++;
      //     }
      //   }
      // });
    }
  };
}  // namespace ALPAKA_ACCELERATOR_NAMESPACE

// void KernelComputeHistogram(TICLLayerTiles &d_hist,
//     ClusterCollectionSerial &points) {
//   for (unsigned int clusterIdxSoA = 0; clusterIdxSoA < points.x.size(); clusterIdxSoA++) {
//       d_histSoA.fill(points.layer[clusterIdxSoA],
//           std::abs(points.eta[clusterIdxSoA]),
//           points.phi[clusterIdxSoA],
//           clusterIdxSoA);
//   }
// };

// void KernelCalculateDensity(TICLLayerTiles &d_hist,
//     ClusterCollectionSerial &points,
//     int algoVerbosity = 0,
//     int densitySiblingLayers = 3,
//     int densityXYDistanceSqr = 3.24,
//     float kernelDensityFactor = 0.2,
//     bool densityOnSameLayer = false
//     ) {

//   constexpr int nEtaBin = TICLLayerTiles::constants_type_t::nEtaBins;
//   constexpr int nPhiBin = TICLLayerTiles::constants_type_t::nPhiBins;
//   constexpr int nLayers = TICLLayerTiles::constants_type_t::nLayers;
//   for (unsigned int clusterIdxSoA = 0; clusterIdxSoA < points.x.size(); ++clusterIdxSoA) {
//     int layerId = points.layer[clusterIdxSoA];

//     auto isReachable = [](float r0, float r1, float phi0, float phi1, float delta_sqr) -> bool {
//       // TODO(rovere): import reco::deltaPhi implementation as well
//       auto delta_phi = reco::deltaPhi(phi0, phi1);
//       return (r0 - r1) * (r0 - r1) + r1 * r1 * delta_phi * delta_phi < delta_sqr;
//     };
//     auto distance_debug = [&](float x1, float x2, float y1, float y2) -> float {
//       return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
//     };

//     // We need to partition the two sides of the HGCAL detector
//     constexpr int lastLayerPerSide = nLayers / 2;
//     int maxLayer = 2 * lastLayerPerSide - 1;
//     int minLayer = 0;
//     if (layerId < lastLayerPerSide) {
//       minLayer = std::max(layerId - densitySiblingLayers, minLayer);
//       maxLayer = std::min(layerId + densitySiblingLayers, lastLayerPerSide - 1);
//     } else {
//       minLayer = std::max(layerId - densitySiblingLayers, lastLayerPerSide);
//       maxLayer = std::min(layerId + densitySiblingLayers, maxLayer);
//     }

//     for (int currentLayer = minLayer; currentLayer <= maxLayer; currentLayer++) {
//       if (algoVerbosity > 0) {
//         std::cout << "RefLayer: " << layerId << " SoaIDX: " << clusterIdxSoA;
//         std::cout << "NextLayer: " << currentLayer;
//       }
//       const auto &tileOnLayer = d_hist[currentLayer];
//       bool onSameLayer = (currentLayer == layerId);
//       if (algoVerbosity > 0) {
//         std::cout << "onSameLayer: " << onSameLayer;
//       }
//       const int etaWindow = 2;
//       const int phiWindow = 2;
//       int etaBinMin = std::max(tileOnLayer.etaBin(points.eta[clusterIdxSoA]) - etaWindow, 0);
//       int etaBinMax = std::min(tileOnLayer.etaBin(points.eta[clusterIdxSoA]) + etaWindow, nEtaBin);
//       int phiBinMin = tileOnLayer.phiBin(points.phi[clusterIdxSoA]) - phiWindow;
//       int phiBinMax = tileOnLayer.phiBin(points.phi[clusterIdxSoA]) + phiWindow;
//       if (algoVerbosity > 0) {
//         std::cout << "eta: " << points.eta[clusterIdxSoA] << std::endl;
//         std::cout << "phi: " << points.phi[clusterIdxSoA] << std::endl;
//         std::cout << "etaBinMin: " << etaBinMin << ", etaBinMax: " << etaBinMax << std::endl;
//         std::cout << "phiBinMin: " << phiBinMin << ", phiBinMax: " << phiBinMax << std::endl;
//       }
//       for (int ieta = etaBinMin; ieta <= etaBinMax; ++ieta) {
//         auto offset = ieta * nPhiBin;
//         if (algoVerbosity > 0) {
//           std::cout << "offset: " << offset << std::endl;
//         }
//         for (int iphi_it = phiBinMin; iphi_it <= phiBinMax; ++iphi_it) {
//           int iphi = ((iphi_it % nPhiBin + nPhiBin) % nPhiBin);
//           if (algoVerbosity > 0) {
//             std::cout << "iphi: " << iphi << std::endl;
//             std::cout
//               << "Entries in tileBin: " << tileOnLayer[offset + iphi].size() << std::endl;
//           }
//           for (auto otherClusterIdx : tileOnLayer[offset + iphi]) {
//             //              auto const &clustersLayer = points[currentLayer];
//             if (algoVerbosity > 0) {
//               std::cout
//                 << "OtherLayer: " << currentLayer << " SoaIDX: " << otherClusterIdx << std::endl;
//               std::cout << "OtherEta: " << points.eta[otherClusterIdx] << std::endl;
//               std::cout << "OtherPhi: " << points.phi[otherClusterIdx] << std::endl;
//             }
//             bool reachable = false;
//             // Still differentiate between silicon and Scintillator.
//             // Silicon has yet to be studied further.
//             if (points.isSilicon[clusterIdxSoA]) {
//               reachable = isReachable(points.r_over_absz[clusterIdxSoA] * points.z[clusterIdxSoA],
//                   points.r_over_absz[otherClusterIdx] * points.z[clusterIdxSoA],
//                   points.phi[clusterIdxSoA],
//                   points.phi[otherClusterIdx],
//                   densityXYDistanceSqr);
//             } else {
//               reachable = isReachable(points.r_over_absz[clusterIdxSoA] * points.z[clusterIdxSoA],
//                   points.r_over_absz[otherClusterIdx] * points.z[clusterIdxSoA],
//                   points.phi[clusterIdxSoA],
//                   points.phi[otherClusterIdx],
//                   points.radius[clusterIdxSoA] * points.radius[clusterIdxSoA]);
//             }
//             if (algoVerbosity > 0) {
//               std::cout << "Distance[eta,phi]: "
//                 << reco::deltaR2(points.eta[clusterIdxSoA],
//                     points.phi[clusterIdxSoA],
//                     points.eta[otherClusterIdx],
//                     points.phi[otherClusterIdx]) << std::endl;
//               auto dist = distance_debug(
//                   points.r_over_absz[clusterIdxSoA],
//                   points.r_over_absz[otherClusterIdx],
//                   points.r_over_absz[clusterIdxSoA] * std::abs(points.phi[clusterIdxSoA]),
//                   points.r_over_absz[otherClusterIdx] * std::abs(points.phi[otherClusterIdx]));
//               std::cout << "Distance[cm]: " << (dist * points.z[clusterIdxSoA]) << std::endl;
//               std::cout
//                 << "Energy Other:   " << points.energy[otherClusterIdx] << std::endl;
//               std::cout << "Cluster radius: " << points.radius[clusterIdxSoA] << std::endl;
//             }
//             if (reachable) {
//               float factor_same_layer_different_cluster = (onSameLayer && !densityOnSameLayer) ? 0.f : 1.f;
//               auto energyToAdd = ((clusterIdxSoA == otherClusterIdx)
//                   ? 1.f
//                   : kernelDensityFactor * factor_same_layer_different_cluster) *
//                 points.energy[otherClusterIdx];
//               points.rho[clusterIdxSoA] += energyToAdd;
//               if (algoVerbosity > 0) {
//                 std::cout
//                   << "Adding " << energyToAdd << " partial " << points.rho[clusterIdxSoA] << std::endl;
//               }
//             }
//           }  // end of loop on possible compatible clusters
//         }    // end of loop over phi-bin region
//       }      // end of loop over eta-bin region
//     }        // end of loop on the sibling layers
//   }
// //  for (unsigned int i = 0; i < points.rho.size(); ++i) {
// //    std::cout << "Layer " << points.layer[i] << " i " << i << " rho " << points.rho[i] << std::endl;
// //  }
// }

// void KernelComputeDistanceToHigher(TICLLayerTiles &d_hist,
//     ClusterCollectionSerial &points,
//     int algoVerbosity = 0,
//     int densitySiblingLayers = 3,
//     bool nearestHigherOnSameLayer = false) {
//   constexpr int nEtaBin = TICLLayerTiles::constants_type_t::nEtaBins;
//   constexpr int nPhiBin = TICLLayerTiles::constants_type_t::nPhiBins;
//   constexpr int nLayers = TICLLayerTiles::constants_type_t::nLayers;
//   for (unsigned int clusterIdxSoA = 0; clusterIdxSoA < points.x.size(); ++clusterIdxSoA) {
//     int layerId = points.layer[clusterIdxSoA];

//     auto distanceSqr = [](float r0, float r1, float phi0, float phi1) -> float {
//       auto delta_phi = reco::deltaPhi(phi0, phi1);
//       return (r0 - r1) * (r0 - r1) + r1 * r1 * delta_phi * delta_phi;
//     };

//     if (algoVerbosity > 0) {
//       std::cout
//         << "Starting searching nearestHigher on " << layerId << " with rho: " << points.rho[clusterIdxSoA]
//         << " at eta, phi: " << d_hist[layerId].etaBin(points.eta[clusterIdxSoA]) << ", "
//         << d_hist[layerId].phiBin(points.phi[clusterIdxSoA]);
//     }
//     // We need to partition the two sides of the HGCAL detector
//     constexpr int lastLayerPerSide = nLayers / 2;
//     int minLayer = 0;
//     int maxLayer = 2 * lastLayerPerSide - 1;
//     if (layerId < lastLayerPerSide) {
//       minLayer = std::max(layerId - densitySiblingLayers, minLayer);
//       maxLayer = std::min(layerId + densitySiblingLayers, lastLayerPerSide - 1);
//     } else {
//       minLayer = std::max(layerId - densitySiblingLayers, lastLayerPerSide + 1);
//       maxLayer = std::min(layerId + densitySiblingLayers, maxLayer);
//     }
//     constexpr float maxDelta = std::numeric_limits<float>::max();
//     float i_delta = maxDelta;
//     std::pair<int, int> i_nearestHigher(-1, -1);
//     std::pair<float, int> nearest_distances(maxDelta, std::numeric_limits<int>::max());
//     for (int currentLayer = minLayer; currentLayer <= maxLayer; currentLayer++) {
//       if (!nearestHigherOnSameLayer && (layerId == currentLayer))
//         continue;
//       const auto &tileOnLayer = d_hist[currentLayer];
//       int etaWindow = 1;
//       int phiWindow = 1;
//       int etaBinMin = std::max(tileOnLayer.etaBin(points.eta[clusterIdxSoA]) - etaWindow, 0);
//       int etaBinMax = std::min(tileOnLayer.etaBin(points.eta[clusterIdxSoA]) + etaWindow, nEtaBin);
//       int phiBinMin = tileOnLayer.phiBin(points.phi[clusterIdxSoA]) - phiWindow;
//       int phiBinMax = tileOnLayer.phiBin(points.phi[clusterIdxSoA]) + phiWindow;
//       for (int ieta = etaBinMin; ieta <= etaBinMax; ++ieta) {
//         auto offset = ieta * nPhiBin;
//         for (int iphi_it = phiBinMin; iphi_it <= phiBinMax; ++iphi_it) {
//           int iphi = ((iphi_it % nPhiBin + nPhiBin) % nPhiBin);
//           if (algoVerbosity > 0) {
//             std::cout
//               << "Searching nearestHigher on " << currentLayer << " eta, phi: " << ieta << ", " << iphi_it << " "
//               << iphi << " " << offset << " " << (offset + iphi);
//           }
//           for (auto otherClusterIdx : tileOnLayer[offset + iphi]) {
//             //              auto const &clustersOnOtherLayer = points[currentLayer];
//             auto dist = maxDelta;
//             auto dist_transverse = maxDelta;
//             int dist_layers = std::abs(currentLayer - layerId);
//             dist_transverse = distanceSqr(points.r_over_absz[clusterIdxSoA] * points.z[clusterIdxSoA],
//                 points.r_over_absz[otherClusterIdx] * points.z[clusterIdxSoA],
//                 points.phi[clusterIdxSoA],
//                 points.phi[otherClusterIdx]);
//             // Add Z-scale to the final distance
//             dist = dist_transverse;
//             // TODO(rovere): in case of equal local density, the ordering in
//             // the original CLUE3D implementaiton is bsaed on the index of
//             // the LayerCclusters in the LayerClusterCollection. In this
//             // case, the index is based on the ordering of the SOA indices.
//             bool foundHigher = (points.rho[otherClusterIdx] >
//                 points.rho[clusterIdxSoA]) ||
//               (points.rho[otherClusterIdx] == points.rho[clusterIdxSoA] &&
//                otherClusterIdx > clusterIdxSoA);
//             if (algoVerbosity > 0) {
//               std::cout
//                 << "Searching nearestHigher on " << currentLayer
//                 << " with rho: " << points.rho[otherClusterIdx]
//                 << " on layerIdxInSOA: " << currentLayer << ", " << otherClusterIdx
//                 << " with distance: " << sqrt(dist) << " foundHigher: " << foundHigher;
//             }
//             if (foundHigher && dist <= i_delta) {
//               // update i_delta
//               i_delta = dist;
//               nearest_distances = std::make_pair(sqrt(dist_transverse), dist_layers);
//               // update i_nearestHigher
//               i_nearestHigher = std::make_pair(currentLayer, otherClusterIdx);
//             }
//           }  // End of loop on clusters
//         }    // End of loop on phi bins
//       }      // End of loop on eta bins
//     }        // End of loop on layers

//     bool foundNearestInFiducialVolume = (i_delta != maxDelta);
//     if (algoVerbosity > 0) {
//       std::cout
//         << "i_delta: " << i_delta << " passed: " << foundNearestInFiducialVolume << " " << i_nearestHigher.first
//         << " " << i_nearestHigher.second << " distances: " << nearest_distances.first << ", "
//         << nearest_distances.second;
//     }
//     if (foundNearestInFiducialVolume) {
//       points.delta[clusterIdxSoA] = nearest_distances;
//       points.nearestHigher[clusterIdxSoA] = i_nearestHigher;
//     } else {
//       // otherwise delta is guaranteed to be larger outlierDeltaFactor_*delta_c
//       // we can safely maximize delta to be maxDelta
//       points.delta[clusterIdxSoA] = std::make_pair(maxDelta, std::numeric_limits<int>::max());
//       points.nearestHigher[clusterIdxSoA] = {-1, -1};
//     }
//   }
// };

// int KernelFindAndAssignClusters(ClusterCollectionSerial &points,
//     int algoVerbosity = 0,
//     float criticalXYDistance = 1.8, // cm
//     float criticalZDistanceLyr = 5,
//     float criticalDensity = 0.6, // GeV
//     float criticalSelfDensity = 0.15,
//     float outlierMultiplier = 2.) {
//   unsigned int nTracksters = 0;

//   std::vector<std::pair<int, int>> localStack;
//   auto critical_transverse_distance = criticalXYDistance;
//   // find cluster seeds and outlier
//   for (unsigned int clusterIdxSoA = 0; clusterIdxSoA < points.x.size(); ++clusterIdxSoA) {
//     int layerId = points.layer[clusterIdxSoA];
//     // initialize tracksterIndex
//     points.tracksterIndex[clusterIdxSoA] = -1;
//     bool isSeed = (points.delta[clusterIdxSoA].first > critical_transverse_distance ||
//         points.delta[clusterIdxSoA].second > criticalZDistanceLyr) &&
//       (points.rho[clusterIdxSoA] >= criticalDensity) &&
//       (points.energy[clusterIdxSoA] / points.rho[clusterIdxSoA] > criticalSelfDensity);
//     if (!points.isSilicon[clusterIdxSoA]) {
//       isSeed = (points.delta[clusterIdxSoA].first > points.radius[clusterIdxSoA] ||
//           points.delta[clusterIdxSoA].second > criticalZDistanceLyr) &&
//         (points.rho[clusterIdxSoA] >= criticalDensity) &&
//         (points.energy[clusterIdxSoA] / points.rho[clusterIdxSoA] > criticalSelfDensity);
//     }
//     bool isOutlier = (points.delta[clusterIdxSoA].first > outlierMultiplier * critical_transverse_distance) &&
//       (points.rho[clusterIdxSoA] < criticalDensity);
//     if (isSeed) {
//       if (algoVerbosity > 0) {
//         std::cout
//           << "Found seed on Layer " << layerId << " SOAidx: " << clusterIdxSoA << " assigned ClusterIdx: " << nTracksters;
//       }
//       points.tracksterIndex[clusterIdxSoA] = nTracksters++;
//       points.isSeed[clusterIdxSoA] = true;
//       localStack.emplace_back(layerId, clusterIdxSoA);
//     } else if (!isOutlier) {
//       auto [lyrIdx, soaIdx] = points.nearestHigher[clusterIdxSoA];
//       if (algoVerbosity > 0) {
//         std::cout
//           << "Found follower on Layer " << layerId << " SOAidx: " << clusterIdxSoA << " attached to cluster on layer: " << lyrIdx
//           << " SOAidx: " << soaIdx;
//       }
//       if (lyrIdx >= 0)
//         points.followers[soaIdx].emplace_back(layerId, clusterIdxSoA);
//     } else {
//       if (algoVerbosity > 0) {
//         std::cout
//           << "Found Outlier on Layer " << layerId << " SOAidx: " << clusterIdxSoA << " with rho: " << points.rho[clusterIdxSoA]
//           << " and delta: " << points.delta[clusterIdxSoA].first << ", " << points.delta[clusterIdxSoA].second;
//       }
//     }
//   }

//   std::cout << "Number of Tracksters: " << nTracksters << std::endl;
//   // Propagate cluster index
//   while (!localStack.empty()) {
//     auto [lyrIdx, soaIdx] = localStack.back();
//     auto &thisSeed = points.followers[soaIdx];
//     localStack.pop_back();

//     // loop over followers
//     for (auto [follower_lyrIdx, follower_soaIdx] : thisSeed) {
//       // pass id to a follower
//       points.tracksterIndex[follower_soaIdx] = points.tracksterIndex[soaIdx];
//       // push this follower to localStack
//       localStack.emplace_back(follower_lyrIdx, follower_soaIdx);
//     }
//   }

//   return nTracksters;
// };

#endif
