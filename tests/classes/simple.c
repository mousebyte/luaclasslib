#include "simple.h"

static int simple_base_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static int simple_base_foo(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, 3);
    lua_pushnumber(L, n * x);
    return 1;
}

luaL_Reg simple_base_class_methods[] = {
    {"foo", simple_base_foo },
    {"new", simple_base_init},
    {NULL,  NULL            }
};

static int simple_derived_init(lua_State *L) {
    luaL_checknumber(L, 3);
    lua_setfield(L, 1, "v");
    luaC_superinit(L);
    return 0;
}

static int simple_derived_inc(lua_State *L) {
    luaL_checknumber(L, 2);
    lua_getfield(L, 1, "v");
    lua_arith(L, LUA_OPADD);
    lua_setfield(L, 1, "v");
    return 0;
}

luaL_Reg simple_derived_class_methods[] = {
    {"inc", simple_derived_inc },
    {"new", simple_derived_init},
    {NULL,  NULL               }
};
