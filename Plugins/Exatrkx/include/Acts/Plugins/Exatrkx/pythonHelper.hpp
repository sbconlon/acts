#include "Python.h"
#include <vector>

template <typename spacepoint_container_t>
void hits_to_list(spacepoint_container_t& sps, PyListObject* phits){
  // This function translates a container of external spacepoints into a
  // 2D python list

  // Initialize variables
  PyListObject* row;
  PyObject *pX, *pY, *pZ, *pvolId, *playId, *pmodId;
  // Iterate through every spacepoint object
  for(size_t i=0; i<sps.size(); ++i){
    // Initialize a empty sub-list
    row = (PyListObject*) PyList_New(0);
    // Convert spacepoint data to python data
    float x = sps[i].x();
    float y = sps[i].y();
    float z = sps[i].z();
    pX = PyFloat_FromDouble(x);
    pY = PyFloat_FromDouble(y);
    pZ = PyFloat_FromDouble(z);
    pvolId = PyFloat_FromDouble(0);  // Filling volume id with dummy 0
    playId = PyFloat_FromDouble(0);  // Filling layer  id with dummy 0
    pmodId = PyFloat_FromDouble(0);  // Filling module id with dummy 0
    // Fill sub-list with the spacepoint data
    PyList_Append((PyObject*) row, pX);
    PyList_Append((PyObject*) row, pY);
    PyList_Append((PyObject*) row, pZ);
    PyList_Append((PyObject*) row, pvolId);
    PyList_Append((PyObject*) row, playId);
    PyList_Append((PyObject*) row, pmodId);
    // Append sub-list to final list
    PyList_Append((PyObject*) phits, (PyObject*) row);
  }
}

template <typename spacepoint_container_t>
void cells_to_list(spacepoint_container_t& sps, PyListObject* pcells){
  // Because we do not currently have cell information, this list is
  // intentionally filled with zeros
  PyListObject* row;
  float zero = 0.0;
  for(size_t i=0; i<sps.size(); ++i){
    row = (PyListObject*) PyList_New(0);
    for(size_t j=0; j<3; ++j){
      PyList_Append((PyObject*) row, PyFloat_FromDouble(zero));
    }
    PyList_Append((PyObject*) pcells, (PyObject*) row);
  }
}

template <typename spacepoint_container_t>
void hids_to_list(spacepoint_container_t& sps, PyListObject* phids) {
  // Potentially, this function could be merged with hits_to_list to avoid
  // looping through spacepoint container twice.
  for(size_t i=0; i<sps.size(); ++i){
    long id = sps[i].measurementIndex();
    PyList_Append((PyObject*) phids, PyLong_FromLong(id));
  }
}

/*
template <typename external_truth_t>
void truth_to_list(std::vector<external_truth_t>* truth, PyListObject* ptruth){
  // This function translates a vector of truth into a 2D python list
  // Initialize variables
  PyListObject* row;
  PyObject *phid, *ppid, *ptx, *pty, *ptz, *ptpx, *ptpy, *ptpz, *pweight;
  // Iterate through every spacepoint object
  for(size_t i=0; i<truth->size(); ++i){
    // Initialize a empty sub-list
    row = (PyListObject*) PyList_New(0);
    // Convert truth object data to python data
    phid = PyLong_FromLong((*truth)[i]->hid());
    ppid = PyLong_FromUnsignedLong((*truth)[i]->pid());
    ptx = PyFloat_FromDouble((*truth)[i]->tx());
    pty = PyFloat_FromDouble((*truth)[i]->ty());
    ptz = PyFloat_FromDouble((*truth)[i]->tz());
    ptpx = PyFloat_FromDouble((*truth)[i]->tpx());
    ptpy = PyLong_FromDouble((*truth)[i]->tpy());
    ptpz = PyLong_FromDouble((*truth)[i]->tpz());
    pweight = PyLong_FromDouble((*truth)[i]->weight());
    // Fill sub-list with the truth data
    PyList_Append((PyObject*) row, phid);
    PyList_Append((PyObject*) row, ppid);
    PyList_Append((PyObject*) row, ptx);
    PyList_Append((PyObject*) row, pty);
    PyList_Append((PyObject*) row, ptz);
    PyList_Append((PyObject*) row, ptpx);
    PyList_Append((PyObject*) row, ptpy);
    PyList_Append((PyObject*) row, ptpz);
    PyList_Append((PyObject*) row, pweight);
    // Append sub-list to final list
    PyList_Append((PyObject*) ptruth, (PyObject*) row);
  }
}

template <typename external_cell_t>
void cells_to_list(std::vector<external_cell_t>* cells, PyListObject* pcells){
  // This function translates a vector of cells into a 2D python list
  // Initialize variables
  PyListObject* row;
  PyObject *phid, *pch0, *pch1, *pvalue;
  // Iterate through every cell object
  for(size_t i=0; i<cells->size(); ++i){
    // Initialize a empty sub-list
    row = (PyListObject*) PyList_New(0);
    // Convert cell data to python data
    phid = PyLong_FromLong((*cells)[i]->hid());
    pch0 = PyLong_FromLong((*cells)[i]->ch0());
    pch1 = PyLong_FromLong((*cells)[i]->ch1());
    pvalue = PyFloat_FromDouble((*cells)[i]->value());
    // Fill sub-list with the cell data
    PyList_Append((PyObject*) row, phid);
    PyList_Append((PyObject*) row, pch0);
    PyList_Append((PyObject*) row, pch1);
    PyList_Append((PyObject*) row, pvalue);
    // Append sub-list to final list
    PyList_Append((PyObject*) pcells, (PyObject*) row);
  }
}

template <typename external_particle_t>
void particles_to_list(std::vector<external_particle_t>* particles, PyListObject* pparticles){
  // This function translates a vector of particles into a 2D python list
  // Initialize variables
  PyListObject* row;
  PyObject *ppid, *pvx, *pvy, *pvz, *ppx, *ppy, *ppz, *pq, *pnhits;
  // Iterate through every particle object
  for(size_t i=0; i<particles->size(); ++i){
    // Initialize a empty sub-list
    row = (PyListObject*) PyList_New(0);
    // Convert particle data to python data
    ppid = PyLong_FromUnsignedLong((*particles)[i]->pid());
    pvx = PyFloat_FromDouble((*particles)[i]->vx());
    pvy = PyFloat_FromDouble((*particles)[i]->vy());
    pvz = PyFloat_FromDouble((*particles)[i]->vz());
    ppx = PyFloat_FromDouble((*particles)[i]->px());
    ppy = PyFloat_FromDouble((*particles)[i]->py());
    ppz = PyFloat_FromDouble((*particles)[i]->pz());
    pq = PyLong_FromLong((*particles)[i]->q());
    pnhits = PyLong_FromLong((*particles)[i]->nhits());
    // Fill sub-list with the particle data
    PyList_Append((PyObject*) row, ppid);
    PyList_Append((PyObject*) row, pvx);
    PyList_Append((PyObject*) row, pvy);
    PyList_Append((PyObject*) row, pvz);
    PyList_Append((PyObject*) row, ppx);
    PyList_Append((PyObject*) row, ppy);
    PyList_Append((PyObject*) row, ppz);
    PyList_Append((PyObject*) row, pq);
    PyList_Append((PyObject*) row, pnhits);
    // Append sub-list to final list
    PyList_Append((PyObject*) pparticles, (PyObject*) row);
  }
}
*/
