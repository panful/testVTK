set(target_name "01_10_grid")

find_package(VTK)

if (NOT VTK_FOUND)
  message(FATAL_ERROR "------------------ VTK not found ------------------")
endif()

add_executable(${target_name} "main.cpp")
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES})

# 初始化vtk模块
vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# 安装EXE文件
install(TARGETS ${target_name} RUNTIME DESTINATION .)

# 拷贝DLL文件
FILE(GLOB _VTK_LIBRARIES_ ${VTK_DIR}/../../../bin/*.dll)
list(APPEND LIB_LIST ${_VTK_LIBRARIES_})
foreach(_file_ IN LISTS LIB_LIST)
    # 拷贝DLL到安装目录
    install(FILES ${_file_} DESTINATION .)
    
    # 拷贝DLL到生成目录
    add_custom_command(TARGET ${target_name}
    PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E
            copy ${_file_} $<TARGET_FILE_DIR:${target_name}>
    )
endforeach()
