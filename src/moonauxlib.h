#ifndef MOONAUXLIB_H
#define MOONAUXLIB_H

#include <lauxlib.h>

/**
 * @brief Loads a Moonscript string as a Lua chunk.
 *
 * @param L The Lua state.
 * @param str The Moonscript string to load.
 *
 * @return The Lua pcall status code.
 */
static inline int moonL_loadstring(lua_State *L, const char *str) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "loadstring");
    lua_remove(L, -2);
    lua_pushstring(L, str);
    return lua_pcall(L, 1, 1, 0);
}

/**
 * @brief Loads a Moonscript file as a Lua chunk.
 *
 * @param L The Lua state.
 * @param str The name of the file to load.
 *
 * @return The Lua pcall status code.
 */
static inline int moonL_loadfile(lua_State *L, const char *name) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "loadfile");
    lua_remove(L, -2);
    lua_pushstring(L, name);
    return lua_pcall(L, 1, 1, 0);
}

/**
 * @brief Loads a Moonscript file as a Lua chunk, then runs the returned chunk.
 *
 * @param L The Lua state.
 * @param str The name of the file to load.
 *
 * @return The Lua pcall status code.
 */
static inline int moonL_dofile(lua_State *L, const char *name) {
    luaL_loadstring(L, "return require('moonscript')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "dofile");
    lua_remove(L, -2);
    lua_pushstring(L, name);
    return lua_pcall(L, 1, 1, 0);
}

/**
 * @brief Prints a formatted version of an object.
 *
 * @param L The Lua state.
 * @param index The index of the object to print.
 *
 * @return The Lua pcall status code.
 */
static inline int moonL_print(lua_State *L, int index) {
    lua_pushvalue(L, index);
    luaL_loadstring(L, "return require('moon')");
    lua_pcall(L, 0, 1, 0);
    lua_getfield(L, -1, "p");
    lua_insert(L, -3);
    lua_pop(L, 1);
    return lua_pcall(L, 1, 0, 0);
}

#endif
