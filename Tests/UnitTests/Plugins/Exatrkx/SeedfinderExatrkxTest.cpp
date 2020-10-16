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

int getTrackMLLayer(int volId, int layId){
    // Maps volume and layer id pairs onto 0-9 integer space
    // and returns -1 if hit is in a endcap
    switch(volId){
        case 8:
            if(layId == 2){return 0;}
            if(layId == 4){return 1;}
            if(layId == 6){return 2;}
            if(layId == 8){return 3;}
            break;
        case 13:
            if(layId == 2){return 4;}
            if(layId == 4){return 5;}
            if(layId == 6){return 6;}
            if(layId == 8){return 7;}
            break;
        case 17:
            if(layId == 2){return 8;}
            if(layId == 4){return 9;}
            break;
        default:
            return -1;
            break;
    }
}

bool isInt(std::string str) {
    if (str.empty()) {
        return false;
    }
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool isFloat(std::string str) {
    if (str.empty()) {
        return false;
    }
    return str.find_first_not_of("0123456789.-") == std::string::npos;
}

std::vector<const SpacePoint*> readFile(std::string filename, std::string type) {

    std::vector<const SpacePoint *> readSP;
    int layer;
    float x, y, z;
    SpacePoint* sp;
    Truth* tr;

    if(type == "TrackML") {
        // Filename is treated as "/path/to/eventID"
        std::string hitFilename = filename + "-hits.csv";
        std::cout << hitFilename << std::endl;
        std::string truthFilename = filename + "-truth.csv";
        std::ifstream htFile(hitFilename);
        std::ifstream trFile(truthFilename);
        if(htFile.is_open() && trFile.is_open() && htFile.good() && trFile.good()) {
            std::string token;
            std::getline(htFile, token);  // discard column labels of csv file
            std::getline(trFile, token);  // discard column labels of csv file
            while(!htFile.eof() && !trFile.eof()) {
                // Parse hit file
                unsigned long htHid;
                int volId, layId;
                std::getline(htFile, token, ',');
                if (!isInt(token)) { std::getline(htFile, token); continue; }
                htHid = std::stol(token);
                std::getline(htFile, token, ',');
                std::getline(htFile, token, ',');
                if (!isInt(token)) { std::getline(htFile, token); continue; }
                volId = std::stoi(token);
                std::getline(htFile, token, ',');
                if (!isInt(token)) { std::getline(htFile, token); continue; }
                layId = std::stoi(token);
                std::getline(htFile, token, ',');
                std::getline(htFile, token, ',');
                if (!isFloat(token)) { std::getline(htFile, token); continue; }
                x = std::stof(token);
                std::getline(htFile, token, ',');
                if (!isFloat(token)) { std::getline(htFile, token); continue; }
                y = std::stof(token);
                std::getline(htFile, token, ',');
                if (!isFloat(token)) { std::getline(htFile, token); continue; }
                z = std::stof(token);
                std::getline(htFile, token);
                layer = getTrackMLLayer(volId, layId);

                // Parse truth file
                unsigned long trHid, pid;
                std::getline(trFile, token, ',');
                if (!isInt(token)) { std::getline(trFile, token); continue; }
                trHid = std::stol(token);
                std::getline(trFile, token, ',');
                if (!isInt(token)) { std::getline(trFile, token); continue; }
                pid = std::stol(token);
                std::getline(trFile, token);

                // Construct spacepoint
                if (trHid == htHid && layer != -1) {
                    sp = new SpacePoint{x, y, z,  layer};
                    tr = new Truth{trHid, pid};
                    sp->ids = tr;
                    readSP.push_back(sp);
                }
            }
        }

    } else if(type == "lxyz") {
        std::ifstream spFile(filename);
        if(spFile.is_open()) {
            while(!spFile.eof()) {
                std::string line;
                std::getline(spFile, line);
                std::stringstream ss(line);
                std::string linetype;
                ss >> linetype;
                float varR, varZ;
                if (linetype == "lxyz") {
                    ss >> layer >> x >> y >> z >> varR >> varZ;
                    float f22 = varR;
                    float wid = varZ;
                    float cov = wid * wid * .08333;
                    if (cov < f22)
                        cov = f22;
                    if (std::abs(z) > 450.) {
                        varZ = 9. * cov;
                        varR = .06;
                    } else {
                        varR = 9. * cov;
                        varZ = .06;
                    }
                    sp = new SpacePoint{x, y, z, layer};
                    sp->varianceR = varR;
                    sp->varianceZ = varZ;
                    readSP.push_back(sp);
                }
            }
        }

    } else {
        std::cerr << "Error: " << type << "file type not found.\n";
        exit(EXIT_FAILURE);
    }

    return readSP;
}

std::vector<std::vector<const SpacePoint*>> truth_to_tracks(std::vector<const SpacePoint*> hits) {

    std::vector<std::vector<const SpacePoint *>> tracks;
    for (auto hitIter=hits.begin(); hitIter!=hits.end(); ++hitIter) {
        auto trkIter=tracks.begin();
        for ( ; trkIter!=tracks.end(); ++trkIter) {
            if (((*trkIter)[0])->ids->pid() == (*hitIter)->ids->pid()) {
                auto it=(*trkIter).begin();
                for ( ; it!=(*trkIter).end(); ++it) {
                    if ((*it)->r() > (*hitIter)->r()) {
                        (*trkIter).insert(it, (*hitIter));
                        break;
                    }
                }
                if (it==(*trkIter).end()) {
                    (*trkIter).push_back((*hitIter));
                }
                break;
            }
        }
        if (trkIter==tracks.end()) {
            std::vector<const SpacePoint*> new_track;
            new_track.push_back(*hitIter);
            tracks.push_back(new_track);
        }
    }
    return tracks;
}

/***
void print_track(std::vector<std::vector<const SpacePoint*>> tracks, int idx) {
    std::vector <const SpacePoint*> trk = tracks[idx];
    std::cout << "Example Size: " << trk.size() << std::endl;
    std::cout << " HID     " << " PID            " << "  layer " << "   R   "<< std::endl;
    for (auto ex=trk.begin(); ex!=trk.end(); ++ex) {
        std::cout << (*ex)->ids->hid() << "  " << (*ex)->ids->pid() << "    " << (*ex)->layer() << "        " <<  (*ex)->r() << std::endl;
    }
}
***/

std::vector<Acts::Seed<SpacePoint>> tracks_to_dplet_seeds(std::vector<std::vector<const SpacePoint *>> tracks) {

    std::vector<Acts::Seed<SpacePoint>> seeds;
    const SpacePoint &mSp = *(new SpacePoint{0, 0, 0, 0});                                        // Dummy spacepoint to fill middle point in the seed
    for (auto trkIter=tracks.begin(); trkIter!=tracks.end(); ++trkIter) {
        for (auto hit1=(*trkIter).begin(); hit1!=(*trkIter).end(); ++hit1) {
            auto hit2 = hit1 + 1;
            while (hit2!=(*trkIter).end() && (*hit1)->layer() >= (*hit2)->layer()) { ++hit2; }    // Screen for duplicate hits
            int next_layer = (*hit2)->layer();
            while(hit2!=(*trkIter).end() && (*hit2)->layer() == next_layer) {                     // Use all hits in the next layer as a seed
                const SpacePoint &bSp = **hit1;
                const SpacePoint &tSp = **hit2;
                seeds.push_back(Acts::Seed<SpacePoint>(bSp, mSp, tSp, 0));
                ++hit2;
            }
        }
    }
    return seeds;
}

std::vector<Acts::Seed<SpacePoint>> tracks_to_tplet_seeds(std::vector<std::vector<const SpacePoint *>> tracks) {

    std::vector<Acts::Seed<SpacePoint>> seeds;
    for (auto trkIter=tracks.begin(); trkIter!=tracks.end(); ++trkIter) {
        for (auto hit1=(*trkIter).begin(); hit1!=(*trkIter).end(); ++hit1) {                         // bottom hit loop
            auto hit2 = hit1 + 1;
            while (hit2!=(*trkIter).end() && (*hit1)->layer() >= (*hit2)->layer()) { ++hit2; }       // screen for duplicate bottom hits
            int second_layer = (*hit2)->layer();
            for (; hit2!=(*trkIter).end() && (*hit2)->layer() == second_layer; ++hit2) {             // middle hit loop (use all hits in 2nd layer)
                auto hit3 = hit2 + 1;
                while (hit3!=(*trkIter).end() && (*hit2)->layer() >= (*hit3)->layer()) { ++hit3; }   // screen for duplicate middle hits
                int third_layer = (*hit3)->layer();
                for (; hit3!=(*trkIter).end() && (*hit3)->layer() == third_layer; ++hit3) {          // top hit loop (use all hits in 3rd layer)
                    const SpacePoint &bSp = **hit1;
                    const SpacePoint &mSp = **hit2;
                    const SpacePoint &tSp = **hit3;
                    seeds.push_back(Acts::Seed<SpacePoint>(bSp, mSp, tSp, 0));
                }
            }
        }
    }
    return seeds;
}

int main(int argc, char** argv) {

    // --> Command line options

    std::string file{"sp.txt"};
    std::string type{"lxyz"};
    //bool help(false);
    //bool quiet(false);
    //bool allgroup(false);
    //bool do_cpu(true);


    // --> Parse command line

    int opt;
    while((opt = getopt(argc, argv, "f:t:h")) != -1){
        switch (opt) {
            case 'f':
                file = optarg;
                break;
            case 't':
                type = optarg;
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-hq] [-f FILENAME]\n";
                exit(EXIT_FAILURE);
        }
    }

    // --> Read File

    //std::ifstream f(file);
    //if (!f.good()) {
    //    std::cerr << "input file \"" << file << "\" does not exist\n";
    //    exit(EXIT_FAILURE);
    //}

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
    // silicon detector max
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
    auto tit = truth_dplet_seedVect.begin();
    for (auto eit=exatrkx_seedVect.begin(); eit!=exatrkx_seedVect.end(); ++eit) {
        for (tit=truth_dplet_seedVect.begin(); tit!=truth_dplet_seedVect.end(); ++tit) {
            if(*(eit->sp()[0]) == *(tit->sp()[0]) && *(eit->sp()[1]) == *(tit->sp()[1]) && *(eit->sp()[2]) == *(tit->sp()[2])) {
                exatrkx_correct++;
                break;
            }
        }
    }
    int acts_correct = 0;
    tit = truth_tplet_seedVect.begin();
    for (auto ait=acts_seedVect.begin(); ait!=acts_seedVect.end(); ++ait) {
        for (tit=truth_tplet_seedVect.begin(); tit!=truth_tplet_seedVect.end(); ++tit) {
            if(*(ait->sp()[0]) == *(tit->sp()[0]) && *(ait->sp()[1]) == *(tit->sp()[1]) && *(ait->sp()[2]) == *(tit->sp()[2])) {
                   acts_correct++;
                   break;
               }
        }
    }

    std::cout << exatrkx_correct << std::endl;

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

    return 0;
}
