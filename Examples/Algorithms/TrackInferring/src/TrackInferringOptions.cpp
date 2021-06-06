// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/TrackInferring/TrackInferringOptions.hpp"

#include <string>

#include <boost/program_options.hpp>

void ActsExamples::Options::addTrackInferringOptions(
    ActsExamples::Options::Description& desc) {
 
  using boost::program_options::value;
  auto opt = desc.add_options();
  opt("ml-module-name", value<std::string>()->default_value("inference_fn"),
      "Name of ML Python module or file name to be imported");
  opt("ml-function-name", value<std::string>()->default_value("gnn_track_finding"),
      "Name of function in ML Python module to be imported and called");
  opt("ml-script-path", value<std::string>()->default_value(""),
      "Path to PyTorch script model");
}

ActsExamples::TrackInferringAlgorithm::Config
ActsExamples::Options::readTrackInferringConfig(
    const ActsExamples::Options::Variables& variables) {
  
  // Load inputs from variables
  auto moduleName = variables["ml-module-name"].template as<std::string>();
  auto funcName = variables["ml-function-name"].template as<std::string>();
  auto scriptPath = variables["ml-script-path"].template as<std::string>();  

  // Instantiate config
  TrackInferringAlgorithm::Config cfg;
  cfg.mlModuleName = moduleName;
  cfg.mlFuncName = funcName;
  cfg.mlScriptPath = scriptPath;
  
  return cfg;
}
