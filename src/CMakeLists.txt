project(main)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

file(GLOB CLIENT_SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)	
file(GLOB CLIENT_HDRS ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.hpp)


add_executable(main main.cpp ${CLIENT_HDRS} ${CLIENT_SRCS})
target_link_libraries(main ${LUA_LIB} mysqlclient fmt)
add_dependencies(main lua)
