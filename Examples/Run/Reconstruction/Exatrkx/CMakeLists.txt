add_executable(
  ActsExampleGNNTracksGeneric
  GenericRecGNNTracks.cpp)
target_link_libraries(
  ActsExampleGNNTracksGeneric
  PRIVATE
    ActsExamplesFramework
    ActsExamplesCommon
    ActsExamplesDigitization
    ActsExamplesTrackFitting
    ActsExamplesTrackFinding
    ActsExamplesTrackInferring
    ActsExamplesMagneticField
    ActsExamplesTruthTracking
    ActsExamplesIoCsv
    ActsExamplesIoPerformance
    ActsExamplesDetectorGeneric)

install(
  TARGETS
    ActsExampleGNNTracksGeneric
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
