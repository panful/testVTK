
cmake_minimum_required(VERSION 3.8)

set(target_name "04_09_DebugVTKEvent")

# 需要将vtk-config.cmake所在路径添加到环境变量 VTK_DIR
find_package(VTK REQUIRED)

# 设置默认的 vtk-config.cmake所在路径，就不需要在环境变量设置 VTK_DIR
#find_package(VTK QUIET
#    PATHS
#        D:/thePath
#    NO_DEFAULT_PATH
#)

add_executable(${target_name} "main.cpp")

target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

# 初始化vtk模块，不再需要在源代码中 VTK_MODULE_INIT(***)
vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# exe文件
install(TARGETS ${target_name}
        RUNTIME DESTINATION ./
)

# 安装本项目依赖的vtk_dll
function(vtk_module_glob_dependencies)
    cmake_parse_arguments(PARSE_ARGV 0 _glob_dependencies
        ""
        "OUTPUT"
        "MODULES"
    )

    if(_glob_dependencies_UNRECOGNIZED_ARGUMENTS)
        message(FATAL_ERROR
            "==============="
            "${_glob_dependencies_UNRECOGNIZED_ARGUMENTS}"
        )
    endif()

    if(NOT _glob_dependencies_OUTPUT)
        message(FATAL_ERROR
            "===========OUPUT"
        )
    endif()

    if(NOT _glob_dependencies_MODULES)
        message(FATAL_ERROR
            "===========MODULES"
        )
    endif()

    set(_glob_dependencies_module_stack ${_glob_dependencies_MODULES})
    set(_glob_dependencies_seen)
    
    while(_glob_dependencies_module_stack)
        list(GET _glob_dependencies_module_stack 0 _glob_dependencies_current_module)
        list(REMOVE_AT _glob_dependencies_module_stack 0)

        if(NOT TARGET "${_glob_dependencies_current_module}")
            continue()
        endif()

        if(_glob_dependencies_current_module IN_LIST _glob_dependencies_seen)
            continue()
        endif()

        list(APPEND _glob_dependencies_seen "${_glob_dependencies_current_module}")

        get_target_property(_glob_dependencies_public_dependencies "${_glob_dependencies_current_module}" "INTERFACE_vtk_module_depends")
        get_target_property(_glob_dependencies_private_dependencies "${_glob_dependencies_current_module}" "INTERFACE_vtk_module_private_depends")
        get_target_property(_glob_dependencies_optional_dependencies "${_glob_dependencies_current_module}" "INTERFACE_vtk_module_optional_depends")

        list(INSERT _glob_dependencies_module_stack 0 "${_glob_dependencies_public_dependencies}" "${_glob_dependencies_private_dependencies}")

        foreach(__optional_dependecy IN LISTS _glob_dependencies_optional_dependencies)
            if(TARGET "${__optional_dependency}")
                list(INSERT _glob_dependencies_module_stack 0 "${__optional_dependency}")
            endif()
        endforeach()

    endwhile()

    list(REMOVE_DUPLICATES _glob_dependencies_seen)
    set(${_glob_dependencies_OUTPUT} ${_glob_dependencies_seen} PARENT_SCOPE)
endfunction()

vtk_module_glob_dependencies(
    OUTPUT IMPORTED_VTK_MODULES
    MODULES ${VTK_LIBRARIES}
)

list(REMOVE_DUPLICATES IMPORTED_VTK_MODULES)
set(INSTALL_VTK_MODULES)

foreach(_imported_vtk_target IN LISTS IMPORTED_VTK_MODULES)
    get_target_property(_imported_vtk_target_type "${_imported_vtk_target}" "TYPE")

    if(NOT ${_imported_vtk_target_type} STREQUAL "INTERFACE_LIBRARY")
        list(APPEND _temp_imported_vtk_modules "$<TARGET_FILE_NAME:${_imported_vtk_target}>")
        list(APPEND INSTALL_VTK_MODULES "$<TARGET_FILE:${_imported_vtk_target}>")
    endif()
endforeach()

set(IMPORTED_VTK_MODULES ${_temp_imported_vtk_modules})
unset(_temp_imported_vtk_modules)

foreach(_vtk_module_file IN LISTS INSTALL_VTK_MODULES)
    install(FILES ${_vtk_module_file} DESTINATION .)
endforeach()