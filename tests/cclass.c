#include "tests.h"

static int CClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    return 0;
}

static luaC_Class CClass = {
    .name      = "CClass",
    .parent    = NULL,
    .user_ctor = 1,
    .alloc     = NULL,
    .gc        = NULL,
    .methods   = {{"foo", foo}, {"new", CClass_init}, {NULL, NULL}}
};

static int DCClass_init(lua_State *L) {
    lua_setfield(L, 1, "x");
    lua_setfield(L, 1, "str");
    return 0;
}

static luaC_Class DCClass = {
    .name      = "DCClass",
    .parent    = "Base",
    .user_ctor = 1,
    .alloc     = NULL,
    .gc        = NULL,
    .methods   = {{"foo", foo}, {"new", DCClass_init}, {NULL, NULL}}
};
