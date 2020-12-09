#include <fstream>
#include<iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <unistd.h>
#include <iterator>
#include <chrono>

#include "Acts/Seeding/BinFinder.hpp"
#include "Acts/Seeding/BinnedSPGroup.hpp"
#include "Acts/Seeding/InternalSeed.hpp"
#include "Acts/Seeding/InternalSpacePoint.hpp"
#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/Seedfinder.hpp"
#include "Acts/Seeding/SpacePointGrid.hpp"

#include "Acts/Plugins/Exatrkx/gnn.hpp"
#include "ATLASCuts.hpp"
#include "SpacePoint.hpp"
#include "helper.hpp"
#include "ioFileHelper.hpp"
#include "trackHelper.hpp"

int main(int argc, char** argv) {

    // --> Command line options
    std::string file{"sp.txt"};
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
    std::vector<const SpacePoint*> spVect = readFile(file, type);
    std::cout << std::endl;
    std::cout << "--> Loaded " << spVect.size() << " hits" << std::endl;



    // --> Build Truth Graph
    std::vector<std::vector<const SpacePoint*>> tracks = truth_to_tracks(spVect);
    std::vector<Acts::Seed<SpacePoint>> truth_dplet_seedVect = tracks_to_dplet_seeds(tracks);
    std::vector<Acts::Seed<SpacePoint>> truth_tplet_seedVect = tracks_to_tplet_seeds(tracks);
    std::cout << "--> Number of Truth Tracks: " << tracks.size() << std::endl;
    std::cout << "--> Number of Truth Doublet Seeds: " << truth_dplet_seedVect.size() << std::endl;
    std::cout << "--> Number of Truth Triplet Seeds: " << truth_tplet_seedVect.size() << std::endl;



    // --> Build Exatrkx Graph
    auto start_trkx = std::chrono::system_clock::now();
    std::vector<Acts::Seed<SpacePoint>> exatrkx_seedVect;
    exatrkx_seedVect = Acts::prepareTripletGraph<SpacePoint>(spVect.begin(),
                                                             spVect.end(),
                                                             spVect.size(),
                                                             "prep_small.yaml",
                                                             "event00000");
    auto end_trkx = std::chrono::system_clock::now();
    std::chrono::duration<double> elap_trkx = end_trkx - start_trkx;
    double exatrkx_time= elap_trkx.count();
    std::cout << "--> Number of Exatrkx Triplet Seeds: " << exatrkx_seedVect.size() << std::endl;



    // --> Build ACTS Graph
    auto start_acts = std::chrono::system_clock::now();

    Acts::SeedfinderConfig<SpacePoint> config;
    // silicon detector max               // Default parameters
    config.rMax = 600.;                   //   160.
    config.deltaRMin = 5.;                //     5.
    config.deltaRMax = 400.;              //   160.
    config.collisionRegionMin = -150.;    //  -250.
    config.collisionRegionMax = 150.;     //   250.
    config.zMin = -2800.;                 // -2800.
    config.zMax = 2800.;                  //  2800.
    config.maxSeedsPerSpM = 5;            //     5
    // 2.7 eta
    config.cotThetaMax = 7.40627;          // 7.40627
    config.sigmaScattering = 5;            // 1.00000
    config.minPt = 200.;                   // 500.
    config.bFieldInZ = 0.00208;            // 0.00199724
    config.beamPos = {0, 0};               // {-.5, -.5}
    config.impactMax = 20.;                // 10.

    auto bottomBinFinder = std::make_shared<Acts::BinFinder<SpacePoint>>(
      Acts::BinFinder<SpacePoint>());
    auto topBinFinder = std::make_shared<Acts::BinFinder<SpacePoint>>(
      Acts::BinFinder<SpacePoint>());
    Acts::SeedFilterConfig sfconf;
    Acts::ATLASCuts<SpacePoint> atlasCuts = Acts::ATLASCuts<SpacePoint>();
    config.seedFilter = std::make_unique<Acts::SeedFilter<SpacePoint>>(
      Acts::SeedFilter<SpacePoint>(sfconf, &atlasCuts));
    Acts::Seedfinder<SpacePoint> a(config);

    // covariance tool, sets covariances per spacepoint as required
    auto ct = [=](const SpacePoint& sp, float, float, float) -> Acts::Vector2D {
        return {sp.varianceR, sp.varianceZ};
    };

    // setup spacepoint grid config
    Acts::SpacePointGridConfig gridConf;
    gridConf.bFieldInZ = config.bFieldInZ;
    gridConf.minPt = config.minPt;
    gridConf.rMax = config.rMax;
    gridConf.zMax = config.zMax;
    gridConf.zMin = config.zMin;
    gridConf.deltaRMax = config.deltaRMax;
    gridConf.cotThetaMax = config.cotThetaMax;
    // create grid with bin sizes according to the configured geometry
    std::unique_ptr<Acts::SpacePointGrid<SpacePoint>> grid =
      Acts::SpacePointGridCreator::createGrid<SpacePoint>(gridConf);
    auto spGroup = Acts::BinnedSPGroup<SpacePoint>(spVect.begin(), spVect.end(), ct,
                                                   bottomBinFinder, topBinFinder,
                                                   std::move(grid), config);

    std::vector<std::vector<Acts::Seed<SpacePoint>>> acts_2DseedVect;
    auto start = std::chrono::system_clock::now();
    auto groupIt = spGroup.begin();
    auto endOfGroups = spGroup.end();
    for (; !(groupIt == endOfGroups); ++groupIt) {
        acts_2DseedVect.push_back(a.createSeedsForGroup(
            groupIt.bottom(), groupIt.middle(), groupIt.top()));
    }

    auto end_acts = std::chrono::system_clock::now();
    std::chrono::duration<double> elap_acts = end_acts - start_acts;
    double acts_time= elap_acts.count();

    std::vector<Acts::Seed<SpacePoint>> acts_seedVect;
    for (auto it=acts_2DseedVect.begin(); it!=acts_2DseedVect.end(); ++it){
	for (auto jt=(*it).begin(); jt!=(*it).end(); ++jt) {
            acts_seedVect.push_back(*jt);
        }
    }
    std::cout << "--> Number of ACTS Triplet Seeds: " << acts_seedVect.size() << std::endl;



    // --> Graph Evaluation
    int exatrkx_correct = 0;
    auto tit = truth_tplet_seedVect.begin();
    for (auto eit=exatrkx_seedVect.begin(); eit!=exatrkx_seedVect.end(); ++eit) {
        for (tit=truth_tplet_seedVect.begin(); tit!=truth_tplet_seedVect.end(); ++tit) {
            if(*(eit->sp()[0]) == *(tit->sp()[0]) && *(eit->sp()[1]) == *(tit->sp()[1]) && *(eit->sp()[2]) == *(tit->sp()[2])) {
                exatrkx_correct++;
                break;
            }
        }
    }
    int acts_correct = 0;
    for (auto ait=acts_seedVect.begin(); ait!=acts_seedVect.end(); ++ait) {
        for (tit=truth_tplet_seedVect.begin(); tit!=truth_tplet_seedVect.end(); ++tit) {
            if(*(ait->sp()[0]) == *(tit->sp()[0]) && *(ait->sp()[1]) == *(tit->sp()[1]) && *(ait->sp()[2]) == *(tit->sp()[2])) {
                   acts_correct++;
                   break;
               }
        }
    }

    float exatrkx_precision = (float) exatrkx_correct / (float) exatrkx_seedVect.size();
    float exatrkx_recall = (float) exatrkx_correct / (float) truth_tplet_seedVect.size();
    float acts_precision = (float) acts_correct / (float) acts_seedVect.size();
    float acts_recall = (float) acts_correct / (float) truth_tplet_seedVect.size();

    std::cout << std::endl;
    std::cout << "------- ( Exatrkx Seeding ) -------" << std::endl;
    std::cout << "Runtime:   " << exatrkx_time << " sec" << std::endl;
    std::cout << "Precision: " << exatrkx_precision << std::endl;
    std::cout << "Recall:    " << exatrkx_recall << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "------- ( ACTS Seeding ) ----------" << std::endl;
    std::cout << "Runtime:   " << acts_time << " sec" << std::endl;
    std::cout << "Precision: " << acts_precision << std::endl;
    std::cout << "Recall:    " << acts_recall << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << std::endl;

    if(!save.empty()) {
      save_edges_to_csv(exatrkx_seedVect, save + "/exatrkx");
      std::cout << "Successfully saved exatrkx triplets" << std::endl;
      save_edges_to_csv(acts_seedVect, save + "/acts");
      std::cout << "Successfully saved acts triplets" << std::endl;
    }

    return 0;
}
