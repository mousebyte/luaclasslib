#include "tests.hpp"
extern "C" {
#include "classes/simple.h"
}

TEST_SUITE("Simple Classes") {
    TEST_CASE("Simple Base Class") {
        LCL_TEST_BEGIN

        luaC_newclass(L, "SimpleBase", NULL, simple_base_class_methods);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 7);
        luaC_construct(L, 1, "SimpleBase");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "SimpleBase"));

        lua_pushnumber(L, 3);
        luaC_mcall(L, "foo", 1, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 21);

        LCL_TEST_END
    }

    TEST_CASE(
        "Derived Simple Classes 1" *
        doctest::description("moonscript class extended by C class")) {
        LCL_TEST_BEGIN

        moonL_dofile(L, "Base.moon");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_registerclass(L, -1));
        LCL_CHECKSTACK(1);
        lua_pop(L, 1);

        luaC_newclass(L, "SimpleDerived", "Base", simple_derived_class_methods);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));

        REQUIRE(lua_getfield(L, -1, "var") == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Eek!");
        lua_pop(L, 2);

        lua_pushstring(L, "I have C functions!");
        lua_pushnumber(L, 12);
        luaC_construct(L, 2, "SimpleDerived");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "SimpleDerived"));

        lua_pushnumber(L, 3);
        luaC_mcall(L, "inc", 1, 0);
        LCL_CHECKSTACK(1);
        REQUIRE(lua_getfield(L, -1, "v") == LUA_TNUMBER);
        REQUIRE(lua_tonumber(L, -1) == 15);
        lua_pop(L, 1);

        lua_pushnumber(L, 11.2);
        luaC_mcall(L, "squeak", 1, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "n is now 11.2, squeak!");

        LCL_TEST_END
    }

    TEST_CASE(
        "Derived Simple Classes 2" *
        doctest::description("C class extended by C class")) {
        LCL_TEST_BEGIN

        luaC_newclass(L, "SimpleBase", NULL, simple_base_class_methods);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        luaC_newclass(
            L, "SimpleDerived", "SimpleBase", simple_derived_class_methods);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 3);
        lua_pushnumber(L, 7);
        luaC_construct(L, 2, "SimpleDerived");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "SimpleDerived"));

        lua_pushnumber(L, 10);
        luaC_mcall(L, "foo", 1, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 30);
        lua_pop(L, 1);

        lua_pushnumber(L, 3);
        luaC_mcall(L, "inc", 1, 0);
        LCL_CHECKSTACK(1);
        REQUIRE(lua_getfield(L, -1, "v") == LUA_TNUMBER);
        REQUIRE(lua_tonumber(L, -1) == 10);
        lua_pop(L, 1);

        LCL_TEST_END
    }
}
