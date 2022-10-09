cmake_minimum_required(VERSION 3.8)

set(target_name "04_11_Qt_FFMPEG_VTK")

find_package(VTK REQUIRED)

find_package(Qt6 COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

#include(${VTK_USE_FILE})

add_executable(${target_name} "main.cpp" "widget.cpp" "widget.h")

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

target_link_libraries(${target_name} PRIVATE Qt6::Widgets)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)
