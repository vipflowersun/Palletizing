project(PalletizingApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION_MAJOR 5)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

file(GLOB_RECURSE PROJECT_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

if(QT_VERSION_MAJOR EQUAL 5)
    qt5_add_resources(PROJECT_SOURCES ${PROJECT_ROOT_DIR}/res/Palletizing.qrc)
else()
    qt_add_resources(PROJECT_SOURCES ${PROJECT_ROOT_DIR}/res/Palletizing.qrc)
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Qt
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::PrintSupport)

# spdlog
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/spdlog/include)

# nlohmann
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/nlohmann)

# eigen
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/eigen-3.4.0)

#ElaWidgetTools
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
	target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools_Qt6.8.3/include)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools_Qt6.8.3/lib)
		target_link_libraries(${PROJECT_NAME} PUBLIC ElaWidgetToolsd)
	else()
		target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools_Qt6.8.3/lib)
		target_link_libraries(${PROJECT_NAME} PUBLIC ElaWidgetTools)
	endif()
else()
	target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools/include)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools/lib)
		target_link_libraries(${PROJECT_NAME} PUBLIC ElaWidgetToolsd)
	else()
		target_link_directories(${PROJECT_NAME} PUBLIC ${PROJECT_ROOT_DIR}/thirdParty/ElaWidgetTools/lib)
		target_link_libraries(${PROJECT_NAME} PUBLIC ElaWidgetTools)
	endif()
endif()

# luaIDE
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/luaIDE/include)
if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/luaIDE/lib/windows/debug)
        target_link_libraries(${PROJECT_NAME} PRIVATE LuaIDEd)
    else()
        target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/luaIDE/lib/windows/release)
        target_link_libraries(${PROJECT_NAME} PRIVATE LuaIDE)
    endif()
else()
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/luaIDE/lib/linux/debug)
        target_link_libraries(${PROJECT_NAME} PRIVATE libLuaIDEd.so)
    else()
        target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/luaIDE/lib/linux/release)
        target_link_libraries(${PROJECT_NAME} PRIVATE libLuaIDE.so)
    endif()
endif()

#zmc
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/zmc/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${PROJECT_ROOT_DIR}/thirdParty/zmc/lib/x64)
target_link_libraries(${PROJECT_NAME} PRIVATE zmotion zauxdll)

#Component
target_link_libraries(${PROJECT_NAME} PUBLIC Component)

# ModuleComponentWidget
target_link_libraries(${PROJECT_NAME} PUBLIC ModuleComponentWidget)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DWIN32_LEAN_AND_MEAN
    -D_USE_MATH_DEFINES
    -DNOMINMAX
    -DSPDLOG_ACTIVE_LEVEL=0)