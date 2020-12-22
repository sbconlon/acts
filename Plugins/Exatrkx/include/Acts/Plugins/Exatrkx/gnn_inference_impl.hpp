#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "Python.h"
#include "numpy/arrayobject.h"
#include "numpy/npy_common.h"
#include "numpy/ndarrayobject.h"

#include "pythonHelper.hpp"


namespace Acts {

  template <typename external_spacepoint_t, 
            typename external_truth_t,
            typename external_cell_t, 
            typename external_particle_t,
            typename external_track_t>
  void Acts::inferTracks(std::vector<const external_spacepoint_t*>* hits,
                         std::vector<const external_truth_t*>* truth,
                         std::vector<const external_cell_t*>* cells,
                         std::vector<const external_particle_t*>* particles,
                         std::vector<external_track_t*>* tracks){

    // Variable declarations
    PyObject *pName, *pModule, *pFunc;
    PyObject *pTracks;
    PyListObject *pHits, *pTruth, *pCells, *pParticles;
    
    // Initialize Python session
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");
    
    // Import Exatrkx Python module
    pName = PyUnicode_FromString("wrap_inference");
    if (pName == NULL){
      PyErr_Print();
      throw std::runtime_error("String to unicode conversion failed");
    }
    pModule = PyImport_Import(pName);
    wrap_import_array();
    Py_DECREF(pName);

    if(pModule != NULL) {
      // Import python function
      pFunc = PyObject_GetAttrString(pModule, "test_inference");
      if (pFunc && PyCallable_Check(pFunc)) {
        // Initialize Python lists
        pHits = (PyListObject*) PyList_New(0);
        pTruth = (PyListObject*) PyList_New(0);
        pCells = (PyListObject*) PyList_New(0);
        pParticles = (PyListObject*) PyList_New(0);
        
        // Convert C vectors to Python lists
        hits_to_list(hits, pHits);
        truth_to_list(truth, pTruth);
        cells_to_list(cells, pCells);
        particles_to_list(particles, pParticles);
      
      } else {
        PyErr_Print();
        throw std::runtime_error("Failed to load inference function");
      }
      
      pTracks = PyObject_CallFunctionObjArgs(pFunc, pHits, pTruth, pCells, pParticles, NULL);  
      
      if(PyList_Check(pTracks)){
        PyObject* ptrack;
        for(Py_ssize_t i=0; i<PyList_Size(pTracks); ++i){
	  ptrack = PyList_GET_ITEM(pTracks, i);
	  if(!PyList_Check(ptrack)){
	    throw std::runtime_error("The returned value of the inference function must be a 2D list");
	  }
	  PyObject* phid;
	  external_track_t* track = new external_track_t;
	  for(Py_ssize_t j=0; j<PyList_Size(ptrack); ++j){
	    phid = PyList_GET_ITEM(ptrack, j);
	    if(!PyLong_Check(phid)){
	      throw std::runtime_error("The elements of the sub-lists must be integers");
	    }
	    int hid = PyLong_AsLong(phid);
	    track->add(hid);
	  }
	  tracks->push_back(track);
       	}	
      }
      
    }else {
      PyErr_Print();
      throw std::runtime_error("Failed to load Exatrkx module");
    }

      Py_Finalize();
    }
}
