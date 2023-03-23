#include "doctest.h"
extern "C" {
#include <lauxlib.h>
#include <lualib.h>
#include <moonaux.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

inline int foo(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, 3);
    lua_pushnumber(L, n * x);
    return 1;
}
}
