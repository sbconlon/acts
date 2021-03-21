#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "Python.h"

#include "pythonHelper.hpp"


namespace Acts {

  template<typename spacepoint_container_t>
  std::vector<Result<GraphNeuralNetworkResult<
      typename spacepoint_container_t::value_type>>>
  inferTracks(const spacepoint_container_t& hits,
              const GraphNeuralNetworkOptions& ifOptions) const {

    using Spacepoint = typename spacepoint_container_t::value_type;

    // Variable declarations
    PyObject *pName, *pModule, *pFunc;
    PyObject *pTracks;
    ///PyListObject *pHits, *pTruth, *pCells, *pParticles;
    PyListObject *pHits;
    std::vector<Result<GraphNeuralNetworkresult<Spacepoint>>> res_vect;

    // Initialize Python session
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    // Import Exatrkx Python module
    pName = PyUnicode_FromString(ifOptions->mlModuleName);
    if (pName == NULL){
      PyErr_Print();
      throw std::runtime_error("String to unicode conversion failed");
    }
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if(pModule != NULL) {
      // Import python function
      pFunc = PyObject_GetAttrString(pModule, ifOptions->mlFuncName);
      if (pFunc && PyCallable_Check(pFunc)) {
        // Initialize Python lists
        pHits = (PyListObject*) PyList_New(0);
        //pTruth = (PyListObject*) PyList_New(0);
        //pCells = (PyListObject*) PyList_New(0);
        //pParticles = (PyListObject*) PyList_New(0);

        // Convert C vectors to Python lists
        hits_to_list(hits, pHits);
        //truth_to_list(truth, pTruth);
        //cells_to_list(cells, pCells);
        //particles_to_list(particles, pParticles);

      } else {
        PyErr_Print();
        throw std::runtime_error("Failed to load track finding function");
      }

      pTracks = PyObject_CallFunctionObjArgs(pFunc, pHits, pCells, pTruth, NULL);

      if(PyList_Check(pTracks)){
        PyObject* pTrack, pSpIdx;
        for(Py_ssize_t i=0; i<PyList_Size(pTracks); ++i){
	        pTrack = PyList_GET_ITEM(pTracks, i);
	        if(!PyList_Check(pTrack)){
	          throw std::runtime_error("The returned value of the inference function must be a 2D list");
	        }
          Result<GraphNeuralNetworkresult<Spacepoint>> res;
	        for(Py_ssize_t j=0; j<PyList_Size(ptrack); ++j){
	          pSpIdx = PyList_GET_ITEM(ptrack, j);
	          if(!PyLong_Check(pSpIdx)){
	            throw std::runtime_error("The elements of the sub-lists must be integers");
	          }
	          res.track.emplace_back(hits[PyLong_AsLong(pSpIdx)]);
	        }
	        res_vect.push_back(res);
       	}
      }

    }else {
      PyErr_Print();
      throw std::runtime_error("Failed to load inference python module");
    }
      Py_Finalize();
    }
    return res_vect;
}
