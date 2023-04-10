#include "tests.h"

typedef struct BasicClass_Data {
    int *x;
} BasicClass_Data;

static void BasicClass_alloc(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)lua_newuserdatauv(L, sizeof(BasicClass_Data), 1);
    o->x    = (int *)malloc(sizeof(int));
    *(o->x) = luaL_checknumber(L, 3);
    lua_remove(L, -2);
}

static void BasicClass_gc(void *p) {
    BasicClass_Data *o = (BasicClass_Data *)p;
    if (o->x) {
        free(o->x);
        o->x = NULL;
    }
}

static int BasicClass_init(lua_State *L) {
    luaC_superinit(L);
    return 0;
}

static int BasicClass_setx(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)luaC_checkuclass(L, 1, "BasicClass");
    if (o->x) {
        *(o->x) = luaL_checknumber(L, 2);
    }
    return 0;
}

static int BasicClass_sum(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)luaC_checkuclass(L, 1, "BasicClass");
    lua_getfield(L, 1, "n");
    int x = o->x ? *(o->x) : 0;
    int n = lua_type(L, -1) == LUA_TNUMBER ? lua_tonumber(L, -1) : 0;
    lua_pushnumber(L, x + n);
    return 1;
}

static luaC_Class BasicClass = {
    .name      = "BasicClass",
    .parent    = "Base",
    .user_ctor = 1,
    .alloc     = BasicClass_alloc,
    .gc        = BasicClass_gc,
    .methods   = {
                  {"new", BasicClass_init},
                  {"setx", BasicClass_setx},
                  {"sum", BasicClass_sum},
                  {NULL, NULL}}
};

#define FILECLASS_DATA_FIELDS \
    FILE *file;               \
    char *name;

typedef struct FileClass_Data {
    FILECLASS_DATA_FIELDS
} FileClass_Data;

static void
FileClass_populate_fields(FileClass_Data *data, size_t len, const char *str) {
    data->name = (char *)malloc(len + 1);
    data->file = fopen(str, "r");
    strcpy(data->name, str);
}

static void FileClass_alloc(lua_State *L) {
    FileClass_Data *o =
        (FileClass_Data *)lua_newuserdatauv(L, sizeof(FileClass_Data), 1);
    size_t      len;
    const char *str = lua_tolstring(L, 3, &len);
    FileClass_populate_fields(o, len, str);
}

static void FileClass_gc(void *p) {
    FileClass_Data *o = (FileClass_Data *)p;
    if (o->file) {
        fclose(o->file);
        o->file = NULL;
    }
    if (o->name) {
        free(o->name);
        o->name = NULL;
    }
}

static int FileClass_init(lua_State *L) {
    lua_pushvalue(L, 2);
    lua_setfield(L, 1, "x");
    return 0;
}

static int FileClass_filename(lua_State *L) {
    FileClass_Data *o = (FileClass_Data *)luaC_checkuclass(L, 1, "FileClass");
    lua_pushstring(L, o->name);
    return 1;
}

static int FileClass_readline(lua_State *L) {
    FileClass_Data *o = (FileClass_Data *)luaC_checkuclass(L, 1, "FileClass");
    luaL_Buffer     b;
    luaL_buffinit(L, &b);
    int c;
    while ((c = fgetc(o->file)) != EOF && c != '\n')
        luaL_addchar(&b, c);
    luaL_pushresult(&b);
    return 1;
}

static luaC_Class FileClass = {
    .name      = "FileClass",
    .parent    = NULL,
    .user_ctor = 1,
    .alloc     = FileClass_alloc,
    .gc        = FileClass_gc,
    .methods   = {
                  {"foo", foo},
                  {"new", FileClass_init},
                  {"filename", FileClass_filename},
                  {"readline", FileClass_readline},
                  {NULL, NULL}}
};

typedef struct DFileClass_Data {
    FILECLASS_DATA_FIELDS
    size_t arrlen;
    int   *arr;
} DFileClass_Data;

static void DFileClass_alloc(lua_State *L) {
    size_t           len;
    const char      *str = lua_tolstring(L, 3, &len);
    DFileClass_Data *o =
        (DFileClass_Data *)lua_newuserdatauv(L, sizeof(DFileClass_Data), 1);
    FileClass_populate_fields((FileClass_Data *)o, len, str);
    size_t num = luaL_checknumber(L, 4);
    o->arr     = (int *)malloc(sizeof(int) * num);
    o->arrlen  = num;
}

static void DFileClass_gc(void *p) {
    DFileClass_Data *o = (DFileClass_Data *)p;
    if (o->arr) {
        free(o->arr);
        o->arr    = NULL;
        o->arrlen = 0;
    }
}

static int DFileClass_init(lua_State *L) {
    luaC_superinit(L);
    return 0;
}

static int DFileClass_setint(lua_State *L) {
    DFileClass_Data *o =
        (DFileClass_Data *)luaC_checkuclass(L, 1, "DFileClass");
    size_t index = luaL_checknumber(L, 2);
    if (index < o->arrlen) {
        o->arr[index] = luaL_checknumber(L, 3);
    }
    return 0;
}

static int DFileClass_getint(lua_State *L) {
    DFileClass_Data *o =
        (DFileClass_Data *)luaC_checkuclass(L, 1, "DFileClass");
    size_t index = luaL_checknumber(L, 2);
    if (index < o->arrlen) lua_pushnumber(L, o->arr[index]);
    else lua_pushnil(L);
    return 1;
}

static luaC_Class DFileClass = {
    .name      = "DFileClass",
    .parent    = "FileClass",
    .user_ctor = 1,
    .alloc     = DFileClass_alloc,
    .gc        = DFileClass_gc,
    .methods   = {
                  {"new", DFileClass_init},
                  {"setint", DFileClass_setint},
                  {"getint", DFileClass_getint},
                  {NULL, NULL}}
};
