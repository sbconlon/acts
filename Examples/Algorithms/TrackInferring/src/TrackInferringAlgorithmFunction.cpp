// This file is part of the Acts project.
// //
// // Copyright (C) 2021 CERN for the benefit of the Acts project
// //
// // This Source Code Form is subject to the terms of the Mozilla Public
// // License, v. 2.0. If a copy of the MPL was not distributed with this
// // file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/TrackInferring/TrackInferringAlgorithm.hpp"

#include <random>
#include <stdexcept>

namespace {


using GNN = Acts::GraphNeuralNetwork;

struct TrackInferrerFunctionImpl {

  GNN trackInferrer;

  TrackInferrerFunctionImpl(GNN&& f) : trackInferrer(std::move(f)) {}

  ActsExamples::TrackInferringAlgorithm::TrackInferrerResult operator()(
      const ActsExamples::IndexSpacePointsContainer& spacepoints,
      const ActsExamples::TrackInferringAlgorithm::TrackInferrerOptions& options)
      const {
    return trackInferrer.inferTracks(spacepoints, options);
  };
};

}  // namespace

ActsExamples::TrackInferringAlgorithm::TrackInfererFunction
ActsExamples::TrackInferringAlgorithm::makeTrackInfererFunction() {

  CKF trackFinder();

  // build the track inferrer functions. owns the infer track object.
  return TrackInferrerFunctionImpl(std::move(trackInferrer));
}
