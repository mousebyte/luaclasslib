#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
extern "C" {
#include <lauxlib.h>
#include <lualib.h>
#include <moonaux.h>
}
#include <filesystem>
#include <iostream>
#include "doctest.h"

extern "C" {
int foo(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, 3);
    lua_pushnumber(L, n * x);
    return 1;
}

int newmyclass(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static const luaL_Reg funcs[] = {
    {"foo", foo       },
    {"new", newmyclass},
    {NULL,  NULL      }
};
}

TEST_CASE("Basic functions") {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    moonL_dofile(L, "Derived.moon");

    CHECK(lua_type(L, -1) == LUA_TTABLE);
    CHECK(moonL_isclass(L, -1));
    CHECK(moonL_registerclass(L, -1));

    lua_pop(L, 1);

    CHECK(moonL_getclass(L, "Base") == LUA_TTABLE);
    CHECK(moonL_isclass(L, -1));

    lua_pop(L, 1);

    CHECK(moonL_getclass(L, "Derived") == LUA_TTABLE);
    CHECK(moonL_isclass(L, -1));

    lua_pop(L, 1);

    SUBCASE("Construction") {
        lua_pushstring(L, "Hello, squeak!");
        moonL_construct(L, 1, "Base");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "Base"));
        REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
        REQUIRE(doctest::String(lua_tostring(L, -1)) == "Hello, squeak!");
        lua_pop(L, 1);
        lua_pushnumber(L, 7);
        moonL_mcall(L, "squeak", 1, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(
            doctest::String(lua_tostring(L, -1)) == "n is now 7.0, squeak!");
        lua_pop(L, 3);
        moonL_newclass(L, "myclass", NULL, funcs, NULL);
        REQUIRE(moonL_isclass(L, -1));
        lua_pushnumber(L, 7);
        moonL_construct(L, 1, "myclass");
        REQUIRE(moonL_isinstance(L, -1, "myclass"));
        lua_pushnumber(L, 3);
        moonL_mcall(L, "foo", 1, 1);
    }

    lua_close(L);
}
