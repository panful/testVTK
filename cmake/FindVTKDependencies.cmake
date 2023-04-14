function(vtk_module_glob_dependencies)
    cmake_parse_arguments(PARSE_ARGV 0 _glob_dependencies
        ""
        "OUTPUT"
        "MODULES"
    )

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

# ARGS的前两个参数是输出参数，分别是dll的全名和名称
# 后面的参数是 ${VTK_LIBRARIES}
function(GetDependencies)
	cmake_parse_arguments(
		PARSE_ARGV 0 theArgs
		"" "" "ARGS"
	)

	if(the_arg_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "------------- wrong of function args -------------")
	endif()
	
	if(NOT theArgs_ARGS)
        message(FATAL_ERROR "------------- wrong of function args -------------")
	endif()

	list(LENGTH theArgs_ARGS the_length)
	if(NOT the_length GREATER 2)
        message(FATAL_ERROR "------------- wrong of function args -------------")
	endif()

    list(GET theArgs_ARGS 0 _arg0)
    list(GET theArgs_ARGS 1 _arg1)

    list(REMOVE_AT theArgs_ARGS 0)
    list(REMOVE_AT theArgs_ARGS 0)

    vtk_module_glob_dependencies(
        OUTPUT _MODULE_NAME_
        MODULES ${theArgs_ARGS}
    )

    list(REMOVE_DUPLICATES _MODULE_NAME_)
    set(_MODULE_FULL_NAME_)
    
    foreach(_imported_vtk_target IN LISTS _MODULE_NAME_)
        get_target_property(_imported_vtk_target_type "${_imported_vtk_target}" "TYPE")
    
        if(NOT ${_imported_vtk_target_type} STREQUAL "INTERFACE_LIBRARY")
            list(APPEND _temp__NAME_ "$<TARGET_FILE_NAME:${_imported_vtk_target}>")
            list(APPEND _MODULE_FULL_NAME_ "$<TARGET_FILE:${_imported_vtk_target}>")
        endif()
    
    endforeach()
    
    set(_MODULE_NAME_ ${_temp__NAME_})
    unset(_temp__NAME_)    

    set(${_arg0} ${_MODULE_FULL_NAME_} PARENT_SCOPE)
    set(${_arg1} ${_MODULE_NAME_} PARENT_SCOPE)

endfunction(GetDependencies)
