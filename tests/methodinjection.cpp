#include "tests.hpp"
extern "C" {
static int index_override(lua_State *L) {
    luaL_getmetafield(L, 1, "__class");
    if (lua_getfield(L, -1, "Properties") == LUA_TTABLE) {
        lua_pushvalue(L, 2);                      // push key
        if (lua_gettable(L, -2) == LUA_TTABLE &&  // check properties for key
            lua_getfield(L, -1, "get") == LUA_TFUNCTION) {
            lua_pushvalue(L, 1);  // push self
            lua_call(L, 1, 1);    // call getter
            return 1;
        }
    }
    luaC_deferindex(L);
    return 1;
}

static int newindex_override(lua_State *L) {
    luaL_getmetafield(L, 1, "__class");
    if (lua_getfield(L, -1, "Properties") == LUA_TTABLE) {
        lua_pushvalue(L, 2);                      // push key
        if (lua_gettable(L, -2) == LUA_TTABLE &&  // check properties for key
            lua_getfield(L, -1, "set") == LUA_TFUNCTION) {
            lua_pushvalue(L, 1);  // push self
            lua_pushvalue(L, 3);  // push value
            lua_call(L, 2, 0);    // call setter
            return 0;
        }
    }
    luaC_defernewindex(L);
    return 0;
}

static int get_n(lua_State *L) {
    lua_getfield(L, 1, "x");
    return 1;
}

static int set_n(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_pushnumber(L, n * 2);
    lua_setfield(L, 1, "x");
    return 0;
}
}

TEST_CASE("Method Injection") {
    LCL_TEST_BEGIN

    luaC_pushclass(L, "Base");
    LCL_CHECKSTACK(1);
    REQUIRE(lua_type(L, -1) == LUA_TTABLE);
    REQUIRE(luaC_isclass(L, -1));

    lua_newtable(L);              // properties table
    lua_newtable(L);              // property 'n'
    lua_pushcfunction(L, get_n);  // getter for 'n'
    lua_setfield(L, -2, "get");
    lua_pushcfunction(L, set_n);  // setter for 'n'
    lua_setfield(L, -2, "set");
    lua_setfield(L, -2, "n");
    lua_setfield(L, -2, "Properties");

    luaC_injectnewindex(L, -1, newindex_override);
    LCL_CHECKSTACK(1);
    luaC_injectindex(L, -1, index_override);
    LCL_CHECKSTACK(1);
    lua_pop(L, 1);

    lua_pushstring(L, "hello!");
    luaC_construct(L, 1, "Base");
    LCL_CHECKSTACK(1);
    REQUIRE(luaC_isobject(L, -1));
    REQUIRE(luaC_isinstance(L, -1, "Base"));

    lua_pushnumber(L, 23);
    luaC_pmcall(L, "squeak", 1, 1, 0);
    LCL_CHECKSTACK(2);
    REQUIRE(lua_type(L, -1) == LUA_TSTRING);
    REQUIRE(String(lua_tostring(L, -1)) == "n is now 46.0, squeak!");
    lua_pop(L, 2);

    LCL_TEST_END
}
