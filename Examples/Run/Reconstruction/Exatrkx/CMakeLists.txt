add_executable(
  ActsExampleGNNTracksGeneric
  GenericRecGNNTracks.cpp)
target_link_libraries(
  ActsExampleGNNTracksGeneric
  PRIVATE
    ActsExamplesRecTracksCommon
    ActsExamplesTrackInferring
    ActsExamplesDetectorGeneric)

install(
  TARGETS
    ActsExampleGNNTracksGeneric
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
