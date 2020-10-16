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
    
    template <typename external_spacepoint_t>
    std::vector<Acts::Seed<external_spacepoint_t>> prepareDoubletGraph(const auto first,
                                                                       const auto last,
                                                                       const size_t nhits,
                                                                       const char *config_path="configs/prep_trackml.yaml",
                                                                       const char *filename="event00000",
                                                                       long verbose=0,
                                                                       long show_config=0);
    
    void print_hello();

} // namespace Acts


#include "Acts/Plugins/Exatrkx/gnn_impl.hpp"