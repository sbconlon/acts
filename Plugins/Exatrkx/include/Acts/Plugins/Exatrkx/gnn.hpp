#pragma once

#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/InternalSeed.hpp"
#include "Acts/Seeding/InternalSpacePoint.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"

#include <vector>
#include <string>
#include <iterator>

namespace Acts{

  struct GraphNeuralNetworkOptions {

    /// Graph Neural Network Options
    ///
    /// @param moduleName Name of the ML Python module that should be imported
    /// @param funcName Name of the Python function in the module which should
    ///                 should be used to find tracks.
    GraphNeuralNetworkOptions(std::string moduleName, std::string funcName)
      : mlModuleName(moduleName), mlFuncName(funcName){}

    /// Python module name
    std::string mlModuleName;
    /// Python function inside the module that performs tracking finding
    std::string mlFuncName;
  };

  template <typename spacepoint_t>
  struct GraphNeuralNetworkResults {
    /// result of inference pipeline, list of spacepoints in a track
    std::vector<spacepoint_t> spTrack;
  };

  class GraphNeuralNetwork {
    public:
      /// Default constructor is deleted
      GraphNeuralNetwork() = delete;
      /// Constructor
      GraphNeuralNetwork() { return; }

      /// Graph neural network inference implementation,
      /// calls the Python inference pipeline.
      ///
      /// @tparam spacepoint_container_t Type of the spacepoint container
      ///
      /// @return a container of infer track results
      template<typename spacepoint_container_t>
      std::vector<Result<GraphNeuralNetworkResult<
          typename spacepoint_container_t::value_type>>>
      inferTracks(const spacepoint_container_t& hits,
                  const GraphNeuralNetworkOptions& ifOptions) const;

      void print_hello();
  };

} // namespace Acts

#include "Acts/Plugins/Exatrkx/gnn.ipp"
