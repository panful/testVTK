cmake_minimum_required (VERSION 3.12)

project ("VTK_PROJECT")

# 指定CMake安装目录前缀
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
# 开启必须设置C++标准库版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置C++标准库版本为C++ 17
set(CMAKE_CXX_STANDARD 17)
# 关闭编译器特定扩展
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置生成目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/target_debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/target_release)

# 包含子目录
add_subdirectory(source)
