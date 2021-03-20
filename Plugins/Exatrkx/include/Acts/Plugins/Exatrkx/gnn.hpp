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

  class     
    template <typename external_spacepoint_t, 
              typename external_truth_t, 
              typename external_cell_t, 
              typename external_particle_t, 
              typename external_track_t>
    void inferTracks(std::vector<const external_spacepoint_t*>* hits,
                     std::vector<const external_truth_t*>* truth,
                     std::vector<const external_cell_t*>* cells,
                     std::vector<const external_particle_t*>* particles,
                     std::vector<external_track_t*>* tracks);

    void print_hello();

} // namespace Acts

#include "Acts/Plugins/Exatrkx/gnn_inference_impl.hpp"
