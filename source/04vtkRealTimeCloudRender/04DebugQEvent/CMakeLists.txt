cmake_minimum_required(VERSION 3.8)

set(target_name "04_04_DebugQEvent")

find_package(VTK REQUIRED)

find_package(Qt6 COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

add_executable(${target_name} "main.cpp" "widget.cpp" "widget.h")

vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

target_link_libraries(${target_name} PRIVATE Qt6::Widgets)

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)
