cmake_minimum_required(VERSION 3.16)
project(ImageRegistration VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common warnings — treat as errors (Quality Phase 1)
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Eigen3 — prefer system package, fall back to FetchContent (useful for CI)
find_package(Eigen3 3.3 QUIET NO_MODULE)
if(NOT Eigen3_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG        3.4.0
    GIT_SHALLOW    TRUE
  )
  set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
  set(EIGEN_BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(Eigen)
  # Eigen 3.4 via FetchContent provides Eigen3::Eigen target
  if(NOT TARGET Eigen3::Eigen)
    add_library(Eigen3::Eigen ALIAS eigen)
  endif()
endif()

# Main library target
add_library(ImageRegistration INTERFACE)
add_library(ir::ImageRegistration ALIAS ImageRegistration)

target_include_directories(ImageRegistration
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(ImageRegistration
  INTERFACE
    Eigen3::Eigen
)

# Optional tests
option(IR_BUILD_TESTS "Build unit tests" OFF)
if(IR_BUILD_TESTS)
  enable_testing()

  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2
  )
  FetchContent_MakeAvailable(Catch2)

  # Make Catch2 module path available for catch_discover_tests
  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

  add_subdirectory(tests)
endif()

# Optional examples
option(IR_BUILD_EXAMPLES "Build examples" OFF)
if(IR_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Optional benchmarks
option(IR_BUILD_BENCHMARKS "Build benchmarks" OFF)
if(IR_BUILD_BENCHMARKS)
  add_subdirectory(benchmarks)
endif()
