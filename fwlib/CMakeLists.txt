cmake_minimum_required(VERSION 3.16)

project(fwlib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(HEADERS include/fwlib/idirectorywatcher.hpp include/fwlib/fileinfo.hpp include/fwlib/isubscriber.hpp include/fwlib/subscriber.hpp include/fwlib/directorywatcher.hpp include/fwlib/detail/directorywatcherimpl.hpp)

set(SOURCES src/example.cpp src/directorywatcher.cpp src/detail/directorywatcherimpl.cpp)

if (WIN32)
    list(APPEND SOURCES_OS src/detail/directorywatcherimpl_windows.cpp)
endif(WIN32)
if (UNIX)
    list(APPEND SOURCES_OS src/detail/directorywatcherimpl_linux.cpp)
endif(UNIX)
if (APPLE)
    list(APPEND SOURCES_OS src/detail/directorywatcherimpl_macos.cpp)
endif(APPLE)

#Для простоты подключения в Qt проект, сделаем инклуды публичными.
add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SOURCES_OS})
target_include_directories(${PROJECT_NAME} PUBLIC include/fwlib)

add_executable(example ${HEADERS} ${SOURCES} ${SOURCES_OS})
target_include_directories(example PUBLIC include/fwlib)
