cmake_minimum_required(VERSION 3.16)

project(qt_example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)
find_package(rxcpp CONFIG)
qt_standard_project_setup()

set(QT_EXAMPLE_SOURCES
        src/main.cpp
        src/MainWindow.cpp
        src/DoubleLineEditSubject.cpp
)

set(QT_EXAMPLE_HEADERS
        include/MainWindow.h
        include/DoubleLineEditSubject.h
)

qt_add_executable(qt_example ${QT_EXAMPLE_SOURCES} ${QT_EXAMPLE_HEADERS})

target_link_libraries(qt_example
        PRIVATE Qt6::Core
        PRIVATE Qt6::Widgets
        PRIVATE Qt6::Charts
        PRIVATE rxcpp
)

target_include_directories(qt_example
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)