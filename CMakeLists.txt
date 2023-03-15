cmake_minimum_required(VERSION 3.25)

project(moonlib C)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FindLua)

add_library(moonlib src/moonlib.c)
add_executable(moonlib-test src/test.c)
target_include_directories(moonlib PUBLIC src)
target_link_libraries(moonlib ${LUA_LIBRARIES})
target_include_directories(moonlib PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(moonlib-test moonlib)
configure_file(test2.moon test2.moon COPYONLY)
