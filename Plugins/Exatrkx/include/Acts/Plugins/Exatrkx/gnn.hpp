#pragma once

#include "pythonHelper.hpp"

#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/InternalSeed.hpp"
#include "Acts/Seeding/InternalSpacePoint.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"
#include "Acts/Utilities/Result.hpp"

#include "Python.h"

#include <iterator>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>


namespace Acts{

  struct GraphNeuralNetworkOptions {

    /// Graph Neural Network Options
    ///
    /// @param moduleName Name of the ML Python module that should be imported
    /// @param funcName Name of the Python function in the module which should
    ///                 should be used to find tracks.
    GraphNeuralNetworkOptions(std::string moduleName, std::string funcName)
      : mlModuleName(moduleName), mlFuncName(funcName){}

    /// Python module name
    std::string mlModuleName;
    /// Python function inside the module that performs tracking finding
    std::string mlFuncName;
  };

  template <typename index_t>
  struct GraphNeuralNetworkResult {
    /// result of inference pipeline, list of spacepoints in a track
    std::vector<index_t> spTrack;
  };

  class GraphNeuralNetwork {
    public:
      /// Default constructor is deleted
      //GraphNeuralNetwork() = delete;
      /// Constructor
      //GraphNeuralNetwork() { return; }

      /// Graph neural network inference implementation,
      /// calls the Python inference pipeline.
      ///
      /// @tparam spacepoint_container_t Type of the spacepoint container
      ///
      /// @return a container of infer track results
      template<typename spacepoint_container_t, typename index_t=uint32_t>
      std::vector<Result<GraphNeuralNetworkResult<index_t>>>
      inferTracks(const spacepoint_container_t& hits,
                  const GraphNeuralNetworkOptions& ifOptions) const {

        //using Spacepoint = typename spacepoint_container_t::value_type;

        // Variable declarations
        PyObject *pName, *pModule, *pFunc;
        PyObject *pTracks;
        ///PyListObject *pHits, *pTruth, *pCells, *pParticles;
        PyListObject *pHids, *pHits, *pCells;
        std::vector<Result<GraphNeuralNetworkResult<index_t>>> res_vect;

        // Initialize Python session
        Py_Initialize();
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("sys.path.append(\".\")");

        // Import Exatrkx Python module
        pName = PyUnicode_FromString(&(ifOptions.mlModuleName[0]));
        if (pName == NULL){
          PyErr_Print();
          throw std::runtime_error("String to unicode conversion failed");
        }
        pModule = PyImport_Import(pName);
        Py_DECREF(pName);

        if(pModule != NULL) {
          // Import python function
          pFunc = PyObject_GetAttrString(pModule, &(ifOptions.mlFuncName[0]));
          if (pFunc && PyCallable_Check(pFunc)) {
            // Initialize Python lists
            pHids = (PyListObject*) PyList_New(0);
            pHits = (PyListObject*) PyList_New(0);
            pCells = (PyListObject*) PyList_New(0);

            // Convert C vectors to Python lists
            hids_to_list(hits, pHids);
            hits_to_list(hits, pHits);
            cells_to_list(hits, pCells);

          } else {
            PyErr_Print();
            throw std::runtime_error("Failed to load track finding function");
          }

          pTracks = PyObject_CallFunctionObjArgs(pFunc, pHids, pHits, pCells, NULL);

          if(PyList_Check(pTracks)){
            PyObject *pTrack, *pSpIdx;
            for(Py_ssize_t i=0; i<PyList_Size(pTracks); ++i){
    	        pTrack = PyList_GET_ITEM(pTracks, i);
    	        if(!PyList_Check(pTrack)){
    	          throw std::runtime_error("The returned value of the inference function must be a 2D list");
    	        }
                GraphNeuralNetworkResult<index_t> res_track;
    	        for(Py_ssize_t j=0; j<PyList_Size(pTrack); ++j){
    	          pSpIdx = PyList_GET_ITEM(pTrack, j);
    	          if(!PyLong_Check(pSpIdx)){
    	            throw std::runtime_error("The elements of the sub-lists must be integers");
    	          }
    	          res_track.spTrack.emplace_back((index_t) PyLong_AsLong(pSpIdx));
    	        }
    	        res_vect.push_back(res_track);
           	}
          }

        }else {
          PyErr_Print();
          throw std::runtime_error("Failed to load inference python module");
        }
          Py_Finalize();
          return res_vect;
      }

      void print_hello();
  };

} // namespace Acts

//#include "Acts/Plugins/Exatrkx/gnn.ipp"
