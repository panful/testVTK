
set(target_name "01_10_grid")

find_package(VTK
    COMPONENTS
        CommonCore
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
        FiltersExtraction
        #IOCGNS             # CGNS需要在生成VTK时选择
        IOGeometry          # vtkFLUENTReader
        RenderingAnnotation # vtkScalarBarActor
    QUIET
)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "------------------ VTK not found ------------------")
endif()

add_executable(${target_name} "main.cpp")
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES})
target_link_directories(${target_name} PRIVATE ${VTK_PREFIX_PATH}/lib) # 找不到IOCGNS，手动链接
target_link_libraries(${target_name} PRIVATE vtkIOCGNSReader-$<IF:$<CONFIG:Debug>,9.1d,9.1>.lib)

# 初始化vtk模块，不再需要在源代码中使用 VTK_MODULE_INIT(***)
vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# 安装EXE文件
install(TARGETS ${target_name} RUNTIME DESTINATION .)

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

# cgns依赖的动态库
install(FILES ${VTK_PREFIX_PATH}/bin/vtkIOCGNSReader-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll DESTINATION .)
install(FILES ${VTK_PREFIX_PATH}/bin/vtkcgns-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll DESTINATION .)
install(FILES ${VTK_PREFIX_PATH}/bin/vtkhdf5-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll DESTINATION .)
add_custom_command(TARGET ${target_name}
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_if_different ${VTK_PREFIX_PATH}/bin/vtkIOCGNSReader-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll $<TARGET_FILE_DIR:${target_name}>/vtkIOCGNSReader-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_if_different ${VTK_PREFIX_PATH}/bin/vtkcgns-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll $<TARGET_FILE_DIR:${target_name}>/vtkcgns-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E
    copy_if_different ${VTK_PREFIX_PATH}/bin/vtkhdf5-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll $<TARGET_FILE_DIR:${target_name}>/vtkhdf5-$<IF:$<CONFIG:Debug>,9.1d,9.1>.dll
)
