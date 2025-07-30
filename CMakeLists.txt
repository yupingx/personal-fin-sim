# =============================================================================
#  CMakeLists.txt
#
#    This CMake configuration builds the main executable for the personal
#    finance simulation tool, along with an optional unit test suite.
#
#  Key Tasks:
#    - Builds the core application executable (pfsim)
#    - Defines a reusable static library (pfsimlib)
#    - Optional GoogleTest integration (enable via -DBUILD_TESTING=ON)
#
#  Usage:
#    cmake -S . -B build [-DBUILD_TESTS=ON]
#    cmake --build build
#
#  Maintainer:  Yuping X.
#  Created:     June 2025
# =============================================================================
cmake_minimum_required(VERSION 3.14)
project(personalFinSim)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include/ directory for headers
include_directories(include)

add_library(pfsimlib
    src/asset.cpp
    src/modelRecession.cpp
    src/userDataLoading.cpp
)

add_executable(pfsim
    src/main.cpp
    src/clparser.cpp
    src/personalFinSim.cpp
)

target_link_libraries(pfsim pfsimlib)

target_include_directories(pfsim PRIVATE include)

# Optional test build
option(BUILD_TESTS "Enable building of unit tests" OFF)

if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  )

  FetchContent_MakeAvailable(googletest)

  add_subdirectory(tests/)
  
  # Link test binary against GTest and core library to be tested
  target_link_libraries(tests
    pfsimlib
  )
  enable_testing()

  include(GoogleTest)
endif()
