// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

namespace ActsExamples {
class IBaseDetector;
}

/// The options for running GNN reco
///
/// @param desc The options description to add options to
void addRecGNNOptions(ActsExamples::Options::Description& desc);

/// Main function for running CKF reco with a specific detector.
///
/// @param argc number of command line arguments
/// @param argv command line arguments
/// @param detector is the detector to be used
int runRecGNNTracks(int argc, char* argv[],
                    std::shared_ptr<ActsExamples::IBaseDetector> detector);
