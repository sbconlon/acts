// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/TrackInferring/TrackInferringAlgorithm.hpp"

#include "ActsExamples/EventData/ProtoTrack.hpp"
#include "ActsExamples/Framework/WhiteBoard.hpp"

#include <stdexcept>

ActsExamples::TrackInferringAlgorithm::TrackInferringAlgorithm(
    Config cfg, Acts::Logging::Level level)
    : ActsExamples::BareAlgorithm("TrackInferringAlgorithm", level),
      m_cfg(std::move(cfg)) {
  if (m_cfg.inputSpacePoints.empty()) {
    throw std::invalid_argument("Missing space points input collection");
  }
  if (m_cfg.outputProtoTracks.empty()) {
    throw std::invalid_argument("Missing trajectories output collection");
  }
}

ActsExamples::ProcessCode ActsExamples::TrackInferringAlgorithm::execute(
    const ActsExamples::AlgorithmContext& ctx) const {
  // Read input data
  const auto& spacepoints =
    ctx.eventStore.get<SimSpacePointContainer>(m_cfg.inputSpacePoints);

  // Prepare the output data with ProtoTracks
  ProtoTrackContainer protoTracks;

  // Set the graph nueral network options
  ActsExamples::TrackInferringAlgorithm::TrackInferrerOptions options(m_cfg.mlModuleName, m_cfg.mlFuncName);

  // Perform the track inferring for all input space points
  ACTS_DEBUG("Invoke track inferring with " << spacepoints.size()
                                          << " space points.");
  auto results = m_cfg.inferTracks(spacepoints, options);

  // Loop over the track inferring results for all output tracks
  protoTracks.reserve(results.size());
  for (std::size_t itrack = 0; itrack < results.size(); ++itrack) {
    // The result for this seed
    auto& result = results[itrack];
    if (result.ok()) {
      // Get the track inferring output object
      const auto& trackOutput = &(result.value().spTrack);
      auto protoTrack = ProtoTrack();
      std::cout << "Track #" << itrack << ": [";
      for(std::size_t spi=0; spi<trackOutput->size(); spi++){
        auto idx = trackOutput[0][spi];
        std::cout << " " << idx;
        protoTrack.push_back(trackOutput[0][spi]);
      }
      std::cout << "]" << std::endl; 
      // Add newly created prototrack to prototracks
      protoTracks.push_back(protoTrack);

    } else {
      ACTS_WARNING("Track inferring failed for track " << itrack << " with error"
                                                       << result.error());
      // Track inferring failed. Add an empty result so the output container has
      // the same number of entries as the input.
      protoTracks.push_back(ProtoTrack());
    }
  }

  ctx.eventStore.add(m_cfg.outputProtoTracks, std::move(protoTracks));
  return ActsExamples::ProcessCode::SUCCESS;
}
