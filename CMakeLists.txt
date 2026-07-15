cmake_minimum_required(VERSION 3.16)
#cmake_policy(SET CMP0144 NEW)
project(Palletizing)

# ============================================
# 基础信息打印
# ============================================
message(STATUS "Target System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT CMAKE_BUILD_TYPE STREQUAL "Release"
    AND NOT CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# ============================================
# option选项
# ============================================
option(SUPPORT_VZNL "Support VZNL SDK" ON)
# ============================================
# 编译选项
# ============================================
if(MSVC)
    # 移除默认的运行时检查选项
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCsu" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCs" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/RTCu" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    # 对 C 标志也做同样处理
    string(REPLACE "/RTC1" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/RTCsu" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/RTCs" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    string(REPLACE "/RTCu" "" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
    add_compile_options(/utf-8)
    add_compile_options(/bigobj)
    add_compile_options(/wd4819)
    add_compile_definitions(_SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING)
    add_compile_definitions(_HAS_DEPRECATED_STDEXT_ARR_ITERS=1)
    string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_C "${CMAKE_DEPFILE_FLAGS_C}")
    string(REPLACE "/showIncludes" "" CMAKE_DEPFILE_FLAGS_CXX "${CMAKE_DEPFILE_FLAGS_CXX}")
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CoWeldApp)

# ============================================
# 核心宏定义
# ============================================
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_OUTPUT_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_OUTPUT_DIR}/release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_OUTPUT_DIR}/relwithdebinfo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_OUTPUT_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_OUTPUT_DIR}/release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_OUTPUT_DIR}/relwithdebinfo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_OUTPUT_DIR}/debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_OUTPUT_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_OUTPUT_DIR}/relwithdebinfo)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/relwithdebinfo)
else()
    set(CMAKE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIR}/release)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_INSTALL_PREFIX "${PROJECT_ROOT_DIR}/install" CACHE PATH "Installation Directory" FORCE)
set(RESOURCE_DIR ${PROJECT_ROOT_DIR}/res/${PROJECT_NAME}.qrc)

# ============================================
# 三方库路径
# ============================================
set(QT_VERSION "Qt5.15.2")
set(CMAKE_PREFIX_PATH ${PROJECT_ROOT_DIR}/thirdParty/${QT_VERSION})
# ============================================
# 公共函数
# ============================================

# ============================================
# 全局宏
# ============================================

# ============================================
# 子项目
# ============================================
add_subdirectory(${PROJECT_ROOT_DIR}/component)
add_subdirectory(${PROJECT_ROOT_DIR}/module)
add_subdirectory(${PROJECT_ROOT_DIR}/app)

# ============================================
# Install 配置
# ============================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Install.cmake)