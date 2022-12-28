cmake_minimum_required(VERSION 3.0.0)
project(pj2 VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
link_directories(" ${ROOT_SOURCE_DIR}/bin")
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/spdlog/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/compat/msvc
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/eigen
  ${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty
  ${CMAKE_CURRENT_SOURCE_DIR}/include/3rdparty/libcds
  ${CMAKE_CURRENT_SOURCE_DIR}/common
  G:/CPPLib/boost_1_73_0
  )
include(CTest)

enable_testing()
add_executable(pj2 Consol.cpp)
target_link_libraries(pj2 bxRelease)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
