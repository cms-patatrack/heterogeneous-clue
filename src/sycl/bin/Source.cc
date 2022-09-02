#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "Source.h"

struct cloudRaw {
  float x;
  float y;
  float layer;
  float weight;
};

namespace {

  PointsCloud readRaw(std::filesystem::path const &inputFile) {
    PointsCloud data;
    std::ifstream in(inputFile, std::ios::binary);
    while (true) {
      cloudRaw raw;
      in.read((char *)&raw, sizeof(cloudRaw));
      if (in.eof())
        break;
      data.x.emplace_back(raw.x);
      data.y.emplace_back(raw.y);
      data.layer.emplace_back(raw.layer);
      data.weight.emplace_back(raw.weight);
    }
    in.close();
    data.n = data.x.size();
    return data;
  }

  PointsCloud readRawLayers(std::filesystem::path const &inputFile) {
    PointsCloud data;
    for (int l = 0; l < NLAYERS; l++) {
      std::ifstream in(inputFile, std::ios::binary);
      while (true) {
        cloudRaw raw;
        in.read((char *)&raw, sizeof(cloudRaw));
        if (in.eof()) {
          break;
        }
        data.x.emplace_back(raw.x);
        data.y.emplace_back(raw.y);
        data.layer.emplace_back(raw.layer + l);
        data.weight.emplace_back(raw.weight);
      }
      in.close();
    }
    data.n = data.x.size();
    return data;
  }
}  // namespace

namespace edm {
  Source::Source(int maxEvents, int runForMinutes, ProductRegistry &reg, std::filesystem::path const &inputFile)
      : maxEvents_(maxEvents), runForMinutes_(runForMinutes), cloudToken_(reg.produces<PointsCloud>()) {
    std::string input(inputFile);
    if (input.find("layers") != std::string::npos) {
      cloud_.emplace_back(readRaw(inputFile));
    } else {
      cloud_.emplace_back(readRawLayers(inputFile));
    }

    if (runForMinutes_ < 0 and maxEvents_ < 0) {
      maxEvents_ = 10;
    }
  }

  void Source::startProcessing() {
    if (runForMinutes_ >= 0) {
      startTime_ = std::chrono::steady_clock::now();
    }
  }

  std::unique_ptr<Event> Source::produce(int streamId, ProductRegistry const &reg) {
    if (shouldStop_) {
      return nullptr;
    }

    const int old = numEvents_.fetch_add(1);
    const int iev = old + 1;
    if (runForMinutes_ < 0) {
      if (old >= maxEvents_) {
        shouldStop_ = true;
        --numEvents_;
        return nullptr;
      }
    } else {
      if (numEvents_ - numEventsTimeLastCheck_ > static_cast<int>(cloud_.size())) {
        std::scoped_lock lock(timeMutex_);
        // if some other thread beat us, no need to do anything
        if (numEvents_ - numEventsTimeLastCheck_ > static_cast<int>(cloud_.size())) {
          auto processingTime = std::chrono::steady_clock::now() - startTime_;
          if (std::chrono::duration_cast<std::chrono::minutes>(processingTime).count() >= runForMinutes_) {
            shouldStop_ = true;
          }
          numEventsTimeLastCheck_ = (numEvents_ / cloud_.size()) * cloud_.size();
        }
        if (shouldStop_) {
          --numEvents_;
          return nullptr;
        }
      }
    }
    auto ev = std::make_unique<Event>(streamId, iev, reg);
    const int index = old % cloud_.size();

    ev->emplace(cloudToken_, cloud_[index]);

    return ev;
  }
}  // namespace edm