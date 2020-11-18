# Exatrkx Plugin

## Compiling Acts on Cori

  1) Load modules
  ```sh
  module load gcc/8.3.0
  module load cuda/10.2.89
  module load tbb
  module load boost
  module load eigen3
  module load cmake
  export CMAKE_PREFIX_PATH=$EIGEN3_DIR:$CMAKE_PREFIX_PATH
  ```
  2) Build and Load Root Module
  This step is necessary because the default root module on Cori is too new for ACTS
  ```sh
  mkdir modulefiles && cd modulefiles
  mkdir root && cd root
  ```
  Create a file called "6.14.08" and copy and paste the following:
  ```
  #%Module2.0

  conflict root

  # prereq
  if { ![is-loaded gcc/8.3.0] } {
    module load gcc/8.3.0
  }

  set _module_name  [module-info name]
  set is_module_rm  [module-info mode remove]
  set sys        [uname sysname]
  set os         [uname release]

  if { [module-info mode load] } {
     puts stderr "loading $_module_name"
  } elseif { [module-info mode remove] } {
     puts stderr "unloading $_module_name"
  }

  set ROOT_VERSION v6-14-08
  set GCC_LEVEL gcc83
  set ROOT_PATH /global/cfs/cdirs/atlas/leggett/root/${ROOT_VERSION}_${GCC_LEVEL}

  prepend-path PATH $ROOT_PATH/bin
  prepend-path LD_LIBRARY_PATH $ROOT_PATH/lib
  prepend-path CMAKE_PREFIX_PATH $ROOT_PATH

  setenv ROOTDIR $ROOT_PATH
  ```
  then do
  ```sh
  export MODULEPATH=~/modulefiles:$MODULEPATH
  module load root/6.14.08
  ```
  
  3) Build ACTS
  ```sh
  cmake -DCMAKE_INSTALL_PREFIX=${ACTSROOT}/inst \
  -DACTS_BUILD_EXAMPLES=ON -DACTS_BUILD_UNITTESTS=ON -DACTS_BUILD_TGEO_PLUGIN=OFF \
  -DACTS_BUILD_EXATRKX_PLUGIN=ON \
  $ACTSSRC

  make -j30
  ```

## Functionality

  - `Acts::prepareDoubletGraph<T>`
  TODO: Fill in parameter definitions and explain return value
  - `Acts::prepareTripletGraph<T>`
  TODO: Fill in parameter definitions and explain return value
  - `Acts::inferTriplets<T>`
  TODO: Watch this space
  - `Acts::inferTracks<T>`
  TODO: Watch this space

## Running Exatrkx Seeding Tests

  1) Add the location of `exatrkx-work/gnn_pytorch` directory to the `PYTHONPATH` environent variable.
  ```sh
  export PYTHONPATH=/path/to/exatrkx-work/gnn_pytorch:$PYTHONPATH
  ```
  
  2) Add the config file to the same directory as the `ActsUnitTestExatrkxSeeding` executable
  
  3) Run
  
  For TrackML data format:
  ```sh
  ./ActsUnitTestExatrkxSeeding -f /path/to/data/event000000000 -t TrackML
  ```
  
  For lxyz data format:
  ```sh
  ./ActsUnitTestExatrkxSeeding -f /path/to/data/file -t lxyz
  ```
  
  - `-f`: path to file containing data. If using the TrackML, append `event<EVENT ID>` to the end of the path.
  - `-t`: specifies the format of the data, either TrackML or lxyz
  - `-s`: path to save directory. Saves the triplets as three hit id lists in csv format.
