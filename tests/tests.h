#include <lauxlib.h>
#include <luaclasslib.h>
#include <lualib.h>
#include <moonauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int foo(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, 3);
    lua_pushnumber(L, n * x);
    return 1;
}
