#include "doctest.h"
extern "C" {
#include "userdataclass.c"
}

TEST_CASE("User Data Classes") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    SUBCASE("Basic User Data Class") {
        luaC_newuclass(L, &FileClass);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 16);
        lua_pushstring(L, "Derived.moon");
        luaC_construct(L, 2, "FileClass");
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "FileClass"));

        lua_pushnumber(L, 4);
        luaC_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 64);
        lua_pop(L, 1);

        luaC_mcall(L, "filename", 0, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
        lua_pop(L, 1);

        luaC_mcall(L, "readline", 0, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Base = require \"Base\"");
        lua_pop(L, 2);
    }

    SUBCASE("Derived User Data Class") {
        SUBCASE("From Basic Class") {
            moonL_dofile(L, "Base.moon");
            REQUIRE(lua_type(L, -1) == LUA_TTABLE);
            REQUIRE(luaC_isclass(L, -1));
            REQUIRE(luaC_registerclass(L, -1));
            lua_pop(L, 1);

            luaC_newuclass(L, &BasicClass);
            REQUIRE(luaC_isclass(L, -1));

            REQUIRE(lua_getfield(L, -1, "var") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Eek!");
            lua_pop(L, 2);

            lua_pushstring(L, "got any cheese?");
            lua_pushnumber(L, 666);
            luaC_construct(L, 2, "BasicClass");
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "BasicClass"));

            luaC_mcall(L, "sum", 0, 1);
            REQUIRE(lua_tonumber(L, -1) == 666);
            lua_pop(L, 1);

            lua_pushnumber(L, -444);
            luaC_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "n is now -444.0, squeak!");
            lua_pop(L, 1);

            luaC_mcall(L, "sum", 0, 1);
            REQUIRE(lua_tonumber(L, -1) == 222);
            lua_pop(L, 2);
        }

        SUBCASE("From User Data Class") {
            luaC_newuclass(L, &FileClass);
            REQUIRE(luaC_isclass(L, -1));
            lua_pop(L, 1);

            luaC_newuclass(L, &DFileClass);
            REQUIRE(luaC_isclass(L, -1));
            lua_pop(L, 1);

            lua_pushnumber(L, 77);
            lua_pushstring(L, "Derived.moon");
            lua_pushnumber(L, 2);
            luaC_construct(L, 3, "DFileClass");
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "DFileClass"));

            lua_pushnumber(L, 2);
            luaC_mcall(L, "foo", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 154);
            lua_pop(L, 1);

            luaC_mcall(L, "filename", 0, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
            lua_pop(L, 1);

            luaC_mcall(L, "readline", 0, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Base = require \"Base\"");
            lua_pop(L, 1);

            lua_pushnumber(L, 0);
            lua_pushnumber(L, 37);
            luaC_mcall(L, "setint", 2, 0);

            lua_pushnumber(L, 1);
            lua_pushnumber(L, 99);
            luaC_mcall(L, "setint", 2, 0);

            lua_pushnumber(L, 0);
            luaC_mcall(L, "getint", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 37);
            lua_pop(L, 1);

            lua_pushnumber(L, 1);
            luaC_mcall(L, "getint", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 99);
            lua_pop(L, 2);
        }
    }

    lua_close(L);
}
