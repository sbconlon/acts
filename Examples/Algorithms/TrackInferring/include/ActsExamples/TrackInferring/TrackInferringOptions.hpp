// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Utilities/Logger.hpp"
#include "ActsExamples/TrackInferring/TrackInferringAlgorithm.hpp"
#include "ActsExamples/Utilities/OptionsFwd.hpp"

namespace ActsExamples {
namespace Options {

/// Add TrackInferring options.
///
/// @param desc The options description to add options to
void addTrackInferringOptions(Description& desc);

/// Read TrackInferring options to create the algorithm config.
///
/// @param variables The variables to read from
TrackInferringAlgorithm::Config readTrackInferringConfig(
    const Variables& variables);

}  // namespace Options
}  // namespace ActsExamples
