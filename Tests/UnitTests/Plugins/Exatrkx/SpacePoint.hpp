// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

struct Truth {
    // Member Variables
    unsigned long m_hid;
    unsigned long m_pid;

    // Constructors
    Truth(unsigned long hid, unsigned long pid) {
        m_hid = hid;
        m_pid = pid;
    }
    Truth(unsigned long hid) {
        m_hid = hid;
        m_pid = 0;
    }

    // Member Functions
    unsigned long hid() const { return m_hid; }
    unsigned long pid() const { return m_pid; }
    void setPid(unsigned long pid){ m_pid = pid; }
};

struct Volumes {
    // Member variables
    int m_volId;
    int m_layerId;

    // Constructors
    Volums(int layerId, int volId){
      m_layerId = layerId;
      m_volId = volId;
    }

    // Member Functions
    int volId() const { return m_volId; }
    int layerId() const { return m_layerId; }
}

struct SpacePoint {
  // Member variables
  float m_x;
  float m_y;
  float m_z;
  float m_r;
  int m_layer;
  float varianceR=0;
  float varianceZ=0;
  Truth* ids = NULL;
  Volumes* vols = NULL;

  // Constructor
  SpacePoint (float x, float y, float z, int layer) {
      m_x = x;
      m_y = y;
      m_z = z;
      m_layer = layer;
      m_r = std::sqrt(m_x * m_x + m_y * m_y);
  }

  // Member Functions
  float x() const { return m_x; }
  float y() const { return m_y; }
  float z() const { return m_z; }
  float r() const { return m_r; }
  int layer() const { return m_layer; }
};

bool operator==(SpacePoint a, SpacePoint b) {
    if (a.ids != NULL && b.ids != NULL) {
        return (a.ids->hid() == b.ids->hid());
    } else {
        return (a.x() == b.x() &&
                a.y() == b.y() &&
                a.z() == b.z() &&
                a.layer() == b.layer());
    }
}
