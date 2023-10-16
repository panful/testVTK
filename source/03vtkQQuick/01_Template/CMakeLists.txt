
set(target_name "03_01_Template")

set(CMAKE_AUTOMOC ON)  # Meta-Object Compiler
set(CMAKE_AUTORCC OFF) # Resource Compiler
set(CMAKE_AUTOUIC OFF) # User Interface Compiler

find_package(Qt6 COMPONENTS Core Gui Quick Qml)

find_package(VTK
    COMPONENTS
        CommonCore
        GUISupportQtQuick
        InfovisCore
    QUIET
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "------------------ VTK not found ------------------")
endif()

add_executable(${target_name} "main.cpp")
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick)

# 初始化vtk模块
vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# 安装EXE和资源文件*.qml
set_target_properties(${target_name} PROPERTIES RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/QQuickVTK.qml")
install(TARGETS ${target_name}
        RUNTIME DESTINATION .
        RESOURCE DESTINATION .
)

# 拷贝*.qml到生成目录
add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/QQuickVTK.qml" $<TARGET_FILE_DIR:${target_name}>/QQuickVTK.qml
)

# 调用函数获取依赖的DLL文件
set(ARG _FULL_NAME_ _NAME_ ${VTK_LIBRARIES})
include(${PROJECT_SOURCE_DIR}/cmake/FindVTKDependencies.cmake)
GetDependencies(ARGS ${ARG})

# 拷贝DLL文件
foreach(_full_name _name IN ZIP_LISTS _FULL_NAME_ _NAME_)
    # 拷贝DLL到安装目录
    install(FILES ${_full_name} DESTINATION .)
    
    # 拷贝DLL到生成目录
    add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_if_different ${_full_name} $<TARGET_FILE_DIR:${target_name}>/${_name}
    )
endforeach()
