cmake_minimum_required(VERSION 3.16)
project(RideShareGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    rideshare.h
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(RideShareGUI MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(RideShareGUI ${PROJECT_SOURCES})
endif()

target_link_libraries(RideShareGUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RideShareGUI)
endif()
