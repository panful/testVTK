cmake_minimum_required (VERSION 3.8)

set(target_name "04_02_WinSockClient")

# 将源代码添加到此项目的可执行文件。
add_executable (${target_name} "main.cpp" "socket.h" "socket.cpp" "window.h" "window.cpp")

find_package(Qt6 COMPONENTS REQUIRED Core Widgets)

set(CMAKE_AUTOMOC ON) # Meta-Object Compiler
set(CMAKE_AUTORCC OFF) # Resource Compiler
set(CMAKE_AUTOUIC ON) # User Interface Compiler

target_link_libraries(${target_name} PRIVATE wsock32 ws2_32 Qt6::Core Qt6::Widgets)

# 将库文件，可执行文件，头文件安装到指定目录
install(TARGETS ${target_name} RUNTIME DESTINATION ./)
