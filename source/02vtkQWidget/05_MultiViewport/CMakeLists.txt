cmake_minimum_required(VERSION 3.8)

set(target_name "02_05_MultiViewport")

#find_package(VTK COMPONENTS CommonCore REQUIRED)

find_package(VTK REQUIRED)

find_package(Qt6 COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

#include(${VTK_USE_FILE})

add_executable(${target_name} "main.cpp" "SideBySideRenderWindowsQt.cpp" "SideBySideRenderWindowsQt.h")

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

target_link_libraries(${target_name} PRIVATE Qt6::Widgets)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)
