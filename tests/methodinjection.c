#include "tests.h"

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
