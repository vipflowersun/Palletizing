add_library(ModuleAbstract SHARED IMPORTED GLOBAL)

set_target_properties(ModuleAbstract PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES     "${CMAKE_CURRENT_SOURCE_DIR}/include"
    IMPORTED_IMPLIB                   "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/ModuleAbstract.lib"
    IMPORTED_IMPLIB_DEBUG             "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/ModuleAbstract.lib"
    IMPORTED_IMPLIB_RELEASE           "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/ModuleAbstract.lib"
    IMPORTED_IMPLIB_RELWITHDEBINFO    "${CMAKE_CURRENT_SOURCE_DIR}/lib/relwithdebinfo/ModuleAbstract.lib"
    IMPORTED_LOCATION                 "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/ModuleAbstract.dll"
    IMPORTED_LOCATION_DEBUG           "${CMAKE_CURRENT_SOURCE_DIR}/bin/debug/ModuleAbstract.dll"
    IMPORTED_LOCATION_RELEASE         "${CMAKE_CURRENT_SOURCE_DIR}/bin/release/ModuleAbstract.dll"
    IMPORTED_LOCATION_RELWITHDEBINFO  "${CMAKE_CURRENT_SOURCE_DIR}/bin/relwithdebinfo/ModuleAbstract.dll"
)

set_target_properties(ModuleAbstract PROPERTIES
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
    MAP_IMPORTED_CONFIG_NOCONFIG   Release
)
