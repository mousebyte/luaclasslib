#include "tests.hpp"
extern "C" {
#include "classes/blocking_signal.h"
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

        luaC_registeruclass(L, &signal_class);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        luaC_registeruclass(L, &blocking_signal_class);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        luaC_construct(L, 0, "BlockingSignal");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "BlockingSignal"));

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

        moonL_dofile(L, "Base.moon");
        REQUIRE(luaC_registerclass(L, -1));
        LCL_CHECKSTACK(1);
        lua_pop(L, 1);

        luaC_registeruclass(L, &udata_derived_class);
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 8);
        luaC_construct(L, 1, "UdataDerived");
        LCL_CHECKSTACK(1);
        REQUIRE(luaC_isobject(L, -1));
        REQUIRE(luaC_isinstance(L, -1, "UdataDerived"));

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
}
