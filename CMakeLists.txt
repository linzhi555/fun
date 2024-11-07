cmake_minimum_required(VERSION 3.5)

project(fun-editor VERSION 0.1 LANGUAGES C CXX)

enable_testing()

set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(QT NAMES Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)


set( PROJECT_SOURCES
        ./src/version.cpp
        ./src/main.cpp
        ./src/mainwindow.cpp
        ./src/config.cpp
        ./src/editorwidget.cpp
        ./src/mainwindow.h
)
set(CMAKE_BUILD_TYPE Debug)
message(${PROJECT_SOURCES})
add_executable(fun ${PROJECT_SOURCES})
target_link_libraries(fun PRIVATE Qt5::Widgets)
