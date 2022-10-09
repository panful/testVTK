cmake_minimum_required(VERSION 3.8)

set(target_name "4.12RenderServer")

find_package(VTK REQUIRED)

find_package(Qt6 COMPONENTS Core Widgets Network)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

#include(${VTK_USE_FILE})

add_executable(${target_name} "main.cpp" "serverWidget.cpp" "serverWidget.h")

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} Qt6::Widgets Qt6::Network)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)
