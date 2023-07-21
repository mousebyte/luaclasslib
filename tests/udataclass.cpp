#include "tests.hpp"
extern "C" {
#include "classes/file.h"
#include "classes/signal.h"

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
    TEST_CASE("Basic User Data Classes") {
        LCL_TEST_BEGIN

        SUBCASE("File Class") {
            lua_pushlightuserdata(L, &file_class);
            luaC_classfromptr(L);
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isclass(L, -1));
            register_lcl_class(L);

            lua_pushstring(L, "Derived.moon");
            luaC_construct(L, 1, "lcltests.File");
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "File"));

            luaC_mcall(L, "filename", 0, 1);
            LCL_CHECKSTACK(2);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
            lua_pop(L, 1);

            luaC_mcall(L, "readline", 0, 1);
            LCL_CHECKSTACK(2);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Base = require \"Base\"");
        }

        SUBCASE("Signal Class") {
            lua_pushlightuserdata(L, &signal_class);
            luaC_classfromptr(L);
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isclass(L, -1));
            register_lcl_class(L);

            luaC_construct(L, 0, "lcltests.Signal");
            LCL_CHECKSTACK(1);
            REQUIRE(luaC_isobject(L, -1));
            REQUIRE(luaC_isinstance(L, -1, "Signal"));

            lua_pushcfunction(L, slot1);
            luaC_mcall(L, "connect", 1, 0);
            LCL_CHECKSTACK(1);
            lua_pushcfunction(L, slot2);
            luaC_mcall(L, "connect", 1, 0);
            LCL_CHECKSTACK(1);
            lua_pushvalue(L, -1);
            lua_pushnumber(L, 12);
            lua_call(L, 1, 0);
            LCL_CHECKSTACK(1);
            REQUIRE(slot1_var == 12);
            REQUIRE(slot2_var == 12);

            lua_pushcfunction(L, slot1);
            luaC_mcall(L, "disconnect", 1, 0);
            LCL_CHECKSTACK(1);
            lua_pushvalue(L, -1);
            lua_pushnumber(L, 17);
            lua_call(L, 1, 0);

            REQUIRE(slot1_var == 12);
            REQUIRE(slot2_var == 17);
        }

        LCL_TEST_END
    }
}
