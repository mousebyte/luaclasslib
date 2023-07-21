#include "signal.h"
#include <string.h>

typedef struct {
    SIGNAL_HEADER
} signal;

static void signal_alloc(lua_State *L) {
    signal *sig = (signal *)lua_newuserdatauv(L, sizeof(signal), 2);
    lua_newtable(L);
    lua_setiuservalue(L, -2, 2);
    reflist_init(&sig->slots);
}

static void signal_gc(lua_State *L, void *p) {
    reflist_wipe(&((signal *)p)->slots);
}

static int signal_connect(lua_State *L) {
    signal     *sig = (signal *)luaC_checkuclass(L, 1, "Signal");
    const void *ref = lua_topointer(L, 2);
    if (ref != NULL) {
        luaC_uvrawsetp(L, 1, 2, ref);
        reflist_insert(&sig->slots, ref);
    }
    return 0;
}

static int signal_disconnect(lua_State *L) {
    signal      *sig  = (signal *)luaC_checkuclass(L, 1, "Signal");
    const void  *ref  = lua_topointer(L, 2);
    const void **elem = reflist_lookup(&sig->slots, &ref);
    if (elem != NULL) {
        lua_pushnil(L);
        luaC_uvrawsetp(L, 1, 2, *elem);
        reflist_remove(&sig->slots, elem);
    }
    return 0;
}

static int signal_call(lua_State *L) {
    signal *sig   = (signal *)luaC_checkuclass(L, 1, "Signal");
    int     nargs = lua_gettop(L) - 1;
    lua_getiuservalue(L, 1, 2);
    lua_insert(L, 2);
    foreach (slot, sig->slots) {
        lua_rawgetp(L, 2, *slot);
        for (int i = 0; i < nargs; i++)
            lua_pushvalue(L, i + 3);
        lua_call(L, nargs, 0);
    }
    return 0;
}

static luaL_Reg signal_methods[] = {
    {"connect",    signal_connect   },
    {"disconnect", signal_disconnect},
    {"__call",     signal_call      },
    {NULL,         NULL             }
};

luaC_Class signal_class = {
    .name      = "Signal",
    .parent    = NULL,
    .user_ctor = 0,
    .alloc     = signal_alloc,
    .gc        = signal_gc,
    .methods   = signal_methods};
