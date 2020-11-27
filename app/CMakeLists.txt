cmake_minimum_required(VERSION 3.16)

project(app LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "include/*.hpp")
file(GLOB SOURCES_LIST CONFIGURE_DEPENDS "src/*.cpp")

message(${HEADER_LIST})

add_executable(${PROJECT_NAME}
  ${HEADER_LIST}
  ${SOURCES_LIST}
  qml/qml.qrc
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME}
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick fwlib)
target_include_directories(${PROJECT_NAME} PRIVATE include)
