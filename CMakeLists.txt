cmake_minimum_required(VERSION 3.16)

project(text_rx VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(rxcpp CONFIG)
qt_standard_project_setup()

qt_add_executable(text_rx
        main.cpp
)

target_link_libraries(text_rx PRIVATE Qt6::Core PRIVATE Qt6::Widgets PRIVATE rxcpp)