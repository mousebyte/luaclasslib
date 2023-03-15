#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <moonlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void error(lua_State *L, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    lua_close(L);
    exit(EXIT_FAILURE);
}
typedef struct MyStruct {
    double x;
    int    len;
    char   str[];
} MyStruct;

static int newstruct(lua_State *L) {
    double      x   = luaL_checknumber(L, 1);
    const char *str = luaL_checkstring(L, 2);
    size_t      len = strlen(str);
    MyStruct   *s   = (MyStruct *)lua_newuserdatauv(
        L, sizeof(MyStruct) + (sizeof(char) * len), 1);
    luaL_getmetatable(L, "Test.MyStruct");
    lua_setmetatable(L, -2);
    lua_newtable(L);
    lua_setiuservalue(L, -2, 1);
    s->x   = x;
    s->len = len;
    strncpy(s->str, str, len);
    return 1;
}

static int get_x(lua_State *L) {
    MyStruct *s = (MyStruct *)luaL_checkudata(L, 1, "Test.MyStruct");
    lua_pushnumber(L, s->x);
    return 1;
}

static const struct luaL_Reg structlib_f[] = {
    {"new", newstruct},
    {NULL,  NULL     }
};

static const struct luaL_Reg structlib_m[] = {
    {"get_x", get_x},
    {NULL,    NULL }
};
int luaopen_mystruct(lua_State *L) {
    luaL_newmetatable(L, "Test.MyStruct");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    luaL_setfuncs(L, structlib_m, 0);
    luaL_newlib(L, structlib_f);
    lua_setglobal(L, "mystruct");
    return 1;
}

int main() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    moonL_openlib(L);
    moonL_dofile(L, "test2.moon");
    lua_pushstring(L, "hello");
    lua_call(L, 1, 1);
    lua_getfield(L, -1, "squeak");
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);

    // luaopen_mystruct(L);
    // lua_register(L, "uvget", moonL_uvget);
    // lua_register(L, "uvset", moonL_uvset);
    // luaL_dofile(L, "test3.lua");

    /*
    luaL_dofile(s, "Test.lua");
    lua_pcall(s, 0, 1, 0);
    printf("%d\n", lua_type(s, -1));
    lua_pushstring(s, "it works");
    lua_setfield(s, -2, "squeak");
    lua_getfield(s, -1, "squeak");
    if (!lua_isstring(s, -1))
      error(s, "failed!");
    const char *val = lua_tostring(s, -1);
    printf("%s", val);
    */
    lua_close(L);

    return 0;
}
