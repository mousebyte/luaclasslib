cmake_minimum_required(VERSION 3.25)

project(lclexamples LANGUAGES C)

include(FindLua)
find_package(LuaClass)
link_libraries(LuaClass::LuaClass)

add_executable(basic basic/main.c)
add_executable(cclass cclass/main.c)
