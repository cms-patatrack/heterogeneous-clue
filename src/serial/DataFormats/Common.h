#ifndef DataFormats_HGCalReco_Common_h
#define DataFormats_HGCalReco_Common_h

#include <vector>
#include <array>
#include <cstdint>

namespace ticl {
  struct TileConstants {
    static constexpr float minEta = 1.5f;
    static constexpr float maxEta = 3.2f;
    static constexpr int nEtaBins = 34;
    static constexpr int nPhiBins = 126;
    static constexpr int nLayers = 94;
    static constexpr int nBins = nEtaBins * nPhiBins;
  };
}  // namespace ticl

#endif  // DataFormats_HGCalReco_Common_h
