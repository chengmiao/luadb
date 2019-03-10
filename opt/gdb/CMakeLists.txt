project (gdb)
cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/mysql-connector-cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/fmt)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/googletest/googletest/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/googletest/googletest/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/opt/mysql-connector-cpp/driver)
add_definitions(-DSPDLOG_FMT_EXTERNAL)
add_definitions("-Wall -g")

add_executable(test samples/test.cpp ${FMT_SRC})
add_executable(ggtest samples/ggtest.cpp ${FMT_SRC})

target_link_libraries(test mysqlcppconn)
target_link_libraries(test fmt)

target_link_libraries(ggtest mysqlcppconn)
target_link_libraries(ggtest fmt)
target_link_libraries(ggtest libgtest.a -lpthread -lm)
