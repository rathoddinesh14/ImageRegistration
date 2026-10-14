# Examples for ImageRegistration

add_executable(ir_example_synthetic_translation
  synthetic_translation/main.cpp
)

target_link_libraries(ir_example_synthetic_translation
  PRIVATE
    ir::ImageRegistration
)

if(TARGET ir::io)
  target_link_libraries(ir_example_synthetic_translation PRIVATE ir::io)
  target_compile_definitions(ir_example_synthetic_translation PRIVATE IR_HAS_IO=1)
endif()

# Place binary under examples/synthetic_translation/ for a clear run path
set_target_properties(ir_example_synthetic_translation PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/synthetic_translation"
)
