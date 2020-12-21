#include <iostream>

#include "Acts/Plugins/Exatrkx/gnn.hpp"

#include "ioFileHelper.hpp"
#include "Volumes.hpp"
#include "Cell.hpp"
#include "Truth.hpp"
#include "SpacePoint.hpp"
#include "Particle.hpp"
#include "Track.hpp"

int main(int argc, char** argv) {
  // --> Command line options
  std::string file{"./event0000000000"};
  std::string type{"lxyz"};
  std::string save{""};
  
  // --> Parse command line
  int opt;
  while((opt = getopt(argc, argv, "f:t:s:h")) != -1){
    switch (opt) {
      case 'f':
        file = optarg;
        break;
      case 't':
        type = optarg;
        break;
      case 's':
        save = optarg;
        break;
      default:
        std::cerr << "Usage: " << argv[0] << " [-f-t-sh] [-f FILENAME] [-t FILETYPE] [-s SAVEPATH]\n";
        exit(EXIT_FAILURE);
    }
  }
    
  // --> Load Track ML Data
  std::vector<const SpacePoint*> hits;
  std::vector<const Truth*> truths;
  std::vector<const Cell*> cells;
  std::vector<const Particle*> particles;
  
  load_hits<SpacePoint, Volumes>(file, &hits);
  load_truth<Truth>(file, &truths);
  load_cells<Cell>(file, &cells);
  load_particles<Particle>(file, &particles);
  
  std::cout << "Loaded " << hits.size() << " hits" << std::endl;
    
  // --> Run Inference
  std::vector<Track*> tracks;
  Acts::inferTracks(&hits, &truths, &cells, &particles, &tracks);
  
  return 0;
}
