add_executable(
  ActsExampleGNNTracksGeneric
  GenericRecGNNTracks.cpp)

add_executable(
  ActsExampleScriptedGNNTracksGeneric
  GenericRecScriptedGNNTracks.cpp)

target_include_directories(
    ActsExampleGNNTracksGeneric
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(
    ActsExampleScriptedGNNTracksGeneric
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ActsExampleGNNTracksGeneric
  PRIVATE
    ActsExamplesRecTracksCommon
    ActsExamplesTrackInferring
    ActsExamplesDetectorGeneric)

target_link_libraries(
  ActsExampleScriptedGNNTracksGeneric
  PRIVATE
    ActsExamplesRecTracksCommon
    ActsExamplesTrackInferring
    ActsExamplesDetectorGeneric)

install(
  TARGETS
    ActsExampleGNNTracksGeneric
    ActsExampleScriptedGNNTracksGeneric
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
