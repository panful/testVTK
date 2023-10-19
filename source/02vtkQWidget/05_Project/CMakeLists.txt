set(target_name "02_05_Project")

set(CMAKE_AUTOMOC ON)  # Meta-Object Compiler
set(CMAKE_AUTORCC OFF) # Resource Compiler
set(CMAKE_AUTOUIC OFF) # User Interface Compiler

find_package(Qt6 COMPONENTS Widgets)

find_package(VTK
    COMPONENTS
        CommonCore
        FiltersSources
        InteractionStyle
        RenderingCore
        RenderingOpenGL2
        loguru
        ParallelMPI
        GUISupportQt
    QUIET
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "------------------ VTK not found ------------------")
endif()

set(SRC 
    actor.h
    controller.h
    infilter.h
    iviewer.h
    log.h
    main.cpp
    mainWindow.h
    rinterface.h
    scene.h
    vtkViewer.h
    widget.h
    wmanager.h
    outfilter.h
)

add_executable(${target_name} ${SRC})
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} Qt6::Widgets)

# 初始化vtk模块
vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# 安装EXE文件
install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
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
