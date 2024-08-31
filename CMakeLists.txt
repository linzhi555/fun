cmake_minimum_required(VERSION 3.5)

project(fun-editor VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_subdirectory(blah)

set( PROJECT_SOURCES
        ./src/main.cpp
        ./src/mainwindow.cpp
        ./src/mainwindow.ui
        ./src/mainwindow.h
)

message(${PROJECT_SOURCES})
add_executable(fun ${PROJECT_SOURCES})

target_link_libraries(fun PRIVATE Qt5::Widgets blah)
target_include_directories(fun PRIVATE blah)
