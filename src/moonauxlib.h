#include <lauxlib.h>

static inline int moonL_loadstring(lua_State *L, const char *str) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "loadstring");
    lua_remove(L, -2);
    lua_pushstring(L, str);
    return lua_pcall(L, 1, LUA_MULTRET, 0);
}

static inline int moonL_loadfile(lua_State *L, const char *name) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "loadfile");
    lua_remove(L, -2);
    lua_pushstring(L, name);
    return lua_pcall(L, 1, LUA_MULTRET, 0);
}

static inline int moonL_dofile(lua_State *L, const char *name) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "dofile");
    lua_remove(L, -2);
    lua_pushstring(L, name);
    return lua_pcall(L, 1, LUA_MULTRET, 0);
}

static inline int moonL_print(lua_State *L, int index) {
    lua_pushvalue(L, index);
    luaL_loadstring(L, "return require('moon')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -2, "p");
    lua_insert(L, -4);
    lua_pop(L, 2);
    return lua_pcall(L, 1, LUA_MULTRET, 0);
}
