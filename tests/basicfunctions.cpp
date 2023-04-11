#include "tests.hpp"
extern "C" {

#include "classes/simple.h"

static int func_for_derived(lua_State *L) {
    lua_pushstring(L, "Aha! ");
    lua_insert(L, 1);
    lua_concat(L, 2);
    return 1;
}

static int uservalue_index(lua_State *L) {
    lua_pushstring(L, "nothing here!");
    return 1;
}

static int uservalue_newindex(lua_State *L) {
    switch (lua_type(L, 3)) {
        case LUA_TNUMBER:
            lua_pushvalue(L, 3);
            lua_arith(L, LUA_OPADD);
            break;
        case LUA_TSTRING:
            lua_pushstring(L, ", squeak!");
            lua_concat(L, 2);
            break;
    }
    lua_rawset(L, 1);
    return 0;
}
}

TEST_SUITE("Basic Functionality") {
    TEST_CASE("Class Registry") {
        LCL_TEST_BEGIN;

        SUBCASE("Registration") {
            moonL_dofile(L, "Derived.moon");
            LCL_CHECKSTACK(1);
            CHECK(lua_type(L, -1) == LUA_TTABLE);
            CHECK(luaC_isclass(L, -1));
            CHECK(luaC_register(L, -1));
            lua_pop(L, 1);

            CHECK(luaC_getclass(L, "Base") == LUA_TTABLE);
            LCL_CHECKSTACK(1);
            CHECK(luaC_isclass(L, -1));
            lua_pop(L, 1);

            CHECK(luaC_getclass(L, "Derived") == LUA_TTABLE);
            LCL_CHECKSTACK(1);
            CHECK(luaC_isclass(L, -1));
            CHECK(lua_getfield(L, -1, "var") == LUA_TSTRING);
            CHECK(String(lua_tostring(L, -1)) == "Eek!");
        }

        SUBCASE("Unregistration") {
            REQUIRE(luaC_newclass(
                L, "SimpleBase", NULL, simple_base_class_methods));
            LCL_CHECKSTACK(1);
            lua_pop(L, 1);

            REQUIRE(luaC_getclass(L, "SimpleBase") == LUA_TTABLE);
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_getuclass(L, -1) != NULL);

            luaC_unregister(L, "SimpleBase");

            REQUIRE(luaC_getclass(L, "Base") == LUA_TNIL);
            LCL_CHECKSTACK(2);
            lua_pop(L, 1);

            REQUIRE(luaC_getuclass(L, -1) == NULL);
        }

        LCL_TEST_END
    }

    TEST_CASE("Class Construction") {
        LCL_TEST_BEGIN

        moonL_dofile(L, "Derived.moon");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_register(L, -1));
        lua_pop(L, 1);

        SUBCASE("Base") {
            lua_pushstring(L, "Hello, squeak!");
            luaC_construct(L, 1, "Base");
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "Base"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Hello, squeak!");
            lua_pop(L, 1);

            lua_pushnumber(L, 7);
            luaC_mcall(L, "squeak", 1, 1);
            LCL_CHECKSTACK(2);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "n is now 7.0, squeak!");
        }

        SUBCASE("Derived") {
            lua_pushstring(L, "Whee!");
            lua_pushcfunction(L, func_for_derived);
            luaC_construct(L, 2, "Derived");
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "Derived"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Whee!");
            lua_pop(L, 1);

            lua_pushnumber(L, 37.1);
            luaC_mcall(L, "squeak", 1, 1);
            LCL_CHECKSTACK(2);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(
                String(lua_tostring(L, -1)) == "Aha! n is now 37.1, squeak!");
        }
        LCL_TEST_END
    }

    TEST_CASE("User Value Accessors") {
        LCL_TEST_BEGIN

        int  i = 3;
        int *p = (int *)lua_newuserdatauv(L, sizeof(int), 1);
        lua_newtable(L);  // user value
        lua_newtable(L);  // metatable for user value
        lua_pushcfunction(L, uservalue_index);
        lua_setfield(L, -2, "__index");
        lua_pushcfunction(L, uservalue_newindex);
        lua_setfield(L, -2, "__newindex");
        lua_setmetatable(L, -2);
        lua_setiuservalue(L, -2, 1);

        lua_pushnumber(L, 50);
        REQUIRE(luaC_setuvfield(L, -2, 1, "x"));
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_getuvfield(L, -1, 1, "x") == LUA_TNUMBER);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 100);
        lua_pop(L, 1);

        REQUIRE(luaC_getuvfield(L, -1, 1, "n") == LUA_TSTRING);
        REQUIRE(luaC_uvget(L, -2, 1) == LUA_TSTRING);
        LCL_CHECKSTACK(2);
        REQUIRE(String(lua_tostring(L, -1)) == "nothing here!");
        lua_pop(L, 1);

        lua_pushstring(L, "str");
        lua_pushstring(L, "hello");
        REQUIRE(luaC_uvset(L, -3, 1));
        LCL_CHECKSTACK(1);
        lua_pushstring(L, "str");
        REQUIRE(luaC_uvget(L, -2, 1) == LUA_TSTRING);
        LCL_CHECKSTACK(2);
        REQUIRE(String(lua_tostring(L, -1)) == "hello, squeak!");
        lua_pop(L, 1);

        lua_pushstring(L, "s");
        REQUIRE(luaC_uvget(L, -2, 1) == LUA_TSTRING);
        LCL_CHECKSTACK(2);
        REQUIRE(String(lua_tostring(L, -1)) == "nothing here!");
        lua_pop(L, 1);

        lua_pushstring(L, "str");
        lua_pushstring(L, "hello");
        REQUIRE(luaC_uvrawset(L, -3, 1));
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_getuvfield(L, -1, 1, "str") == LUA_TSTRING);
        LCL_CHECKSTACK(2);
        REQUIRE(String(lua_tostring(L, -1)) == "hello");
        lua_pop(L, 1);

        lua_pushstring(L, "g");
        REQUIRE(luaC_uvrawget(L, -2, 1) == LUA_TNIL);
        LCL_CHECKSTACK(2);
        lua_pop(L, 1);

        REQUIRE(luaC_uvrawgetp(L, -1, 1, &i) == LUA_TNIL);
        LCL_CHECKSTACK(2);
        lua_pop(L, 1);

        lua_pushstring(L, "eek!");
        REQUIRE(luaC_uvrawsetp(L, -2, 1, &i));
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_uvrawgetp(L, -1, 1, &i) == LUA_TSTRING);
        LCL_CHECKSTACK(2);
        REQUIRE(String(lua_tostring(L, -1)) == "eek!");

        LCL_TEST_END
    }
}
