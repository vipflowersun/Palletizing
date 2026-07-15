set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(QT_VERSION_MAJOR 5)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Network)
if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
endif()

add_subdirectory(moduleAbstract)
add_subdirectory(moduleComponentWidget)