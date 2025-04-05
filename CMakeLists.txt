cmake_minimum_required(VERSION 3.16)
project(TinyNet)

set(CMAKE_CXX_STANDARD 17)

# 设置默认构建类型为 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    # 设置可能的构建类型选项
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

add_subdirectory(net)

