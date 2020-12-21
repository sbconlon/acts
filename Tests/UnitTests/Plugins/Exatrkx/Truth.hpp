#pragma once
struct Truth {
  // Member Variables
  int m_hid;
  unsigned long m_pid=0;
  unsigned long m_geoId=0;
  float m_tx=0;
  float m_ty=0;
  float m_tz=0;
  float m_tpx=0;
  float m_tpy=0;
  float m_tpz=0;
  float m_weight=0;
  float m_tt=0;
  float m_te=0;
  float m_deltapx=0;
  float m_deltapy=0;
  float m_deltapz=0;
  float m_deltae=0;
  int m_index=0;
  
  // Constructors
  Truth(int hid) {
    m_hid = hid;
  }
  Truth(int hid, unsigned long pid) {
    m_hid = hid;
    m_pid = pid;
  }
  Truth(int hid, unsigned long pid, float tx, float ty, float tz,
        float tpx, float tpy, float tpz){
    m_hid = hid;
    m_pid = pid;
    m_tx = tx;
    m_ty = ty;
    m_tz = tz;
    m_tpx = tpx;
    m_tpy = tpy;
    m_tpz = tpz;
  }

  // Member Functions
  int hid() const { return m_hid; }
  unsigned long pid() const { return m_pid; }
  unsigned long geoId() const { return m_geoId; }
  float tx() const { return m_tx; }
  float ty() const { return m_ty; }
  float tz() const { return m_tz; }
  float tpx() const { return m_tpx; }
  float tpy() const { return m_tpy; }
  float tpz() const { return m_tpz; }
  float weight() const { return m_weight; }
  float tt() const { return m_tt; }
  float te() const { return m_te; }
  float deltapx() const { return m_deltapx; }
  float deltapy() const { return m_deltapy; }
  float deltapz() const { return m_deltapz; }
  float deltae() const { return m_deltae; }
  int index() const { return m_index; }
    
  void set_pid(unsigned long pid){ m_pid = pid; }
  void set_geoId(unsigned long geoId){ m_geoId = geoId; }
  void set_tx(float tx){ m_tx = tx; }
  void set_ty(float ty){ m_ty = ty; }
  void set_tz(float tz){ m_tz = tz; }
  void set_tpx(float tpx){ m_tpx = tpx; }
  void set_tpy(float tpy){ m_tpy = tpy; }
  void set_tpz(float tpz){ m_tpz = tpz; }
  void set_weight(float weight){ m_weight = weight; }
  void set_tt(float tt){ m_tt = tt; }
  void set_te(float te){ m_te = te; }
  void set_deltapx(float deltapx){ m_deltapx = deltapx; }
  void set_deltapy(float deltapy){ m_deltapy = deltapy; }
  void set_deltapz(float deltapz){ m_deltapz = deltapz; }
  void set_deltae(float deltae){ m_deltae = deltae; }
  void set_index(int index){ m_index = index; }
};