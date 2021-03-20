// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/TrackInferring/TrackInferringAlgorithm.hpp"

#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "ActsExamples/EventData/Track.hpp"
#include "ActsExamples/EventData/Trajectories.hpp"
#include "ActsExamples/Framework/WhiteBoard.hpp"

#include <stdexcept>

ActsExamples::TrackInferringAlgorithm::TrackInferringAlgorithm(
    Config cfg, Acts::Logging::Level level)
    : ActsExamples::BareAlgorithm("TrackInferringAlgorithm", level),
      m_cfg(std::move(cfg)) {
  if (m_cfg.inputSpacePoints.empty()) {
    throw std::invalid_argument("Missing space points input collection");
  }
  if (m_cfg.outputTrajectories.empty()) {
    throw std::invalid_argument("Missing trajectories output collection");
  }
}

ActsExamples::ProcessCode ActsExamples::TrackInferringAlgorithm::execute(
    const ActsExamples::AlgorithmContext& ctx) const {
  // Read input data
  const auto& spacePoints =
      ctx.eventStore.get<SpacePointContainer>(m_cfg.inputSpacePoints);

  // Prepare the output data with MultiTrajectory
  TrajectoriesContainer trajectories;
  //trajectories.reserve(initialParameters.size());

  // Set the graph nueral network options
  ActsExamples::TrackFindingAlgorithm::TrackInferOptions options();

  // Perform the track inferring for all input space points
  ACTS_DEBUG("Invoke track inferring with " << spacePoints.size()
                                          << " space points.");
  auto results = m_cfg.inferTracks(spacePoints, options);

  // Loop over the track inferring results for all initial space points
  trajectories.reserve(results.size());
  for (std::size_t itrack = 0; itrack < results.size(); ++itrack) {
    // The result for this seed
    auto& result = results[itrack];
    if (result.ok()) {
      // Get the track inferring output object
      const auto& trackFindingOutput = result.value();
      // Create a Trajectories result struct
      trajectories.emplace_back(std::move(trackFindingOutput.fittedStates),
                                std::move(trackFindingOutput.trackTips),
                                std::move(trackFindingOutput.fittedParameters));
    } else {
      ACTS_WARNING("Track inferring failed for track " << itrack << " with error"
                                                       << result.error());
      // Track inferring failed. Add an empty result so the output container has
      // the same number of entries as the input.
      trajectories.push_back(Trajectories());
    }
  }

  ctx.eventStore.add(m_cfg.outputTrajectories, std::move(trajectories));
  return ActsExamples::ProcessCode::SUCCESS;
}
