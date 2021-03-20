// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Geometry/TrackingGeometry.hpp"
//#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "ActsExamples/EventData/Measurement.hpp"
#include "ActsExamples/EventData/Track.hpp"
#include "ActsExamples/Framework/BareAlgorithm.hpp"
#include "ActsExamples/MagneticField/MagneticField.hpp"

#include <functional>
#include <vector>

namespace ActsExamples {

class TrackInferringAlgorithm final : public BareAlgorithm {
 public:
  /// Track inferrer function that takes input spacepoints and options and 
  /// returns some track-finder-specific result.
  using TrackInferrerOptions =
      Acts::GraphNeuralNetworkOptions<MeasurementCalibrator,
                        Acts::MeasurementSelector>;

  using TrackInferrerResult = std::vector<
      Acts::Result<Acts::GraphNeuralNetworkResult<IndexSpacePoint>>>;
  
  using TrackInferrerFunction = std::function<TrackInferrerResult(
      const IndexSpacePointContainer&, const TrackInferrerOptions&)>;

  /// Create the track inferrer function implementation.
  static TrackInferrerFunction makeTrackInferrerFunction();

  struct Config {
    /// Input space points collection.
    std::string inputSpacePoints;
    /// Output find trajectories collection.
    std::string outputTrajectories;
    /// Type erased track inferer function.
    TrackInferrerFunction inferTracks;
  };

  /// Constructor of the track inferrer algorithm
  ///
  /// @param cfg is the config struct to configure the algorithm
  /// @param level is the logging level
  TrackInferringAlgorithm(Config cfg, Acts::Logging::Level lvl);

  /// Framework execute method of the track inferrer algorithm
  ///
  /// @param ctx is the algorithm context that holds event-wise information
  /// @return a process code to steer the algorithm flow
  ActsExamples::ProcessCode execute(
      const ActsExamples::AlgorithmContext& ctx) const final;

 private:
  Config m_cfg;
};

}  // namespace ActsExamples
