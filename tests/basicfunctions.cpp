#include "doctest.h"
extern "C" {
#include "tests.h"

static int func_for_derived(lua_State *L) {
    lua_pushstring(L, "Aha! ");
    lua_insert(L, 1);
    lua_concat(L, 2);
    return 1;
}
}

TEST_CASE("Basic Functionality") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    SUBCASE("Registration") {
        moonL_dofile(L, "Derived.moon");

        CHECK(lua_type(L, -1) == LUA_TTABLE);
        CHECK(luaC_isclass(L, -1));
        CHECK(luaC_registerclass(L, -1));

        lua_pop(L, 1);

        CHECK(luaC_getclass(L, "Base") == LUA_TTABLE);
        CHECK(luaC_isclass(L, -1));

        lua_pop(L, 1);

        CHECK(luaC_getclass(L, "Derived") == LUA_TTABLE);
        CHECK(luaC_isclass(L, -1));
        CHECK(lua_getfield(L, -1, "var") == LUA_TSTRING);
        CHECK(String(lua_tostring(L, -1)) == "Eek!");

        lua_pop(L, 2);
    }

    SUBCASE("Construction") {
        moonL_dofile(L, "Derived.moon");
        REQUIRE(luaC_registerclass(L, -1));
        lua_pop(L, 1);

        SUBCASE("Base") {
            lua_pushstring(L, "Hello, squeak!");
            luaC_construct(L, 1, "Base");
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "Base"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Hello, squeak!");
            lua_pop(L, 1);

            lua_pushnumber(L, 7);
            luaC_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "n is now 7.0, squeak!");
            lua_pop(L, 2);
        }

        SUBCASE("Derived") {
            lua_pushstring(L, "Whee!");
            lua_pushcfunction(L, func_for_derived);
            luaC_construct(L, 2, "Derived");
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "Derived"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Whee!");
            lua_pop(L, 1);

            lua_pushnumber(L, 37.1);
            luaC_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(
                String(lua_tostring(L, -1)) == "Aha! n is now 37.1, squeak!");
            lua_pop(L, 2);
        }
    }

    lua_close(L);
}
