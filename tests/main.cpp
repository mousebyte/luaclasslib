#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
extern "C" {
#include <lauxlib.h>
#include <lualib.h>
#include <moonaux.h>
#include <stdio.h>
}
#include "doctest.h"

extern "C" {
int foo(lua_State *L) {
    int n = luaL_checknumber(L, 2);
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, 3);
    lua_pushnumber(L, n * x);
    return 1;
}

int CClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static const luaL_Reg CClass_funcs[] = {
    {"foo", foo        },
    {"new", CClass_init},
    {NULL,  NULL       }
};

int DCClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    lua_setfield(L, 1, "str");
    return 0;
}

static const luaL_Reg DCClass_funcs[] = {
    {"foo", foo         },
    {"new", DCClass_init},
    {NULL,  NULL        }
};

int func_for_derived(lua_State *L) {
    lua_pushstring(L, "Aha! ");
    lua_insert(L, 1);
    lua_concat(L, 2);
    return 1;
}

typedef struct UClass_Data {
    FILE *file;
    char  name[];
} UClass_Data;

void UClass_alloc(lua_State *L) {
    size_t       len;
    const char  *str = lua_tolstring(L, 3, &len);
    UClass_Data *o   = (UClass_Data *)lua_newuserdatauv(
        L, sizeof(UClass_Data) + (sizeof(char) * len), 1);
    o->file = fopen(str, "r");
    strcpy(o->name, str);
}

void UClass_gc(void *p) {
    UClass_Data *o = (UClass_Data *)p;
    fclose(o->file);
    o->file = NULL;
}

static moonL_UClass UClass {"UClass", UClass_alloc, UClass_gc};

int UClass_init(lua_State *L) {
    lua_pushvalue(L, 2);
    lua_setfield(L, 1, "x");
    return 0;
}

int UClass_filename(lua_State *L) {
    UClass_Data *o = (UClass_Data *)moonL_checkuclass(L, 1, "UClass");
    lua_pushstring(L, o->name);
    return 1;
}

static luaL_Reg UClass_funcs[] = {
    {"foo",      foo            },
    {"new",      UClass_init    },
    {"filename", UClass_filename},
    {NULL,       NULL           }
};
}

TEST_CASE("Basic Functionality") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    SUBCASE("Registration") {
        moonL_dofile(L, "Derived.moon");

        CHECK(lua_type(L, -1) == LUA_TTABLE);
        CHECK(moonL_isclass(L, -1));
        CHECK(moonL_registerclass(L, -1));

        lua_pop(L, 1);

        CHECK(moonL_getclass(L, "Base") == LUA_TTABLE);
        CHECK(moonL_isclass(L, -1));

        lua_pop(L, 1);

        CHECK(moonL_getclass(L, "Derived") == LUA_TTABLE);
        CHECK(moonL_isclass(L, -1));
        CHECK(lua_getfield(L, -1, "var") == LUA_TSTRING);
        CHECK(String(lua_tostring(L, -1)) == "Eek!");

        lua_pop(L, 2);
    }

    SUBCASE("Construction") {
        moonL_dofile(L, "Derived.moon");
        REQUIRE(moonL_registerclass(L, -1));
        lua_pop(L, 1);

        SUBCASE("Base") {
            lua_pushstring(L, "Hello, squeak!");
            moonL_construct(L, 1, "Base");
            REQUIRE(moonL_isobject(L, -1));
            REQUIRE(moonL_isinstance(L, -1, "Base"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Hello, squeak!");
            lua_pop(L, 1);

            lua_pushnumber(L, 7);
            moonL_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "n is now 7.0, squeak!");
            lua_pop(L, 2);
        }

        SUBCASE("Derived") {
            lua_pushstring(L, "Whee!");
            lua_pushcfunction(L, func_for_derived);
            moonL_construct(L, 2, "Derived");
            REQUIRE(moonL_isobject(L, -1));
            REQUIRE(moonL_isinstance(L, -1, "Derived"));

            REQUIRE(lua_getfield(L, -1, "str") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Whee!");
            lua_pop(L, 1);

            lua_pushnumber(L, 37.1);
            moonL_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(
                String(lua_tostring(L, -1)) == "Aha! n is now 37.1, squeak!");
            lua_pop(L, 2);
        }
    }

    lua_close(L);
}

TEST_CASE("C Classes") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    SUBCASE("Basic C Class") {
        moonL_newclass(L, "CClass", NULL, CClass_funcs, NULL);
        REQUIRE(moonL_isclass(L, -1));

        lua_pushnumber(L, 7);
        moonL_construct(L, 1, "CClass");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "CClass"));

        lua_pushnumber(L, 3);
        moonL_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 21);
        lua_pop(L, 2);
    }

    SUBCASE("Derived C Class") {
        moonL_dofile(L, "Base.moon");
        REQUIRE(moonL_registerclass(L, -1));
        lua_pop(L, -1);

        moonL_newclass(L, "DCClass", "Base", DCClass_funcs, NULL);
        REQUIRE(moonL_isclass(L, -1));

        REQUIRE(lua_getfield(L, -1, "var") == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Eek!");
        lua_pop(L, 1);

        lua_pushstring(L, "I have C functions!");
        lua_pushnumber(L, 12);
        moonL_construct(L, 2, "DCClass");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "DCClass"));

        lua_pushnumber(L, 2);
        moonL_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 24);
        lua_pop(L, 1);

        lua_pushnumber(L, 11.2);
        moonL_mcall(L, "squeak", 1, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "n is now 11.2, squeak!");
        lua_pop(L, 2);
    }

    SUBCASE("User Data Class") {
        moonL_newclass(L, "UClass", NULL, UClass_funcs, &UClass);
        REQUIRE(moonL_isclass(L, -1));

        lua_pushnumber(L, 16);
        lua_pushstring(L, "Derived.moon");
        moonL_construct(L, 2, "UClass");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "UClass"));

        lua_pushnumber(L, 4);
        moonL_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 64);
        lua_pop(L, 1);

        moonL_mcall(L, "filename", 0, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
        lua_pop(L, 2);
    }

    lua_close(L);
}
