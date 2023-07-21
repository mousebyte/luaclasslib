#include "file.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    FILE *file;
    char *name;
} file_t;

// allocator
static void file_alloc(lua_State *L) {
    // must have at least one user value
    lua_newuserdatauv(L, sizeof(file_t), 1);
}

// garbage collector. free any resources used by the object here.
// note that we do not free the pointer itself; it is a userdata and
// will be freed by the lua garbage collector.
static void file_gc(lua_State *L, void *p) {
    file_t *o = (file_t *)p;
    if (o->file) {
        fclose(o->file);
        o->file = NULL;
    }
    if (o->name) {
        free(o->name);
        o->name = NULL;
    }
}

// init function. equivalent to the "new" function in
// moonscript classes. performs the actual setup of the object.
static int file_init(lua_State *L) {
    file_t     *o = (file_t *)luaC_checkuclass(L, 1, "File");
    size_t      len;
    const char *str = lua_tolstring(L, 2, &len);
    o->name         = (char *)malloc(len + 1);
    o->file         = fopen(str, "r");
    strcpy(o->name, str);
    return 0;
}

// retrieves the filename. we could also have stored this as a
// standard Lua value in the userdata's user value.
static int file_filename(lua_State *L) {
    file_t *o = (file_t *)luaC_checkuclass(L, 1, "File");
    lua_pushstring(L, o->name);
    return 1;
}

// reads a line from the file.
static int file_readline(lua_State *L) {
    file_t     *o = (file_t *)luaC_checkuclass(L, 1, "File");
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    int c;
    while ((c = fgetc(o->file)) != EOF && c != '\n')
        luaL_addchar(&b, c);
    luaL_pushresult(&b);
    return 1;
}

static luaL_Reg file_methods[] = {
    {"new",      file_init    },
    {"filename", file_filename},
    {"readline", file_readline},
    {NULL,       NULL         }
};

luaC_Class file_class = {
    .name      = "File",
    .module    = "lcltests",
    .parent    = NULL,
    .user_ctor = 1,
    .alloc     = file_alloc,
    .gc        = file_gc,
    .methods   = file_methods};
