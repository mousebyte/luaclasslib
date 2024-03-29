#include "tests.hpp"
extern "C" {
#include "classes/blocking_signal.h"
#include "classes/file.h"
#include "classes/signal.h"
#include "classes/udata_derived.h"

static int slot1_var, slot2_var;

static int slot1(lua_State *L) {
    slot1_var = luaL_checknumber(L, 1);
    return 0;
}

static int slot2(lua_State *L) {
    slot2_var = luaL_checknumber(L, 1);
    return 0;
}
}

TEST_SUITE("User Data Classes") {
    TEST_CASE(
        "Derived User Data Classes 1" *
        doctest::description("userdata class extended by userdata class")) {
        LCL_TEST_BEGIN

        lua_pushlightuserdata(L, &signal_class);
        luaC_classfromptr(L);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        register_lcl_class(L);

        lua_pushlightuserdata(L, &blocking_signal_class);
        luaC_classfromptr(L);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        register_lcl_class(L);

        luaC_construct(L, 0, "lcltests.BlockingSignal");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "lcltests.BlockingSignal"));

        lua_pushcfunction(L, slot1);
        luaC_mcall(L, "connect", 1, 0);
        LCL_CHECKSTACK(1);
        lua_pushcfunction(L, slot2);
        luaC_mcall(L, "connect", 1, 0);
        LCL_CHECKSTACK(1);
        lua_pushvalue(L, -1);
        lua_pushnumber(L, 33);
        lua_call(L, 1, 0);
        LCL_CHECKSTACK(1);
        REQUIRE(slot1_var == 33);
        REQUIRE(slot2_var == 33);

        luaC_mcall(L, "block", 0, 0);
        LCL_CHECKSTACK(1);
        lua_pushvalue(L, -1);
        lua_pushnumber(L, 185);
        lua_call(L, 1, 0);
        LCL_CHECKSTACK(1);
        REQUIRE(slot1_var == 33);
        REQUIRE(slot2_var == 33);

        luaC_mcall(L, "unblock", 0, 0);
        LCL_CHECKSTACK(1);
        lua_pushvalue(L, -1);
        lua_pushnumber(L, 56235);
        lua_call(L, 1, 0);
        LCL_CHECKSTACK(1);
        REQUIRE(slot1_var == 56235);
        REQUIRE(slot2_var == 56235);

        LCL_TEST_END
    }

    TEST_CASE(
        "Derived User Data Classes 2" *
        doctest::description("standard class extended by userdata class")) {
        LCL_TEST_BEGIN

        lua_pushlightuserdata(L, &udata_derived_class);
        luaC_classfromptr(L);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        register_lcl_class(L);

        lua_pushnumber(L, 8);
        luaC_construct(L, 1, "lcltests.UdataDerived");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "lcltests.UdataDerived"));

        luaC_mcall(L, "get", 0, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 8);
        lua_pop(L, 1);

        lua_pushnumber(L, 12);
        luaC_mcall(L, "squeak", 1, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "n is now 20.0, squeak!");

        LCL_TEST_END
    }

    TEST_CASE(
        "Derived User Data Classes 3" *
        doctest::description("userdata class extended by moonscript class")) {
        LCL_TEST_BEGIN

        lua_pushlightuserdata(L, &file_class);
        luaC_classfromptr(L);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        register_lcl_class(L);

        lua_pushnumber(L, 3);
        lua_pushstring(L, "Derived.moon");
        luaC_construct(L, 2, "DerivedFromUdata");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "DerivedFromUdata"));

        lua_getfield(L, -1, "number");
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 3);
        lua_pop(L, 1);

        luaC_mcall(L, "filename", 0, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
        lua_pop(L, 2);

        lua_pushnumber(L, 2);
        lua_pushstring(L, "Derived.moon");
        luaC_construct(L, 2, "DerivedFromUdata2");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "DerivedFromUdata2"));

        lua_getfield(L, -1, "number");
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 2);
        lua_pop(L, 1);

        luaC_mcall(L, "filename", 0, 1);
        LCL_CHECKSTACK(2);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
        lua_pop(L, 1);

        lua_pushnumber(L, 4);
        luaC_mcall(L, "foo", 1, 0);
        LCL_CHECKSTACK(1);

        lua_getfield(L, -1, "number");
        LCL_CHECKSTACK(2);
        REQUIRE(lua_tonumber(L, -1) == 8);
        lua_pop(L, 1);

        LCL_TEST_END
    }
}
