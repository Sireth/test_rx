cmake_minimum_required(VERSION 3.16)

project(heat_distribution VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Charts)
find_package(rxcpp CONFIG)
find_package(OpenCL REQUIRED)
qt_standard_project_setup()

qt_add_executable(heat_distribution main.cpp)

target_link_libraries(heat_distribution
        PRIVATE Qt6::Core
        PRIVATE Qt6::Widgets
        PRIVATE Qt6::Charts
        PRIVATE rxcpp
        PRIVATE OpenCL::OpenCL
)