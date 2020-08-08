#include "Acts/Plugins/Exatrkx/gnn.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "Python.h"
#include "numpy/arrayobject.h"
#include "numpy/npy_common.h"
#include "numpy/ndarrayobject.h"


static void* wrap_import_array(){
    import_array();
    return NULL;
}

std::vector<std::vector<int>> Acts::prepare_graph(std::vector<float> hits_vect,
                                                  std::string path_to_exatrkx){
    int size = (int) hits_vect.size();
    int nFeatures = 4;
    int nHits = (int) size / nFeatures;

    // Assert hits vector is not empty
    if (nHits == 0){
        throw std::runtime_error("Hits vector is empty");
    }
    
    // Assert hits vector size is divisible by nFeatures
    if (size % nFeatures != 0){
        throw std::runtime_error("Hits vector must be in row-major form and each hit must have (layer, x, y, z) as features.");
    }
    
    PyObject *pName, *pModule, *pFunc;
    PyObject *pValue;
    PyArrayObject *hits_array;
    float* data;

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\".\")");

    pName = PyUnicode_FromString("exatrkxtest");
    if (pName == NULL){
        PyErr_Print();
        throw std::runtime_error("String to unicode conversion failed");
    }
    
    pModule = PyImport_Import(pName);
    wrap_import_array();
    
    Py_DECREF(pName);
    if(pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, "print_array");
        if (pFunc && PyCallable_Check(pFunc)) {
            // Convert C Vector to Numpy Array
            npy_intp dims[2] = {nHits, nFeatures};
            data = (float *) malloc(sizeof(float) * hits_vect.size());
            for (int i=0; i < hits_vect.size(); ++i){
                data[i] = hits_vect[i];
            }
            hits_array = (PyArrayObject *) PyArray_SimpleNewFromData(2, dims, PyArray_FLOAT, (void *) data);
        }
        else {
            PyErr_Print();
            throw std::runtime_error("Failed to load prepare function");
        }
        pValue = PyObject_CallFunctionObjArgs(pFunc, hits_array, NULL);
        //PyObject* elem = PyArray_GETITEM(hits_array, PyArray_DATA(hits_array));
        //std::cout << "elem: " << PyFloat_AsDouble(elem) << std::endl;
        if (pValue != NULL) {
            std::cout << "Call successful!\n";
            Py_DECREF(pValue);
            Py_DECREF(hits_array);
            free(data);
        }
        else {
            Py_DECREF(pFunc);
            Py_DECREF(pModule);
            Py_DECREF(hits_array);
            free(data);
            PyErr_Print();
            throw std::runtime_error("Call failed");
        }
    }
    else {
        PyErr_Print();
        throw std::runtime_error("Failed to load Exatrkx module");
    }
    
    Py_Finalize();

    // Dummy code
    std::vector<std::vector<int>> edges;
    std::vector<int> dummy{0, 1};
    edges.push_back(dummy);

    return edges;
}
