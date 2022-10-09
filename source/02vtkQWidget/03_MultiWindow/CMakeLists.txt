cmake_minimum_required(VERSION 3.8)

set(target_name "02_03_MultiWindow")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

find_package(VTK 
    COMPONENTS
        vtkCommonColor
        vtkCommonCore
        vtkFiltersSources
        vtkInteractionStyle
        vtkInteractionWidgets
        vtkRenderingContextOpenGL2
        vtkRenderingCore
        vtkRenderingFreeType
        vtkRenderingGL2PSOpenGL2
        vtkRenderingOpenGL2
        vtkGUISupportQt
        vtkRenderingQt
    REQUIRED
)

function(prepend_for_each_item src_list prepend_content)
    foreach(_item IN LISTS ${src_list})
        string(PREPEND _item ${prepend_content})
        list(APPEND _dest_list ${_item})
    endforeach()
    set(${src_list} ${_dest_list} PARENT_SCOPE)
endfunction()

set(HEADERS
    mainWindow.h
    service.h
    viewer.h
    window.h
    scene.h
    screenCapture.h
    
    command.h
    setColorCommand.h
)

set(SRCS
    mainWindow.cpp
    service.cpp
    viewer.cpp
    window.cpp
    scene.cpp
    screenCapture.cpp
    
)

set(RSCS
    mainWindow.ui
)

prepend_for_each_item(HEADERS "include/")
prepend_for_each_item(SRCS "source/")
prepend_for_each_item(RSCS "source/")

add_executable(${target_name} "main.cpp" ${HEADERS} ${SRCS} ${RSCS})

target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${target_name} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES} )

vtk_module_autoinit(
    TARGETS ${target_name}
    MODULES ${VTK_LIBRARIES}
)

# exe
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