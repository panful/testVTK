cmake_minimum_required (VERSION 3.8)

project ("VTK_PROJECT")

# 指定CMake安装目录前缀
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/target)
# 开启必须设置C++标准库版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置C++标准库版本为C++ 17
set(CMAKE_CXX_STANDARD 17)
# 关闭编译器特定扩展
set(CMAKE_CXX_EXTENSIONS OFF)
# 添加__cplusplus宏定义，编码格式为utf-8
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /utf-8")

# 添加对VS工程的支持
if(MSVC AND MSVC_VERSION GREATER_EQUAL 1910)
    add_compile_options(/permissive-)
endif()

# 包含子目录
add_subdirectory(source)
