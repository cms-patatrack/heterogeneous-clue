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
    if (runForMinutes_ < 0 and maxEvents_ < 0) {
      maxEvents_ = 10;
    }
    std::string input(inputFile);
    bool repeat_layers = false;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i != maxEvents_; i++) {
      if (input.find("layers") != std::string::npos) {
        cloud_.emplace_back(readRaw(inputFile));
      } else {
        cloud_.emplace_back(readRawLayers(inputFile));
        repeat_layers = true;
      }
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto time = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(diff).count()) / 1e6;

    if (repeat_layers) {
      std::cout << "Emplacing from binary, layers repeated\n";
    } else {
      std::cout << "Emplacing from binary, no data repetition\n";
    }

    std::cout << "Read data for " << maxEvents_ << " events in " << std::scientific << time
              << " seconds, maximum throughput " << std::defaultfloat << (maxEvents_ / time) << " events/s"
              << std::endl;
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