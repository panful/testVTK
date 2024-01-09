set(target_name "01_04_CameraTransform")

find_package(VTK
    COMPONENTS
        CommonCore
        CommonColor
        CommonDataModel
        FiltersSources
        InteractionStyle
        RenderingContextOpenGL2
        RenderingCore
        RenderingFreeType
        RenderingGL2PSOpenGL2
        RenderingOpenGL2
        loguru
        ParallelMPI
        RenderingAnnotation # vtkLegendScaleActor
        FiltersHybrid       # vtkImplicitModeller
        RenderingLOD        # vtkLODActor
        FiltersModeling     # vtkRotationalExtrusionFilter
    QUIET
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "------------------ VTK not found ------------------")
endif()

add_executable(${target_name} "main.cpp")
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

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
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
        copy_if_different ${_full_name} $<TARGET_FILE_DIR:${target_name}>/${_name}
    )
endforeach()
