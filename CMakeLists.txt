cmake_minimum_required(VERSION 3.25)

project(moonlib C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FindLua)

add_library(moonaux src/moonaux.c)
add_executable(tests tests/main.cpp)
target_include_directories(moonaux PUBLIC src)
target_link_libraries(moonaux ${LUA_LIBRARIES})
target_include_directories(moonaux PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(tests moonaux)
add_custom_command(TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/tests/assets $<TARGET_FILE_DIR:tests>)
