#include<fstream>
#include<sstream>
#include<unordered_map>
#include<vector>
#include<unistd.h>

#include "helper.hpp"

std::unordered_map<std::string, int> get_column_order(std::string column_headers){
  // This function takes the first line of a csv file to 
  // create a dictionary style mapping between column names
  // and column indexs.
  
  // Initialize variables
  std::unordered_map<std::string, int> col_order;
  std::string col_name;
  std::stringstream s(column_headers);
  // Populate map
  for(int col_idx=0; std::getline(s, col_name, ','); col_idx++){ 
    col_order.emplace(col_name, col_idx); 
  }
  // Assert map isnt empty
  if(col_order.empty()){
    std::cerr << "Could not determine csv column names" << std::endl;
    exit(EXIT_FAILURE);
  }
  return col_order;
}

bool has_column(std::unordered_map<std::string, int> columns, std::string value) {
  // This function returns true if the column exists and false otherwise
  return (columns.find(value) != columns.end());
}

void assert_has_column(std::unordered_map<std::string, int> columns, std::string value){
  // This function throws error if columns does not contain the given column name.
  if(!has_column(columns, value)){
    std::cerr << "Given hits csv does not contain a '" << value << "' column" << std::endl;
    exit(EXIT_FAILURE);
  }
}

template<typename vl_T>
void populate_optional_volume_members(std::unordered_map<std::string, int> columns, 
                                      std::vector<std::string> row, vl_T* vol){
  // This function populates optional class members for the volume object
  // depending on if they are present in the given csv
  if(has_column(columns, "geometry_id"))
    vol->set_geoId(std::stol(row[columns["geometry_id"]]));
}

template<typename sp_T, typename vl_T>
void load_hits(std::string file_root, std::vector<const sp_T*>* hits) {
  // This function loads hits from a TrackML-like event
  // Note: it is agnostic to column order in order to be more adaptable
  // to datasets from the TrackML challenge or those simulated by
  // ACTS
  
  // Initialize variables
  std::ifstream fin(file_root + "-hits.csv");
  std::vector<std::string> row;
  std::string line, word;
  sp_T* sp;
  vl_T* vl;
  
  // Assert the file has been opened
  if(!fin.is_open()){
    std::cerr << "Could not open " << file_root << "-hits.csv" << std::endl;
  }
  
  // Determine column ordering
  std::getline(fin, line);
  std::unordered_map<std::string, int> col_order = get_column_order(line);
  
  // Assert CSV is well formatted
  assert_has_column(col_order, "x");
  assert_has_column(col_order, "y");
  assert_has_column(col_order, "z");
  assert_has_column(col_order, "hit_id");
  assert_has_column(col_order, "volume_id");
  assert_has_column(col_order, "layer_id");
  assert_has_column(col_order, "module_id");
  
  // Iterate through file and create a spacepoint object for each row
  // and save it to the hits vector.
  while(std::getline(fin, line)) {
    row.clear();
    std::stringstream s(line);
    while(std::getline(s, word, ',')) { row.push_back(word); }
    
    sp = new sp_T{std::stoi(row[col_order["hit_id"]]),
                  std::stof(row[col_order["x"]]),
                  std::stof(row[col_order["y"]]),
                  std::stof(row[col_order["z"]])};
    vl = new vl_T{std::stoi(row[col_order["volume_id"]]),
                  std::stoi(row[col_order["layer_id"]]),
                  std::stoi(row[col_order["module_id"]])}; 
    
    populate_optional_volume_members(col_order, row, vl);
    
    sp->vols = vl;
    hits->push_back(sp);
  }
}

template<typename tr_T>
void populate_optional_truth_members(std::unordered_map<std::string, int> columns, 
                                      std::vector<std::string> row, tr_T* tr){
  // This function populates optional class members for the truth object
  // depending on if they are present in the given csv
  if(has_column(columns, "weight"))
    tr->set_weight(std::stof(row[columns["weight"]]));
  if(has_column(columns, "geometry_id"))
    tr->set_geoId(std::stoul(row[columns["geometry_id"]]));
  if(has_column(columns, "tt"))
    tr->set_tt(std::stof(row[columns["tt"]]));
  if(has_column(columns, "te"))
    tr->set_te(std::stof(row[columns["te"]]));
  if(has_column(columns, "deltapx"))
    tr->set_deltapx(std::stof(row[columns["deltapx"]]));
  if(has_column(columns, "deltapy"))
    tr->set_deltapy(std::stof(row[columns["deltapy"]]));
  if(has_column(columns, "deltapz"))
    tr->set_deltapz(std::stof(row[columns["deltapz"]]));
  if(has_column(columns, "deltae"))
    tr->set_deltae(std::stof(row[columns["deltae"]]));
  if(has_column(columns, "index"))
    tr->set_index(std::stoi(row[columns["index"]]));
}

template<typename tr_T>
void load_truth(std::string file_root, std::vector<const tr_T*>* truths) {
  // This function loads truth from a TrackML-like event
  // Note: it is agnostic to column order in order to be more adaptable
  // to datasets from the TrackML challenge or those simulated by
  // ACTS
  
  // Initialize variables
  std::ifstream fin(file_root + "-truth.csv");
  std::vector<std::string> row;
  std::string line, word;
  tr_T* tr;
  
  // Assert the file has been opened
  if(!fin.is_open()){
    std::cerr << "Could not open " << file_root << "-truth.csv" << std::endl;
  }
  
  // Determine column ordering
  std::getline(fin, line);
  std::unordered_map<std::string, int> col_order = get_column_order(line);
  
  // Assert hits csv has required values
  assert_has_column(col_order, "hit_id");
  assert_has_column(col_order, "particle_id");
  assert_has_column(col_order, "tx");
  assert_has_column(col_order, "ty");
  assert_has_column(col_order, "tz");
  assert_has_column(col_order, "tpx");
  assert_has_column(col_order, "tpy");
  assert_has_column(col_order, "tpz");
  
  // Iterate through file and create a truth object for each row
  // and save it to the truth vector.
  while(std::getline(fin, line)) {
    row.clear();
    std::stringstream s(line);
    while(std::getline(s, word, ',')) { row.push_back(word); }
    
    tr = new tr_T{std::stoi(row[col_order["hit_id"]]),
                  std::stoul(row[col_order["particle_id"]]),
                  std::stof(row[col_order["tx"]]),
                  std::stof(row[col_order["ty"]]),
                  std::stof(row[col_order["tz"]]),
                  std::stof(row[col_order["tpx"]]),
                  std::stof(row[col_order["tpy"]]),
                  std::stof(row[col_order["tpz"]])};
    
    populate_optional_truth_members(col_order, row, tr);
    
    truths->push_back(tr);
  }
}

template<typename cl_T>
void populate_optional_cell_members(std::unordered_map<std::string, int> columns, 
                                      std::vector<std::string> row, cl_T* cl){
  // This function populates optional class members for the cell object
  // depending on if they are present in the given csv
  if(has_column(columns, "timestamp"))
    cl->set_timestamp(std::stoi(row[columns["timestamp"]]));
}

template<typename cl_T>
void load_cells(std::string file_root, std::vector<const cl_T*>* cells) {
  // This function loads cells from a TrackML-like event
  // Note: it is agnostic to column order in order to be more adaptable
  // to datasets from the TrackML challenge or those simulated by
  // ACTS
  
  // Initialize variables
  std::ifstream fin(file_root + "-cells.csv");
  std::vector<std::string> row;
  std::string line, word;
  cl_T* cl;
  
  // Assert the file has been opened
  if(!fin.is_open()){
    std::cerr << "Could not open " << file_root << "-cells.csv" << std::endl;
  }
  
  // Determine column ordering
  std::getline(fin, line);
  std::unordered_map<std::string, int> col_order = get_column_order(line);
  
  // Assert hits csv has required values
  assert_has_column(col_order, "hit_id");
  assert_has_column(col_order, "ch0");
  assert_has_column(col_order, "ch1");
  assert_has_column(col_order, "value");
  
  // Iterate through file and create a cell object for each row
  // and save it to the cell vector.
  while(std::getline(fin, line)) {
    row.clear();
    std::stringstream s(line);
    while(std::getline(s, word, ',')) { row.push_back(word); }
    
    cl = new cl_T{std::stoi(row[col_order["hit_id"]]),
                  std::stoi(row[col_order["ch0"]]),
                  std::stoi(row[col_order["ch1"]]),
                  std::stof(row[col_order["value"]])};
    
    populate_optional_cell_members(col_order, row, cl);
    
    cells->push_back(cl);
  }
}

template<typename pt_T>
void populate_optional_particle_members(std::unordered_map<std::string, int> columns, 
                                                std::vector<std::string> row, pt_T* pt){
  // This function populates optional class members for the particle object
  // depending on if they are present in the given csv
  if(has_column(columns, "particle_type"))
    pt->set_ptype(std::stoi(row[columns["particle_type"]]));
  if(has_column(columns, "process"))
    pt->set_process(std::stoi(row[columns["process"]]));
  if(has_column(columns, "vt"))
    pt->set_vt(std::stof(row[columns["vt"]]));
  if(has_column(columns, "m"))
    pt->set_m(std::stof(row[columns["m"]]));
}

template<typename pt_T>
void load_particles(std::string file_root, std::vector<const pt_T*>* particles) {
  // This function loads particles from a TrackML-like event
  // Note: it is agnostic to column order in order to be more adaptable
  // to datasets from the TrackML challenge or those simulated by
  // ACTS
  
  // Initialize variables
  std::ifstream fin(file_root + "-particles.csv");
  std::vector<std::string> row;
  std::string line, word;
  pt_T* pt;
  
  // Assert the file has been opened
  if(!fin.is_open()){
    std::cerr << "Could not open " << file_root << "-particles.csv" << std::endl;
  }
  
  // Determine column ordering
  std::getline(fin, line);
  std::unordered_map<std::string, int> col_order = get_column_order(line);
  
  // Assert hits csv has required values
  assert_has_column(col_order, "particle_id");
  assert_has_column(col_order, "vx");
  assert_has_column(col_order, "vy");
  assert_has_column(col_order, "vz");
  assert_has_column(col_order, "px");
  assert_has_column(col_order, "py");
  assert_has_column(col_order, "pz");
  assert_has_column(col_order, "q");
  assert_has_column(col_order, "nhits");
  
  // Iterate through file and create a particle object for each row
  // and save it to the cell vector.
  while(std::getline(fin, line)) {
    row.clear();
    std::stringstream s(line);
    while(std::getline(s, word, ',')) { row.push_back(word); }
    
    pt = new pt_T{std::stoul(row[col_order["particle_id"]]),
                  std::stof(row[col_order["vx"]]),
                  std::stof(row[col_order["vy"]]),
                  std::stof(row[col_order["vz"]]),
                  std::stof(row[col_order["px"]]),
                  std::stof(row[col_order["py"]]),
                  std::stof(row[col_order["pz"]]),
                  std::stoi(row[col_order["q"]]),
                  std::stoi(row[col_order["nhits"]])};
    
    populate_optional_particle_members(col_order, row, pt);
    
    particles->push_back(pt);
  }
}

template<typename seed_T>
void save_edges_to_csv(std::vector<seed_T> seeds, std::string output_path) {
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
