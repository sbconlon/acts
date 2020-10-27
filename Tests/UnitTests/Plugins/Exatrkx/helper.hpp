#include<vector>
#include<string>
#include<fstream>

#include "Acts/Seeding/Seed.hpp"

/*#include "Python.h"
#include "numpy/arrayobject.h"
#include "numpy/npy_common.h"
#include "numpy/ndarrayobject.h"*/

#include "SpacePoint.hpp"


/*int save_edges_to_pandas_df(std::vector<Acts::Seed<SpacePoint>> seeds, const char *output_path) {

  // Assert seeds are well formatted
  if (seeds[0]->sp()[0]->ids == NULL) { throw std::runtime_error("Seeds must contain hit id information"); }

  // Variable declarations
  PyObject *pName, *pModule, *pFunc;
  PyObject *pOutpath;
  PyArrayObject *pTplets;
  unsigned long evtid = 0 // TODO: allow for user defined event ids
  unsigned long* edge_array = (unsigned long *) malloc(4 * sizeof(unsigned long) * seeds.size());

  // Convert Seed object vector to C array
  int idx_counter = 0;
  for (auto it=seeds.begin(); it!=seeds.end(); ++it) {
    edge_array[4*idx_counter] = evtid;
    edge_array[4*idx_counter+1] = it->sp()[0]->ids->hid();
    edge_array[4*idx_counter+2] = it->sp()[1]->ids->hid();
    edge_array[4*idx_counter+3] = it->sp()[2]->ids->hid();
    idx_counter++;
  }

  // Initialize Python session
  Py_Initialize();
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append(\".\")");

  // Import Exatrkx Python module
  pName = PyUnicode_FromString("cEmbedded.wrap_prepare");
  if (pName == NULL){
    PyErr_Print();
    throw std::runtime_error("String to unicode conversion failed");
  }
  pModule = PyImport_Import(pName);
  wrap_import_array();
  Py_DECREF(pName);

  if(pModule != NULL) {
    // Import python wrapper function
    pFunc = PyObject_GetAttrString(pModule, "save_edges_as_df");
    if(pFunc && PyCallable_Check(pFunc)) {
      // Convert C Array to Numpy Array
      npy_intp dims[2] = {seeds.size(), 4};
      pTplets = (PyArrayObject *) PyArray_SimpleNewFromData(2, dims, PyArray_ULONG, (void *) edge_array);
      pOutpath = PyUnicode_FromString(output_path);
    } else {
      PyErr_Print();
      throw std::runtime_error("Failed to load edges_as_df function from Exatrkx module");
    }
    PyObject_CallFunctionObjArgs(pFunc, pTplets, pOutpath);
  } else {
    PyErr_Print();
    throw std::runtime_error("Failed to load Exatrkx module");
  }
  free(edge_array);
  Py_Finalize();
  return 1;
}*/



void save_edges_to_csv(std::vector<Acts::Seed<SpacePoint>> seeds, std::string output_path) {
  std::fstream outfile;
  outfile.open(output_path + "-edges.csv", std::fstream::out); // TODO: allow user to define eventid and graph id
  int evtid = 0; // TODO: allow for user defined evtid
  for(auto it=seeds.begin(); it!=seeds.end(); ++it){
    outfile << evtid << ',';
    outfile << it->sp()[0]->ids->hid() << ',';
    outfile << it->sp()[1]->ids->hid() << ',';
    outfile << it->sp()[2]->ids->hid() << std::endl;
  }
  outfile.close();
}
