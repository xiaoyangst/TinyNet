cmake_minimum_required(VERSION 3.16)
project(Net)

set(CMAKE_CXX_STANDARD 17)
aux_source_directory(${CMAKE_SOURCE_DIR}/base BASE_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/net NET_BASE)
add_library(Net SHARED ${BASE_SRC} ${NET_BASE})
#add_library(Net STATIC ${BASE_SRC} ${NET_BASE})
target_include_directories(Net PRIVATE
        ${CMAKE_SOURCE_DIR}
)