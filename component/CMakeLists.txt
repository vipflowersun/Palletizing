# Component - Header-Only INTERFACE 库
add_library(Component INTERFACE IMPORTED GLOBAL)

set_target_properties(Component PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
    INTERFACE_COMPILE_FEATURES    "cxx_std_17"
)
