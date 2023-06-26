cmake_minimum_required(VERSION 3.25)

project(luaclasslib
    VERSION 2.1.0
    LANGUAGES C CXX)

set(LUACLASS_ENABLE_ASAN false CACHE BOOL "Enable address sanitizer for tests target.")
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

### main project
set(LUACLASS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(LUACLASS_MAIN_PROJECT ON)
endif()

include(FindLua)

add_library(luaclass src/luaclasslib.c)
add_library(LuaClass::LuaClass ALIAS luaclass)
target_include_directories(luaclass PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
    ${LUA_INCLUDE_DIR})
target_link_libraries(luaclass ${LUA_LIBRARIES})
set_target_properties(luaclass PROPERTIES EXPORT_NAME LuaClass)

target_compile_options(luaclass PUBLIC 
    -fno-strict-aliasing -Wall -Wextra -Wunused -Wno-unused-function
    $<$<CONFIG:Debug>:-g3 -ggdb3 -pedantic>
    $<$<CONFIG:Release>:-O2 -Wfatal-errors>)

if(LUACLASS_MAIN_PROJECT)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/LuaClassConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LuaClass")
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/LuaClassConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
    install(TARGETS luaclass
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            EXPORT LuaClassTargets)
    install(FILES src/luaclasslib.h src/moonauxlib.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT LuaClassTargets
            FILE LuaClassTargets.cmake
            NAMESPACE LuaClass::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LuaClass)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LuaClassConfig.cmake
                  ${CMAKE_CURRENT_BINARY_DIR}/LuaClassConfigVersion.cmake
            DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/LuaClass")
    export(TARGETS luaclass
           NAMESPACE LuaClass::
           FILE ${CMAKE_CURRENT_BINARY_DIR}/LuaClassTargets.cmake)
    install(FILES ${version_file} ${config_file}
            DESTINATION ${cmake_dir}
            COMPONENT ${projectname_target}Export)
endif()

enable_testing()
add_subdirectory(doctest)
include(doctest/scripts/cmake/doctest.cmake)
add_executable(tests
    tests/classes/file.c
    tests/classes/signal.c
    tests/classes/blocking_signal.c
    tests/classes/udata_derived.c
    tests/classes/simple.c
    tests/main.cpp
    tests/basicfunctions.cpp
    tests/cclass.cpp
    tests/udataclass.cpp
    tests/udataclass_inheritance.cpp
    tests/methodinjection.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests luaclass doctest)
doctest_discover_tests(tests)

file(GLOB asset_files ${CMAKE_SOURCE_DIR}/tests/assets/*)
add_custom_target(assets COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${asset_files} $<TARGET_FILE_DIR:tests>)
add_dependencies(tests assets)

if(LUACLASS_ENABLE_ASAN)
    target_compile_options(tests PUBLIC -fsanitize=address)
    target_link_options(tests PUBLIC -fsanitize=address)
endif()
