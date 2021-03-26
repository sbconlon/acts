# ACTS-Exatrkx Integration


## Table of Contents
* [General Info](#general-info)
* [Cori Setup](#cori-setup)
   - [Starting CVMFS Container](#starting-cvmfs-container)
   - [Starting Python VirtualEnv](#starting-python-virtualenv)
* [Install](#install)
   - [Installing Exatrkx-Iml2020](#installing-exatrkx-inference-pipeline)
   - [Compiling ACTS](#compiling-acts)
* [Getting Started](#getting-started)
   - [Simulating Basic Dataset](#simulating-dataset)
   - [Run Track Reconstruction Pipeline](#run-track-reconstruction-pipeline)
* [Documentation](#documentation)
   - [Track Reconstruction Pipeline](#track-reconstruction-pipeline)
   - [InferTracks](#infertracks)
   - [GraphNeuralNetwork](#graphneuralnetwork-class)


## General Info
The goal of this project is to integrate the Exatrkx inference pipeline into ACTS. This is a working repository and is still under development.


## Cori Setup
This section gives step by step instructions for installing dependencies for ACTS and Exatrkx on Cori.

### Starting CVMFS Container
First time setup - only run the following commands once:
```
rm -rf ~/.alrb
chmod g+s $HOME
setfacl -d -m u:nobody:rx $HOME
setfacl -m u:nobody:rx .
```
Every time you log in:
```
source /global/project/projectdirs/atlas/scripts/setupATLAS.sh
setupATLAS -c centos7+batch
```
You should now be running inside of a container. To load the ACTS dependencies, run the following:
```
source /cvmfs/sft.cern.ch/lcg/views/LCG_97apython3/x86_64-centos7-gcc9-opt/setup.sh
export PATH=/cvmfs/sft.cern.ch/lcg/releases/CMake/3.17.3-75516/x86_64-centos7-gcc9-opt/bin:$PATH
```
You should now have a working container with access to all neccessary ACTS dependencies.

### Starting Python VirtualEnv
We now must create a Python virtual environment to give us the freedom to install Exatrkx's dependencies.
```
python -m venv inference-pipeline-venv
```
Then to load the virtual environment:
```
source inference-pipeline-venv/bin/activate
```
When the container from the previous is initialized, it makes edits to Python environment variables which will become a problem later. These are removed with the following commands:
```
unset PYTHONPATH
export LD_LIBRARY_PATH=/cvmfs/sft.cern.ch/lcg/releases/java/8u222-884d8/x86_64-centos7-gcc9-opt/jre/lib/amd64: \
                       /cvmfs/sft.cern.ch/lcg/views/LCG_97apython3/x86_64-centos7-gcc9-opt/lib64: \
                       /cvmfs/sft.cern.ch/lcg/views/LCG_97apython3/x86_64-centos7-gcc9-opt/lib: \
                       /cvmfs/sft.cern.ch/lcg/releases/gcc/9.2.0-afc57/x86_64-centos7/lib: \
                       /cvmfs/sft.cern.ch/lcg/releases/gcc/9.2.0-afc57/x86_64-centos7/lib64: \
                       /cvmfs/sft.cern.ch/lcg/releases/binutils/2.30-e5b21/x86_64-centos7/lib: \
                       /cvmfs/sft.cern.ch/lcg/releases/R/3.6.3-ca0ad/x86_64-centos7-gcc9-opt/lib64/R/library/readr/rcon
```
You should now have a container loaded with ACTS dependencies and a Python virtual environment ready for Exatrkx's dependencies.


## Install
This section guides the user through the installation of ACTS and Exatrkx's inference pipeline. It is assumed the user already has ACTS's dependencies installed.

### Installing Exatrkx Inference Pipeline
Go to https://github.com/exatrkx/exatrkx-iml2020 and follow the setup steps.
Now add `inference_fn.py` to the `PYTHONPATH` environment variable.
```
export PYTHONPATH=/path/to/exatrkx-iml2020/notebooks:$PYTHONPATH
```
Finally, you should be able to run the following command from any directory without error:
```
python -c "from inference_fn import gnn_track_finding"
```

### Compiling ACTS
From your build directory:
```
cmake -DACTS_BUILD_EXAMPLES=ON -DACTS_BUILD_UNITTESTS=ON -DACTS_BUILD_EXAMPLES_EXATRKX=ON -DACTS_BUILD_PLUGIN_EXATRKX=ON /path/to/acts-source
make -j30
```
Or if you are running inside a Python virtual environment:
```
cmake -DACTS_BUILD_EXAMPLES=ON -DACTS_BUILD_UNITTESTS=ON -DACTS_BUILD_EXAMPLES_EXATRKX=ON -DACTS_BUILD_PLUGIN_EXATRKX=ON -DPython3_FIND_VIRTUALENV=ONLY /path/to/acts-source
```
You are now ready to reconstruct tracks.


## Getting Started
This section guides a user through running the ACTS+Exatrkx example reconstruction pipeline on a basic dataset.

### Simulating a Basic Dataset
Simulate single muon events inside the generic, TrackML-like detector. From the bin directory:
```
./ActsExampleFatrasGeneric \
    --output-dir=/path/to/data/sim-generic/single-muon \
    --output-csv \
    --events=10 \
    --bf-value=0 0 2
```

### Run Track Reconstruction Pipeline
Run the ACTS+Exatrkx example reconstruction pipeline.
```
./ActsExampleGNNTracksGeneric \
    --input-dir=/path/to/data/sim-generic/single-muon \
    --output-dir=/path/to/output \
    --digi-smear \
    --digi-config-file=/path/to/acts-source/Examples/Algorithms/Digitization/share/default-smearing-config-generic.json \
    --ml-module-name=inference_fn \
    --ml-function-name=gnn_track_finding \
    -N 1 -j 1
```
Performance plots are written to given output directory. 

## Documentation
### Track Reconstruction Pipeline
### InferTracks
### GraphNeuralNetwork Class
