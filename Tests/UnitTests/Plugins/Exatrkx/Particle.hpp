#pragma once

#include "SpacePoint.hpp"

struct Particle{
  // Member Variables
  unsigned long m_pid;
  float m_vx;
  float m_vy;
  float m_vz;
  float m_px;
  float m_py;
  float m_pz;
  int m_q;
  int m_nhits;
  int m_ptype = 0;
  int m_process = 0;
  float m_vt = 0;
  float m_m = 0;
  
  // Public Members
  std::vector<const SpacePoint*> sps;
  
  //Constructor
  Particle(unsigned long pid, float vx, float vy, float vz, 
                  float px, float py, float pz, int q, int nhits){
    m_pid = pid;
    m_vx = vx;
    m_vy = vy;
    m_vz = vz;
    m_px = px;
    m_py = py;
    m_pz = pz;
    m_q = q;
    m_nhits = nhits;
  }
  
  // Member Functions
  unsigned long pid() const { return m_pid; }
  float vx() const { return m_vx; }
  float vy() const { return m_vy; }
  float vz() const { return m_vz; }
  float px() const { return m_px; }
  float py() const { return m_py; }
  float pz() const { return m_pz; }
  int q() const { return m_q; }
  int nhits() const { return m_nhits; }
  int ptype() const { return m_ptype; }
  int process() const { return m_process; }
  float vt() const { return m_vt; }
  float m() const { return m_m; }
  void set_ptype(int ptype) { m_ptype = ptype; }
  void set_process(int process) { m_process = process; }
  void set_vt(float vt) { m_vt = vt; }
  void set_m(float m) { m_m = m; }
};