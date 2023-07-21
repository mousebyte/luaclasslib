#include "doctest/doctest.h"
extern "C" {
#include <luaclasslib.h>
#include <lualib.h>
#include <moonauxlib.h>
}

#define LCL_TEST_BEGIN                                  \
    using doctest::String;                              \
    lua_State *L = luaL_newstate();                     \
    luaL_openlibs(L);                                   \
    luaL_loadstring(L, "return require('moonscript')"); \
    lua_pcall(L, 0, 0, 0);                              \
    luaL_requiref(L, "lcl", luaopen_lcl, 0);            \
    lua_pop(L, 1);

#define LCL_TEST_END lua_close(L);

#define LCL_CHECKSTACK(n) CHECK(lua_gettop(L) == n)

static inline void register_lcl_class(lua_State *L) {
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    luaL_getsubtable(L, -1, "lcltests");
    lua_remove(L, -2);
    luaC_getname(L, -2);
    lua_rotate(L, -3, -1);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}
