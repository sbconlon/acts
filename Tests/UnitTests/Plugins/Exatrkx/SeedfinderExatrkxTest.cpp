#include <fstream>
#include<iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <unistd.h>


#include "Acts/Plugins/Exatrkx/gnn.hpp"

std::vector<float> readFile(std::string filename) {
    std::string line;
    std::vector<float> readSP;
    std::ifstream spFile(filename);
    if(spFile.is_open()) {
        while(!spFile.eof()) {
            std::getline(spFile, line);
            std::stringstream ss(line);
            std::string linetype;
            ss >> linetype;
            float layer, x, y, z;
            if (linetype == "lxyz") {
                ss >> layer >> x >> y >> z;
                readSP.push_back(layer);
                readSP.push_back(x);
                readSP.push_back(y);
                readSP.push_back(z);
            }
        }
    }
    return readSP;
}


int main(int argc, char** argv) {
    // TODO: add clock start
    
    // Command line options
    std::string file{"sp.txt"};
    //bool help(false);
    //bool quiet(false);
    //bool allgroup(false);
    //bool do_cpu(true);
    
    // Parse command line
    int opt;
    while((opt = getopt(argc, argv, "f:h")) != -1){
        switch (opt) {
            case 'f':
                file = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-hq] [-f FILENAME]\n";
                exit(EXIT_FAILURE);
        }
    }
    
    // Read File
    std::ifstream f(file);
    if (!f.good()) {
        std::cerr << "input file \"" << file << "\" does not exist\n";
        exit(EXIT_FAILURE);
    }
    std::vector<float> spVec = readFile(file);
    
    // Build Graph
    std::string exatrkx_path = "dummy";
    std::vector<std::vector<int>> edges = Acts::prepare_graph(spVec, exatrkx_path);
    
    // Print Edges
    for(size_t i=0; i < edges.size(); ++i) {
        for(size_t j=0; j < edges[i].size(); ++j){
            std::cout << edges[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}