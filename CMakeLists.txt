cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #导出编译命令
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) #强制使用c++标准

project(CppServer)

# People keep running CMake in the wrong folder, completely nuking their project or creating weird bugs.
# This checks if you're running CMake from a folder that already has CMakeLists.txt.
# Importantly, this catches the common case of running it from the root directory.
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O2 -std=c++17 -Wall -Wextra")


# 设置构建输出路径为 build 文件夹
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 包含头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 查找所有源文件
file(GLOB LIB_SOURCES "src/*.cpp")

# 生成动态链接库
add_library(_server SHARED ${LIB_SOURCES})

# 生成可执行文件
add_executable(Server test/server.cpp)
target_link_libraries(Server _server)

add_executable(Single_Client test/single_client.cpp)
target_link_libraries(Single_Client _server)

add_executable(Mult_Client test/mult_client.cpp)
target_link_libraries(Mult_Client _server)

# 安装动态链接库到指定目录
# install(TARGETS Server DESTINATION ${CMAKE_BINARY_DIR}/bin)
# install(TARGETS Client DESTINATION ${CMAKE_BINARY_DIR}/bin)
# install(TARGETS _server DESTINATION ${CMAKE_BINARY_DIR}/bin)