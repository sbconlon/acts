#pragma once

struct Volumes {
    // Member variables
    int m_volId;
    int m_layId;
    int m_modId;
    unsigned long m_geoId; 

    // Constructors
    Volumes(int volId, int layId, int modId){
      m_layId = layId;
      m_volId = volId;
      m_modId = modId;
      m_geoId = 0;
    }
    Volumes(int volId, int layId, int modId, unsigned long geoId){
      m_layId = layId;
      m_volId = volId;
      m_modId = modId;
      m_geoId = geoId;
    }

    // Member Functions
    int volId() const { return m_volId; }
    int layId() const { return m_layId; }
    int modId() const { return m_modId; }
    unsigned long geoId() const { return m_geoId; }
    void set_geoId(unsigned long geoId){ m_geoId = geoId; }
};

bool operator==(Volumes a, Volumes b) {
    return (a.volId() == b.volId() && 
            a.layId() == b.layId() &&
            a.modId() == b.modId() &&
            a.geoId() == b.geoId());
}