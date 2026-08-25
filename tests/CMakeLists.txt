# Unit tests for ImageRegistration

add_executable(ir_tests
  test_smoke.cpp
)

target_link_libraries(ir_tests
  PRIVATE
    ir::ImageRegistration
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(ir_tests)
