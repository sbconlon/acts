add_executable(
  ActsExampleGNNTracksGeneric
  GenericRecGNNTracks.cpp)
target_link_libraries(
  ActsExampleGNNTracksGeneric
  PRIVATE
    ActsExamplesTrackInferring
    ActsExamplesRecTracksCommon
    ActsExamplesRecDetectorGeneric)

install(
  TARGETS
    ActsExampleGNNTracksGeneric
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
