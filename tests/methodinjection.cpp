#include "doctest.h"
extern "C" {
#include "methodinjection.c"
}

TEST_CASE("Method Injection") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    moonL_dofile(L, "Base.moon");
    REQUIRE(lua_type(L, -1) == LUA_TTABLE);
    REQUIRE(luaC_isclass(L, -1));
    lua_newtable(L);
    lua_newtable(L);
    lua_pushcfunction(L, get_n);
    lua_setfield(L, -2, "get");
    lua_pushcfunction(L, set_n);
    lua_setfield(L, -2, "set");
    lua_setfield(L, -2, "n");
    lua_setfield(L, -2, "Properties");
    luaC_injectnewindex(L, -1, newindex_override);
    luaC_injectindex(L, -1, index_override);
    REQUIRE(luaC_registerclass(L, -1));
    lua_pop(L, 1);

    lua_pushstring(L, "hello!");
    luaC_construct(L, 1, "Base");
    REQUIRE(luaC_isobject(L, -1));
    REQUIRE(luaC_isinstance(L, -1, "Base"));

    lua_pushnumber(L, 23);
    luaC_pmcall(L, "squeak", 1, 1, 0);
    REQUIRE(lua_type(L, -1) == LUA_TSTRING);
    REQUIRE(String(lua_tostring(L, -1)) == "n is now 46.0, squeak!");
    lua_pop(L, 2);

    lua_close(L);
}
