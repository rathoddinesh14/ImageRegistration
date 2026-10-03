# Unit tests for ImageRegistration

add_executable(ir_tests
  test_smoke.cpp
  core/test_point2d.cpp
  core/test_image_geometry2d.cpp
  core/test_image2d.cpp
  io/test_png_writer.cpp
  transform/test_transform2d.cpp
)

target_link_libraries(ir_tests
  PRIVATE
    ir::ImageRegistration
    ir::io
    Catch2::Catch2WithMain
)

include(CTest)
include(Catch)
catch_discover_tests(ir_tests)
