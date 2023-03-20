#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
extern "C" {
#include <lauxlib.h>
#include <lualib.h>
#include <moonaux.h>
}
#include "doctest.h"

TEST_CASE("Basic functions") {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    moonL_dofile(L, "Base.moon");

    CHECK(lua_type(L, -1) == LUA_TTABLE);
    CHECK(moonL_isclass(L, -1));
    CHECK(moonL_registerclass(L, -1));

    lua_pop(L, 1);

    CHECK(moonL_getclass(L, "Base") == LUA_TTABLE);
    CHECK(moonL_isclass(L, -1));

    lua_pop(L, 1);

    SUBCASE("Construction") {
        lua_pushstring(L, "Hello, squeak!");
        moonL_construct(L, 1, "Base");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "Base"));
    }

    lua_close(L);
}
