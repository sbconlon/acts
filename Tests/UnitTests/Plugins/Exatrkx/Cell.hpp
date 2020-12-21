#pragma once
struct Cell{
  // Member Variables
  int m_hid;
  int m_ch0;
  int m_ch1;
  float m_value;
  int m_timestamp;
  
  // Constructors
  Cell(int hid, int ch0, int ch1, float value){
    m_hid = hid;
    m_ch0 = ch0;
    m_ch1 = ch1;
    m_value = value;
    m_timestamp = 0;
  }
  Cell(int hid, int ch0, int ch1, float value, int timestamp){
    m_hid = hid;
    m_ch0 = ch0;
    m_ch1 = ch1;
    m_value = value;
    m_timestamp = timestamp;
  }
    
  // Member Functions
  int hid() const { return m_hid; }
  int ch0() const { return m_ch0; }
  int ch1() const { return m_ch1; }
  float value() const { return m_value; }
  int timestamp() const { return m_timestamp; }
  
  void set_timestamp(int timestamp){ m_timestamp = timestamp; }
};