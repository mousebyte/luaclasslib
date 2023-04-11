#include "blocking_signal.h"
#include "signal.h"

typedef struct {
    SIGNAL_HEADER
    int blocked;
} blocking_signal;

static void blocking_signal_alloc(lua_State *L) {
    blocking_signal *sig =
        (blocking_signal *)lua_newuserdatauv(L, sizeof(blocking_signal), 2);
    lua_newtable(L);
    lua_setiuservalue(L, -2, 2);
    reflist_init(&sig->slots);
    sig->blocked = 0;
}

static int blocking_signal_block(lua_State *L) {
    blocking_signal *sig =
        (blocking_signal *)luaC_checkuclass(L, 1, "BlockingSignal");
    sig->blocked = 1;
    return 0;
}

static int blocking_signal_unblock(lua_State *L) {
    blocking_signal *sig =
        (blocking_signal *)luaC_checkuclass(L, 1, "BlockingSignal");
    sig->blocked = 0;
    return 0;
}

static int blocking_signal_call(lua_State *L) {
    blocking_signal *sig =
        (blocking_signal *)luaC_checkuclass(L, 1, "BlockingSignal");
    if (!sig->blocked) luaC_super(L, "__call", lua_gettop(L) - 1, 0);
    return 0;
}

static luaL_Reg blocking_signal_methods[] = {
    {"block",   blocking_signal_block  },
    {"unblock", blocking_signal_unblock},
    {"__call",  blocking_signal_call   },
    {NULL,      NULL                   }
};

luaC_Class blocking_signal_class = {
    .name      = "BlockingSignal",
    .parent    = "Signal",
    .user_ctor = 0,
    .alloc     = blocking_signal_alloc,
    .gc        = NULL,
    .methods   = blocking_signal_methods};
