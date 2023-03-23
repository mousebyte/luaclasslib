cmake_minimum_required(VERSION 3.25)

project(moonlib C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(FindLua)
add_library(moonaux src/moonaux.c)
add_executable(tests tests/main.cpp tests/cclass.cpp tests/userdataclass.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_include_directories(moonaux PUBLIC src)
target_link_libraries(moonaux ${LUA_LIBRARIES})
target_include_directories(moonaux PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(tests moonaux)
file(GLOB asset_files ${CMAKE_SOURCE_DIR}/tests/assets/*)
add_custom_target(assets COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${asset_files} $<TARGET_FILE_DIR:tests>)
add_dependencies(tests assets)
