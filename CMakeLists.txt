set (CMAKE_CXX_COMPILER /usr/local/bin/g++)
set (CMAKE_C_COMPILER /usr/bin/gcc)

cmake_minimum_required(VERSION 2.8)
project(luadb)

set(CMAKE_CXX_FLAGS "-std=c++14 -ldl -lpthread")
set(CMAKE_C_FLAGS "-fPIC")
set(LUA_LIB liblua.a)    

add_definitions(-DASIO_STANDALONE)

include_directories(
    ${CMAKE_SOURCE_DIR}/opt/lua
    ${CMAKE_SOURCE_DIR}/opt/sol2
    ${CMAKE_SOURCE_DIR}/opt/lua-protobuf
    ${CMAKE_SOURCE_DIR}/opt/asio/asio/include
    ${CMAKE_SOURCE_DIR}/opt/gdb/include
    /usr/include
)


link_directories(
    ${CMAKE_SOURCE_DIR}/lib
)

add_subdirectory(${CMAKE_SOURCE_DIR}/opt)
add_subdirectory(${CMAKE_SOURCE_DIR}/src)
