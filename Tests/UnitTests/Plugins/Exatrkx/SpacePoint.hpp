#pragma once
#include<cmath>
#include<vector>

#include "Truth.hpp"
#include "Volumes.hpp"
#include "Cell.hpp"

struct SpacePoint {
  // Private member variables
  int m_hid;
  float m_x;
  float m_y;
  float m_z;
  float m_r;

  // Public member variables
  Truth* tr = NULL;
  Volumes* vols = NULL;
  std::vector<Cell*> cls;
  

  // Constructors
  SpacePoint(float x, float y, float z) {
    m_hid = -1;
    m_x = x;
    m_y = y;
    m_z = z;
    m_r = std::sqrt(m_x * m_x + m_y * m_y);
  }
  SpacePoint(int hid, float x, float y, float z) {
    m_hid = hid;
    m_x = x;
    m_y = y;
    m_z = z;
    m_r = std::sqrt(m_x * m_x + m_y * m_y);
  }

  // Member Functions
  int hid() const { return m_hid; }
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float r() const { return m_r; }
};

bool operator==(SpacePoint a, SpacePoint b) {
    if (a.tr != NULL && b.tr != NULL) {
        return (a.tr->hid() == b.tr->hid());
    } else {
        return (a.x() == b.x() &&
                a.y() == b.y() &&
                a.z() == b.z());
    }
}
