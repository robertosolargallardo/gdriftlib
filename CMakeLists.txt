cmake_minimum_required(VERSION 2.8.9)
project(libgdrift)
set(CMAKE_BUILD_TYPE Release)
find_package(Boost 1.55.0)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -O3")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cc")
add_library(gdrift++ SHARED ${SOURCES})

add_executable(test_sim "${CMAKE_SOURCE_DIR}/test/sim/test.cc")
add_executable(test_load "${CMAKE_SOURCE_DIR}/test/load/test.cc")
add_executable(test_parallel "${CMAKE_SOURCE_DIR}/test/parallel/test.cc")
add_executable(test_statistics "${CMAKE_SOURCE_DIR}/test/statistics/test.cc")
add_executable(experiment_threads "${CMAKE_SOURCE_DIR}/test/experiment_threads/test.cc")

link_directories(${CMAKE_BINARY_DIR})

target_link_libraries(test_sim gdrift++)
target_link_libraries(test_load gdrift++)
target_link_libraries(test_parallel gdrift++ pthread)
target_link_libraries(test_statistics gdrift++)
target_link_libraries(experiment_threads gdrift++ pthread)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/usr/" CACHE PATH "default install path" FORCE )
endif()

install(TARGETS gdrift++ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS test_sim test_load test_parallel DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/gdrift++ FILES_MATCHING PATTERN "*.h")

