add_library(ModuleComponentWidget SHARED IMPORTED GLOBAL)

set_target_properties(ModuleComponentWidget PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES     "${CMAKE_CURRENT_SOURCE_DIR}/include"
    IMPORTED_IMPLIB                   "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/ModuleComponentWidget.lib"
    IMPORTED_IMPLIB_DEBUG             "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/ModuleComponentWidget.lib"
    IMPORTED_IMPLIB_RELEASE           "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/ModuleComponentWidget.lib"
    IMPORTED_IMPLIB_RELWITHDEBINFO    "${CMAKE_CURRENT_SOURCE_DIR}/lib/relwithdebinfo/ModuleComponentWidget.lib"
    IMPORTED_LOCATION                 "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/ModuleComponentWidget.dll"
    IMPORTED_LOCATION_DEBUG           "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/ModuleComponentWidget.dll"
    IMPORTED_LOCATION_RELEASE         "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/ModuleComponentWidget.dll"
    IMPORTED_LOCATION_RELWITHDEBINFO  "${CMAKE_CURRENT_SOURCE_DIR}/bin/relwithdebinfo/ModuleComponentWidget.dll"
)

set_target_properties(ModuleComponentWidget PROPERTIES
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_NOCONFIG   Release
)
