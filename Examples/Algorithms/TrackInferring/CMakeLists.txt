add_library(
  ActsExamplesTrackInferring SHARED
  src/TrackInferringAlgorithm.cpp
  src/TrackInferringAlgorithmFunction.cpp
  src/TrackInferringOptions.cpp)
target_include_directories(
  ActsExamplesTrackInferring
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(
  ActsExamplesTrackInferring
  PUBLIC
    ActsCore
    ActsExamplesFramework ActsExamplesMagneticField
    Boost::program_options)

install(
  TARGETS ActsExamplesTrackInferring
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
