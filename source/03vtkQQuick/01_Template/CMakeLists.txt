cmake_minimum_required (VERSION 3.8)

set(target_name "03_01_Template")

add_executable (${target_name} "main.cpp")

find_package(VTK
    COMPONENTS
        CommonCore
        GUISupportQtQuick
        InfovisCore
    REQUIRED)

find_package(Qt6 COMPONENTS Core Gui Quick Qml  REQUIRED)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC ON) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

target_link_libraries(${target_name} PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick )

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/QQuickVTK.qml DESTINATION ./)

target_sources(${target_name} PRIVATE QQuickVTK.qml)