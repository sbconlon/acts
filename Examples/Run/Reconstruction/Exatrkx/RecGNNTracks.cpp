// This file is part of the Acts project.
//
// Copyright (C) 2019-2021 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/Detector/IBaseDetector.hpp"
#include "ActsExamples/Digitization/DigitizationOptions.hpp"

#include "ActsExamples/Framework/Sequencer.hpp"
#include "ActsExamples/Framework/WhiteBoard.hpp"

#include "ActsExamples/Geometry/CommonGeometry.hpp"

#include "ActsExamples/Io/Csv/CsvOptionsReader.hpp"
#include "ActsExamples/Io/Csv/CsvParticleReader.hpp"
#include "ActsExamples/Io/Csv/CsvSimHitReader.hpp"
#include "ActsExamples/Io/Performance/TrackFinderPerformanceWriter.hpp"
#include "ActsExamples/Io/Root/RootTrajectoryParametersWriter.hpp"
#include "ActsExamples/Io/Root/RootTrajectoryStatesWriter.hpp"

#include "ActsExamples/MagneticField/MagneticFieldOptions.hpp"

#include "ActsExamples/Options/CommonOptions.hpp"

#include "ActsExamples/TrackFinding/SpacePointMaker.hpp"
#include "ActsExamples/TrackFinding/TrackParamsEstimationAlgorithm.hpp"

#include "ActsExamples/TrackFitting/SurfaceSortingAlgorithm.hpp"
#include "ActsExamples/TrackFitting/TrackFittingAlgorithm.hpp"
#include "ActsExamples/TrackFitting/TrackFittingOptions.hpp"

#include "ActsExamples/TrackInferring/TrackInferringAlgorithm.hpp"
#include "ActsExamples/TrackInferring/TrackInferringOptions.hpp"

#include "ActsExamples/Utilities/Options.hpp"
#include "ActsExamples/Utilities/Paths.hpp"
#include <Acts/Definitions/Units.hpp>

#include <memory>

#include <boost/filesystem.hpp>

#include "RecInput.hpp"

using namespace Acts::UnitLiterals;
using namespace ActsExamples;
using namespace boost::filesystem;
using namespace std::placeholders;

int runRecGNNTracks(int argc, char* argv[],
                    std::shared_ptr<ActsExamples::IBaseDetector> detector) {
  
  std::cout << "HERE HERE HERE" << std::endl;

  // setup and parse options
  auto desc = ActsExamples::Options::makeDefaultOptions();
  Options::addSequencerOptions(desc);
  Options::addRandomNumbersOptions(desc);
  Options::addGeometryOptions(desc);
  Options::addMaterialOptions(desc);
  Options::addInputOptions(desc);
  Options::addOutputOptions(desc, OutputFormat::DirectoryOnly);
  detector->addOptions(desc);
  Options::addMagneticFieldOptions(desc);
  Options::addTrackInferringOptions(desc);
  Options::addDigitizationOptions(desc);

  auto vm = Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  Sequencer sequencer(Options::readSequencerConfig(vm));

  // Read some standard options
  auto logLevel = Options::readLogLevel(vm);
  auto inputDir = vm["input-dir"].as<std::string>();
  auto outputDir = ensureWritableDirectory(vm["output-dir"].as<std::string>());
  auto rnd = std::make_shared<ActsExamples::RandomNumbers>(
      Options::readRandomNumbersConfig(vm));
  
  // Read inference options
  auto moduleName = variables["ml-module-name"].template as<std::string>();
  auto funcName = variables["ml-function-name"].template as<std::string>(); 

  // Setup detector geometry
  auto geometry = Geometry::build(vm, *detector);
  auto trackingGeometry = geometry.first;
  // Add context decorators
  for (auto cdr : geometry.second) {
    sequencer.addContextDecorator(cdr);
  }
  // Setup the magnetic field
  auto magneticField = Options::readMagneticField(vm);

  // Read the sim hits
  auto simHitReaderCfg = setupSimHitReading(vm, sequencer);
  // Read the particles
  auto particleReader = setupParticleReading(vm, sequencer);

  // Run the sim hits smearing
  auto digiCfg = setupDigitization(vm, sequencer, rnd, trackingGeometry,
                                   simHitReaderCfg.outputSimHits);

  // Run the particle selection
  // The pre-selection will select truth particles satisfying provided criteria
  // from all particles read in by particle reader for further processing. It
  // has no impact on the truth hits read-in by the cluster reader.
  TruthSeedSelector::Config particleSelectorCfg;
  particleSelectorCfg.inputParticles = particleReader.outputParticles;
  particleSelectorCfg.inputMeasurementParticlesMap =
      digiCfg.outputMeasurementParticlesMap;
  particleSelectorCfg.outputParticles = "particles_selected";
  particleSelectorCfg.ptMin = 1_GeV;
  particleSelectorCfg.nHitsMin = 9;
  sequencer.addAlgorithm(
      std::make_shared<TruthSeedSelector>(particleSelectorCfg, logLevel));

  // The selected particles
  const auto& inputParticles = particleSelectorCfg.outputParticles;

  
  // Create space points
  SpacePointMaker::Config spCfg;
  spCfg.inputSourceLinks = digiCfg.outputSourceLinks;
  spCfg.inputMeasurements = digiCfg.outputMeasurements;
  spCfg.outputSpacePoints = "spacepoints";
  spCfg.trackingGeometry = trackingGeometry;
  spCfg.geometrySelection = {
      // barrel pixel layers
      Acts::GeometryIdentifier().setVolume(8).setLayer(2),
      Acts::GeometryIdentifier().setVolume(8).setLayer(4),
      Acts::GeometryIdentifier().setVolume(8).setLayer(6),
      Acts::GeometryIdentifier().setVolume(8).setLayer(8),
      // positive endcap pixel layers
      Acts::GeometryIdentifier().setVolume(9).setLayer(2),
      Acts::GeometryIdentifier().setVolume(9).setLayer(4),
      Acts::GeometryIdentifier().setVolume(9).setLayer(6),
      Acts::GeometryIdentifier().setVolume(9).setLayer(8),
      // negative endcap pixel layers
      Acts::GeometryIdentifier().setVolume(7).setLayer(14),
      Acts::GeometryIdentifier().setVolume(7).setLayer(12),
      Acts::GeometryIdentifier().setVolume(7).setLayer(10),
      Acts::GeometryIdentifier().setVolume(7).setLayer(8),
  };
  sequencer.addAlgorithm(std::make_shared<SpacePointMaker>(spCfg, logLevel));
  
  
  // Setup the track inferring algorithm with GNN
  // It takes Spacepoints, python module and function name to be imported
  auto trackInferringCfg = Options::readTrackInferringConfig(vm);
  trackInferringCfg.inputSpacePoints = spCfg.outputSpacePoints;
  trackInferringCfg.mlModuleName = moduleName;
  trackInferringCfg.mlFuncName = funcName;
  trackInferringCfg.outputProtoTracks = "sortedprototracks";
  trackInferringCfg.inferTracks = TrackInferringAlgorithm::makeTrackInferrerFunction();
  sequencer.addAlgorithm(
    std::make_shared<TrackInferringAlgorithm>(trackInferringCfg, logLevel));
  
  
  // Algorithm estimating track parameter from seed
  TrackParamsEstimationAlgorithm::Config paramsEstimationCfg;
  paramsEstimationCfg.inputSeeds = ""; // it will use spacepoints and input proto tracks as inputs.
  paramsEstimationCfg.inputProtoTracks = inputProtoTracks;
  paramsEstimationCfg.inputSpacePoints = {
      spCfg.outputSpacePoints,
  };
  paramsEstimationCfg.inputSourceLinks = digiCfg.outputSourceLinks;
  paramsEstimationCfg.outputTrackParameters = "estimatedparameters";
  paramsEstimationCfg.outputProtoTracks = "prototracks_estimated";
  paramsEstimationCfg.trackingGeometry = tGeometry;
  paramsEstimationCfg.magneticField = magneticField;
  paramsEstimationCfg.bFieldMin = 0.1_T;
  paramsEstimationCfg.deltaRMax = 100._mm;
  paramsEstimationCfg.sigmaLoc0 = 25._um;
  paramsEstimationCfg.sigmaLoc1 = 100._um;
  paramsEstimationCfg.sigmaPhi = 0.005_degree;
  paramsEstimationCfg.sigmaTheta = 0.001_degree;
  paramsEstimationCfg.sigmaQOverP = 0.1 / 1._GeV;
  paramsEstimationCfg.sigmaT0 = 1400._s;
  paramsEstimationCfg.keepOneSeed = true;
  sequencer.addAlgorithm(std::make_shared<TrackParamsEstimationAlgorithm>(
      paramsEstimationCfg, logLevel));

  
  // Track fitting
  // setup the fitter
  TrackFittingAlgorithm::Config fitter;
  fitter.inputMeasurements = digiCfg.outputMeasurements;
  fitter.inputSourceLinks = digiCfg.outputSourceLinks;
  fitter.inputProtoTracks = trkFinderCfg.outputProtoTracks;
  if (dirNav) {
    fitter.inputProtoTracks = sorterCfg.outputProtoTracks;
  }
  fitter.inputInitialTrackParameters = paramsEstimationCfg.outputTrackParameters;
  fitter.outputTrajectories = "trajectories";
  fitter.directNavigation = dirNav;
  fitter.trackingGeometry = tGeometry;
  fitter.dFit = TrackFittingAlgorithm::makeTrackFitterFunction(magneticField);
  fitter.fit = TrackFittingAlgorithm::makeTrackFitterFunction(tGeometry,
                                                              magneticField);
  sequencer.addAlgorithm(
      std::make_shared<TrackFittingAlgorithm>(fitter, logLevel));
  
  
  // write out performance
  // write track finding/seeding performance
  TrackFinderPerformanceWriter::Config tfPerfCfg;
  tfPerfCfg.inputProtoTracks = trkFinderCfg.outputProtoTracks;
  // using selected particles
  tfPerfCfg.inputParticles = inputParticles;
  tfPerfCfg.inputMeasurementParticlesMap = digiCfg.outputMeasurementParticlesMap;
  tfPerfCfg.outputDir = outputDir;
  tfPerfCfg.outputFilename = "performance_seeding_trees.root";
  sequencer.addWriter(
      std::make_shared<TrackFinderPerformanceWriter>(tfPerfCfg, logLevel));

  
  // Write track finding performance data
  CKFPerformanceWriter::Config perfWriterCfg;
  perfWriterCfg.inputParticles = inputParticles;
  perfWriterCfg.inputTrajectories = fitter.outputTrajectories;
  perfWriterCfg.inputMeasurementParticlesMap =
      digiCfg.outputMeasurementParticlesMap;
  // The bottom seed on a pixel detector 'eats' one or two measurements?
  perfWriterCfg.nMeasurementsMin = particleSelectorCfg.nHitsMin;
  perfWriterCfg.outputDir = outputDir;
  sequencer.addWriter(
      std::make_shared<CKFPerformanceWriter>(perfWriterCfg, logLevel));


  return sequencer.run();
}
