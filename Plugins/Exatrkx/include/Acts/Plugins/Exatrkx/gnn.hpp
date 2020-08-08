#pragma once

#include <vector>
#include <string>

namespace Acts{
    
    std::vector<std::vector<int>> prepare_graph(std::vector<float> hits,
                                                std::string exatrkx_path);

} // namespace Acts