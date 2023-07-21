#include "udata_derived.h"
#include <stdlib.h>

typedef struct {
    void *handle;
} udata_derived;

static void udata_derived_alloc(lua_State *L) {
    lua_newuserdatauv(L, sizeof(udata_derived), 1);
}

static void udata_derived_gc(lua_State *L, void *p) {
    udata_derived *o = (udata_derived *)p;
    if (o->handle) {
        free(o->handle);
        o->handle = NULL;
    }
}

static int udata_derived_init(lua_State *L) {
    udata_derived *o = (udata_derived *)luaC_checkuclass(L, 1, "UdataDerived");
    int           *i = (int *)malloc(sizeof(int));
    *i               = luaL_checknumber(L, 2);
    o->handle        = i;
    return 0;
}

static int udata_derived_get(lua_State *L) {
    udata_derived *o = (udata_derived *)luaC_checkuclass(L, 1, "UdataDerived");
    lua_pushnumber(L, *((int *)o->handle));
    return 1;
}

static int udata_derived_squeak(lua_State *L) {
    udata_derived *o = (udata_derived *)luaC_checkuclass(L, 1, "UdataDerived");
    lua_pushnumber(L, *((int *)o->handle));
    lua_arith(L, LUA_OPADD);
    luaC_super(L, "squeak", 1, 1);
    return 1;
}

static luaL_Reg udata_derived_methods[] = {
    {"new",    udata_derived_init  },
    {"get",    udata_derived_get   },
    {"squeak", udata_derived_squeak},
    {NULL,     NULL                }
};

luaC_Class udata_derived_class = {
    .name      = "UdataDerived",
    .module    = "lcltests",
    .parent    = "Base",
    .user_ctor = 0,
    .alloc     = udata_derived_alloc,
    .gc        = udata_derived_gc,
    .methods   = udata_derived_methods};
