#include "Acts/Seeding/Seed.hpp"
#include "Acts/Seeding/InternalSeed.hpp"
#include "Acts/Seeding/InternalSpacePoint.hpp"
#include "Acts/Seeding/SeedFilter.hpp"
#include "Acts/Seeding/SeedfinderConfig.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "Python.h"
#include "numpy/arrayobject.h"
#include "numpy/npy_common.h"
#include "numpy/ndarrayobject.h"


namespace Acts {

    template <typename external_spacepoint_t>
    std::vector<Acts::Seed<external_spacepoint_t>> Acts::prepareTripletGraph(const auto first,
                                                                             const auto last,
                                                                             const size_t nhits,
                                                                             const char *config_path,
                                                                             const char *filename,
                                                                             long verbose,
                                                                             long show_config){

        // Variable declarations
        PyObject *pName, *pModule, *pFunc;
        PyArrayObject *pHits, *pTruth, *pEdgeIdxs;
        PyObject *pConfigPath, *pVerbose, *pShowConfig, *pFilename;
        float *hitData;
        unsigned long *truthData = NULL;
        const int* edgeIdxs = NULL;
        int nEdges=0;
        std::vector<Acts::Seed<external_spacepoint_t>> edges;
        bool hasHitID, hasPrtID;

        // Determine if SpacePoint obj has valid truth info
        if((*first)->ids == NULL){
            hasHitID = false;
            hasPrtID = false;
        } else {
            hasHitID = true;
            if((*first)->ids->pid() > 0) {
                hasPrtID = true;
            } else {
                hasHitID = false;
            }
        }

        // Split SpacePoint vector into hit and truth arrays
        int nHitColumns = 6;
        int nTruthColumns = 2;
        hitData = (float *) malloc(sizeof(float) * nhits * nHitColumns);
        if (hasPrtID) { truthData = (unsigned long *) malloc(sizeof(unsigned long) * nhits * nTruthColumns); }
        auto current = first;
        int i=0;
        for (; current != last; current++) {
            if (hasHitID && hasPrtID) {
                hitData[nHitColumns*i] = (float) (*current)->ids->hid();
                truthData[nTruthColumns*i] = (*current)->ids->hid();
                truthData[nTruthColumns*i + 1] = (*current)->ids->pid();


            } else if(hasHitID && !hasPrtID) {
                hitData[nHitColumns * i] = (float) (*current)->ids->hid();

            } else if (!hasHitID && !hasPrtID) {
                hitData[nHitColumns * i] = (float) i;
            }

            //hitData[nHitColumns * i + 1] = (*current)->layer();
            hitData[nHitColumns * i + 2] = (*current)->x();
            hitData[nHitColumns * i + 3] = (*current)->y();
            hitData[nHitColumns * i + 4] = (*current)->z();
            hitData[nHitColumns * i + 5] = (*current)->r();
            ++i;
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
            pFunc = PyObject_GetAttrString(pModule, "wrap_prepare_triplets");
            if (pFunc && PyCallable_Check(pFunc)) {

                // Convert C Arrays to Numpy Arrays
                npy_intp htDims[2] = {nhits, nHitColumns};
                npy_intp trDims[2] = {nhits, nTruthColumns};
                pHits = (PyArrayObject *) PyArray_SimpleNewFromData(2, htDims, PyArray_FLOAT, (void *) hitData);
                if (hasPrtID) {
                    pTruth = (PyArrayObject *) PyArray_SimpleNewFromData(2, trDims, PyArray_ULONG, (void *) truthData);
                }

                // Convert keyword args to python objects
                pConfigPath = PyUnicode_FromString(config_path);
                pFilename = PyUnicode_FromString(filename);
                pVerbose = PyBool_FromLong(verbose);
                pShowConfig = PyBool_FromLong(show_config);

            }
            else {
                PyErr_Print();
                throw std::runtime_error("Failed to load prepare function");
            }
            if(hasPrtID) {
                pEdgeIdxs = (PyArrayObject *) PyObject_CallFunctionObjArgs(pFunc,
                                                                           pHits,
                                                                           pTruth,
                                                                           pConfigPath,
                                                                           pFilename,
                                                                           pVerbose,
                                                                           pShowConfig, NULL);
            } else {
                pEdgeIdxs = (PyArrayObject *) PyObject_CallFunctionObjArgs(pFunc,
                                                                           pHits,
                                                                           Py_None,
                                                                           pConfigPath,
                                                                           pFilename,
                                                                           pVerbose,
                                                                           pShowConfig, NULL);
            }

            if (pEdgeIdxs != NULL) {

                // Free old memory
                free(hitData);
                free(truthData);
                Py_DECREF(pHits);
                Py_DECREF(pTruth);
                Py_DECREF(pConfigPath);
                Py_DECREF(pVerbose);
                Py_DECREF(pShowConfig);

                // Use edge idxs to populate seed vector
                npy_intp *edgeDims = PyArray_DIMS(pEdgeIdxs);
                int nEdges = edgeDims[0];
                for(int j=0; j < nEdges; ++j) {
                    auto spIter = first;
                    int bIdx = *((int *) PyArray_GETPTR2(pEdgeIdxs, j, 0));
                    std::advance(spIter, bIdx);
                    const external_spacepoint_t &bSp = **(spIter);
                    spIter = first;
                    int mIdx = *((int *) PyArray_GETPTR2(pEdgeIdxs, j, 1));
                    std::advance(spIter, mIdx);
                    const external_spacepoint_t &mSp = **(spIter);
                    spIter = first;
                    int tIdx = *((int *) PyArray_GETPTR2(pEdgeIdxs, j, 2));
                    std::advance(spIter, tIdx);
                    const external_spacepoint_t &tSp = **(spIter);
                    edges.push_back(Acts::Seed<external_spacepoint_t>(bSp, mSp, tSp, 0));
                }

                Py_DECREF(pEdgeIdxs);
            }
            else {
                free(hitData);
                free(truthData);
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                Py_DECREF(pHits);
                Py_DECREF(pTruth);
                Py_DECREF(pConfigPath);
                Py_DECREF(pFilename);
                Py_DECREF(pVerbose);
                Py_DECREF(pShowConfig);
                PyErr_Print();
                throw std::runtime_error("Call failed");
            }
        }
        else {
            PyErr_Print();
            throw std::runtime_error("Failed to load Exatrkx module");
        }

        Py_Finalize();

        return edges;
    }
}
