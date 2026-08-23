cmake_minimum_required(VERSION 3.16)
project(ImageRegistration VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Common warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find Eigen3
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

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
