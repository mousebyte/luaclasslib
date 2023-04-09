#include "tests.h"

static int CClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static const luaL_Reg CClass_funcs[] = {
    {"foo", foo        },
    {"new", CClass_init},
    {NULL,  NULL       }
};

static int DCClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    lua_setfield(L, 1, "str");
    return 0;
}

static const luaL_Reg DCClass_funcs[] = {
    {"foo", foo         },
    {"new", DCClass_init},
    {NULL,  NULL        }
};
