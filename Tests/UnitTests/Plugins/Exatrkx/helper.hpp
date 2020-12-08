#include<vector>
#include<string>
#include<fstream>

//#include "Acts/Seeding/Seed.hpp"

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
  // return true if str represents an positive integer
  if (str.empty()) {
    return false;
  }
  return str.find_first_not_of("0123456789") == std::string::npos;
}


bool isFloat(std::string str) {
  // return true if str represents a decimal number
  if (str.empty()) {
      return false;
  }
  return str.find_first_not_of("0123456789.-") == std::string::npos;
}
