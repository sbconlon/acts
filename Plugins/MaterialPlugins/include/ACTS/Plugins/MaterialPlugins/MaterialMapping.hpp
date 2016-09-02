// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// MaterialMapping.h, ACTS project MaterialPlugins
///////////////////////////////////////////////////////////////////

#ifndef ACTS_MATERIALPLUGINS_MATERIALMAPPIN_H
#define ACTS_MATERIALPLUGINS_MATERIALMAPPIN_H

#include <map>
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {

class Layer;
class MaterialProperties;
class LayerMaterialRecord;

/// @class MaterialMapping
///
/// @brief
///

class MaterialMapping
{
public:
  /// @struct Config
  ///
  /// Configuration for the MaterialMapping
  struct Config
  {
    // ignore events with eta bigger than the cutoff value @TODO add later
    //         double etaCutoff;
    // needed for debugging: -1 negative | 0 all | 1 positive @TODO add later
    //          int etaSide;
    /// extrapolation engine
    std::shared_ptr<IExtrapolationEngine> extrapolationEngine = nullptr;
  };

  ///@brief default constructor
  MaterialMapping(const Config&           cnf,
                  std::unique_ptr<Logger> logger
                  = getDefaultLogger("MaterialMapping", Logging::INFO));

  ///@brief destructor
  ~MaterialMapping();

  /// maps the material for the given direction(eta,phi) onto the layers of the
  /// given tracking geometry
  /// @param stepCollection is a shared_ptr to the MaterialStepCollection object
  /// which is a vector of MaterialSteps including the material on a certain
  /// position
  /// @param eta pseudorapidity - first dimension of the direction in which the
  /// material was collected
  /// @param phi the phi angle  - second dimension of the direction in which the
  /// material was collected
  /// @param startPoint optionally the start point can be given, if the point
  /// from where the material collection started is not origin
  void
  mapMaterial(const MaterialTrackRecord& matTrackRec);
  /// averages the material of the layer records collected so far
  void
  averageLayerMaterial();
  /// after all step collections have been mapped this method needs to be called
  /// it sets the created material to the layers
  void
  finalizeLayerMaterial();
  /// return the layer records
  const std::map<const Layer*, LayerMaterialRecord>
  layerRecords() const;
  /// return the material step positions per layer
  const std::multimap<const Acts::Layer*, const MaterialStep>
  layerMaterialSteps() const;

  /// set logging instance
  void
  setLogger(std::unique_ptr<Logger> logger);

private:
  /// internally used method to collect hits on their corresponding layers
  /// received when extrapolating through the tracking geometry
  bool
  collectLayersAndHits(
      const MaterialTrackRecord& matTrackRec,
      std::vector<std::pair<const Acts::Layer*, Acts::Vector3D>>&
          layersAndHits);
  /// internally used method to associate the material to the right layer in the
  /// tracking geometry
  void
  associateLayerMaterial(
      const MaterialTrackRecord& matTrackRec,
      std::vector<std::pair<const Acts::Layer*, Acts::Vector3D>>&
          layersAndHits);
  /// internally used method to associate a hit to a given layer by recording it
  /// in the layer records map
  void
  associateHit(const Layer*                    layer,
               const Acts::Vector3D&           position,
               const Acts::MaterialProperties* layerMaterialProperties);
  /// configuration object

  const Logger&
  logger() const
  {
    return *m_logger;
  }

  Config                  m_cnf;
  std::unique_ptr<Logger> m_logger;
  std::map<const Layer*, LayerMaterialRecord> m_layerRecords;
  /// create object which connects layer with the original material step
  /// positions
  std::multimap<const Acts::Layer*, const MaterialStep> m_layersAndSteps;
};
}

inline const std::map<const Acts::Layer*, Acts::LayerMaterialRecord>
Acts::MaterialMapping::layerRecords() const
{
  return m_layerRecords;
}

inline const std::multimap<const Acts::Layer*, const Acts::MaterialStep>
Acts::MaterialMapping::layerMaterialSteps() const
{
  return m_layersAndSteps;
}

#endif  // ACTS_MATERIALPLUGINS_MATERIALMAPPIN_Hr