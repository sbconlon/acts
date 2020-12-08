


std::vector<const SpacePoint*> readFile(std::string filename, std::string type) {

    std::vector<const SpacePoint *> readSP;
    int layer;
    float x, y, z;
    SpacePoint* sp;
    Truth* tr;
    Volumes* vl;

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
                //layer = getTrackMLLayer(volId, layId);

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
                if (trHid == htHid) {
                    sp = new SpacePoint{x, y, z};
                    tr = new Truth{trHid, pid};
                    vl = new Volumes{volId, layId};
                    sp->ids = tr;
                    sp->vols = vl;
                    readSP.push_back(sp);
                } else {
                  std::cerr << "Error while reading input file: hit id from hit file does not match hit id from truth file" << std::endl;
                  exit(EXIT_FAILURE);
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
                    sp = new SpacePoint{x, y, z};
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


void save_edges_to_csv(std::vector<Acts::Seed<SpacePoint>> seeds, std::string output_path) {
  // Save edges as hit id triplet in csv file format
  std::fstream outfile;
  outfile.open(output_path + "-edges.csv", std::fstream::out); // TODO: allow user to define eventid and graph id
  int evtid = 0; // TODO: allow for user defined evtid
  for(auto it=seeds.begin(); it!=seeds.end(); ++it){
    outfile << evtid << ',';
    outfile << it->sp()[0]->ids->hid() << ',';
    outfile << it->sp()[1]->ids->hid() << ',';
    outfile << it->sp()[2]->ids->hid() << std::endl;
  }
  outfile.close();
}
