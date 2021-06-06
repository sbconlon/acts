find_package (Python3 COMPONENTS Interpreter Development)
message( "Python3_FOUND = " ${Python3_FOUND})
message( "Python3_INCLUDE_DIRS = ${Python3_INCLUDE_DIRS}")
message( "Python3_LIBRARIES = ${Python3_LIBRARIES}")

add_library(
  ActsExatrkxPlugin SHARED
  include/Acts/Plugins/Exatrkx/gnn.hpp
  src/gnn.cpp)

target_include_directories(
  ActsExatrkxPlugin
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> ${Python3_INCLUDE_DIRS})

target_link_libraries(
  ActsExatrkxPlugin 
  PUBLIC 
    ActsCore
    ${Python3_LIBRARIES}
    "${TORCH_LIBRARIES}")

install(
  TARGETS ActsExatrkxPlugin
  EXPORT ActsExatrkxPluginTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
  
install(
  DIRECTORY include/Acts
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
