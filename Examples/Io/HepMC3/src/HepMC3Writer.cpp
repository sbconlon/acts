// This file is part of the Acts project.
//
// Copyright (C) 2018-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ActsExamples/Io/HepMC3/HepMC3Writer.hpp"

#include "ActsExamples/Utilities/Paths.hpp"

ActsExamples::HepMC3AsciiWriter::HepMC3AsciiWriter(const Config&& cfg,
                                                   Acts::Logging::Level lvl)
    : WriterT(cfg.inputEvents, "HepMC3EventWriter", lvl), m_cfg(cfg) {
  if (m_cfg.outputStem.empty())
    throw std::invalid_argument("Missing output stem file name");
}

ActsExamples::ProcessCode ActsExamples::HepMC3AsciiWriter::writeT(
    const ActsExamples::AlgorithmContext& ctx,
    const std::vector<HepMC3::GenEvent>& events) {
  auto path = perEventFilepath(m_cfg.outputDir, m_cfg.outputStem + ".hepmc3",
                               ctx.eventNumber);

  ACTS_DEBUG("Attempting to write event to " << path);
  HepMC3::WriterAscii writer(path);

  for (const auto& event : events) {
    writer.write_event(event);
    if (writer.failed())
      return ActsExamples::ProcessCode::ABORT;
  }

  writer.close();
  return ActsExamples::ProcessCode::SUCCESS;
}
