#pragma once

#include<vector>

struct Track{
  // Member Variables
  std::vector<int> hids;
  
  // Member Functions
  int nhits() { return hids.size(); }
  void add(int hid) { hids.push_back(hid); }
};