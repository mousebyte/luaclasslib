#include "doctest/doctest.h"
extern "C" {
#include <luaclasslib.h>
#include <lualib.h>
#include <moonauxlib.h>
}

#define LCL_TEST_BEGIN              \
    using doctest::String;          \
    lua_State *L = luaL_newstate(); \
    luaL_openlibs(L);

#define LCL_TEST_END lua_close(L);

#define LCL_CHECKSTACK(n) CHECK(lua_gettop(L) == n)
