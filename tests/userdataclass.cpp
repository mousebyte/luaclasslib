#include "tests.h"
extern "C" {

typedef struct BasicClass_Data {
    int *x;
} BasicClass_Data;

void BasicClass_alloc(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)lua_newuserdatauv(L, sizeof(BasicClass_Data), 1);
    o->x    = (int *)malloc(sizeof(int));
    *(o->x) = luaL_checknumber(L, 3);
    lua_remove(L, -2);
}

void BasicClass_gc(void *p) {
    BasicClass_Data *o = (BasicClass_Data *)p;
    if (o->x) {
        free(o->x);
        o->x = NULL;
    }
}

static moonL_UClass BasicClass {"BasicClass", BasicClass_alloc, BasicClass_gc};

int BasicClass_init(lua_State *L) {
    moonL_superinit(L);
    return 0;
}

int BasicClass_setx(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)moonL_checkuclass(L, 1, "BasicClass");
    if (o->x) {
        *(o->x) = luaL_checknumber(L, 2);
    }
    return 0;
}

int BasicClass_sum(lua_State *L) {
    BasicClass_Data *o =
        (BasicClass_Data *)moonL_checkuclass(L, 1, "BasicClass");
    lua_getfield(L, 1, "n");
    int x = o->x ? *(o->x) : 0;
    int n = lua_type(L, -1) == LUA_TNUMBER ? lua_tonumber(L, -1) : 0;
    lua_pushnumber(L, x + n);
    return 1;
}

static luaL_Reg BasicClass_funcs[] = {
    {"new",  BasicClass_init},
    {"setx", BasicClass_setx},
    {"sum",  BasicClass_sum },
    {NULL,   NULL           }
};

#define FILECLASS_DATA_FIELDS \
    FILE *file;               \
    char *name;

typedef struct FileClass_Data {
    FILECLASS_DATA_FIELDS
} FileClass_Data;

void FileClass_populate_fields(
    FileClass_Data *data,
    size_t          len,
    const char     *str) {
    data->name = (char *)malloc(len + 1);
    data->file = fopen(str, "r");
    strcpy(data->name, str);
}

void FileClass_alloc(lua_State *L) {
    FileClass_Data *o =
        (FileClass_Data *)lua_newuserdatauv(L, sizeof(FileClass_Data), 1);
    size_t      len;
    const char *str = lua_tolstring(L, 3, &len);
    FileClass_populate_fields(o, len, str);
}

void FileClass_gc(void *p) {
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

static moonL_UClass FileClass {"FileClass", FileClass_alloc, FileClass_gc};

int FileClass_init(lua_State *L) {
    lua_pushvalue(L, 2);
    lua_setfield(L, 1, "x");
    return 0;
}

int FileClass_filename(lua_State *L) {
    FileClass_Data *o = (FileClass_Data *)moonL_checkuclass(L, 1, "FileClass");
    lua_pushstring(L, o->name);
    return 1;
}

int FileClass_readline(lua_State *L) {
    FileClass_Data *o = (FileClass_Data *)moonL_checkuclass(L, 1, "FileClass");
    luaL_Buffer     b;
    luaL_buffinit(L, &b);
    int c;
    while ((c = fgetc(o->file)) != EOF && c != '\n')
        luaL_addchar(&b, c);
    luaL_pushresult(&b);
    return 1;
}

static luaL_Reg FileClass_funcs[] = {
    {"foo",      foo               },
    {"new",      FileClass_init    },
    {"filename", FileClass_filename},
    {"readline", FileClass_readline},
    {NULL,       NULL              }
};

typedef struct DFileClass_Data {
    FILECLASS_DATA_FIELDS
    size_t arrlen;
    int   *arr;
} DFileClass_Data;

void DFileClass_alloc(lua_State *L) {
    size_t           len;
    const char      *str = lua_tolstring(L, 3, &len);
    DFileClass_Data *o =
        (DFileClass_Data *)lua_newuserdatauv(L, sizeof(DFileClass_Data), 1);
    FileClass_populate_fields((FileClass_Data *)o, len, str);
    size_t num = luaL_checknumber(L, 4);
    o->arr     = (int *)malloc(sizeof(int) * num);
    o->arrlen  = num;
}

void DFileClass_gc(void *p) {
    DFileClass_Data *o = (DFileClass_Data *)p;
    if (o->arr) {
        free(o->arr);
        o->arr    = NULL;
        o->arrlen = 0;
    }
}

static moonL_UClass DFileClass {"DFileClass", DFileClass_alloc, DFileClass_gc};

int DFileClass_init(lua_State *L) {
    moonL_superinit(L);
    return 0;
}

int DFileClass_setint(lua_State *L) {
    DFileClass_Data *o =
        (DFileClass_Data *)moonL_checkuclass(L, 1, "DFileClass");
    size_t index = luaL_checknumber(L, 2);
    if (index < o->arrlen) {
        o->arr[index] = luaL_checknumber(L, 3);
    }
    return 0;
}

int DFileClass_getint(lua_State *L) {
    DFileClass_Data *o =
        (DFileClass_Data *)moonL_checkuclass(L, 1, "DFileClass");
    size_t index = luaL_checknumber(L, 2);
    if (index < o->arrlen) lua_pushnumber(L, o->arr[index]);
    else lua_pushnil(L);
    return 1;
}

static luaL_Reg DFileClass_funcs[] = {
    {"new",    DFileClass_init  },
    {"setint", DFileClass_setint},
    {"getint", DFileClass_getint},
    {NULL,     NULL             }
};
}

TEST_CASE("User Data Classes") {
    using doctest::String;
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_moonaux(L);

    SUBCASE("Basic User Data Class") {
        moonL_newuclass(L, "FileClass", NULL, FileClass_funcs, &FileClass, 1);
        REQUIRE(moonL_isclass(L, -1));
        lua_pop(L, 1);

        lua_pushnumber(L, 16);
        lua_pushstring(L, "Derived.moon");
        moonL_construct(L, 2, "FileClass");
        REQUIRE(moonL_isobject(L, -1));
        REQUIRE(moonL_isinstance(L, -1, "FileClass"));

        lua_pushnumber(L, 4);
        moonL_mcall(L, "foo", 1, 1);
        REQUIRE(lua_tonumber(L, -1) == 64);
        lua_pop(L, 1);

        moonL_mcall(L, "filename", 0, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
        lua_pop(L, 1);

        moonL_mcall(L, "readline", 0, 1);
        REQUIRE(lua_type(L, -1) == LUA_TSTRING);
        REQUIRE(String(lua_tostring(L, -1)) == "Base = require \"Base\"");
        lua_pop(L, 2);
    }

    SUBCASE("Derived User Data Class") {
        SUBCASE("From Basic Class") {
            moonL_dofile(L, "Base.moon");
            REQUIRE(lua_type(L, -1) == LUA_TTABLE);
            REQUIRE(moonL_isclass(L, -1));
            REQUIRE(moonL_registerclass(L, -1));
            lua_pop(L, 1);

            moonL_newuclass(
                L, "BasicClass", "Base", BasicClass_funcs, &BasicClass, 1);
            REQUIRE(moonL_isclass(L, -1));

            REQUIRE(lua_getfield(L, -1, "var") == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Eek!");
            lua_pop(L, 2);

            lua_pushstring(L, "got any cheese?");
            lua_pushnumber(L, 666);
            moonL_construct(L, 2, "BasicClass");
            REQUIRE(moonL_isobject(L, -1));
            REQUIRE(moonL_isinstance(L, -1, "BasicClass"));

            moonL_mcall(L, "sum", 0, 1);
            REQUIRE(lua_tonumber(L, -1) == 666);
            lua_pop(L, 1);

            lua_pushnumber(L, -444);
            moonL_mcall(L, "squeak", 1, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "n is now -444.0, squeak!");
            lua_pop(L, 1);

            moonL_mcall(L, "sum", 0, 1);
            REQUIRE(lua_tonumber(L, -1) == 222);
            lua_pop(L, 2);
        }

        SUBCASE("From User Data Class") {
            moonL_newuclass(
                L, "FileClass", NULL, FileClass_funcs, &FileClass, 1);
            REQUIRE(moonL_isclass(L, -1));
            lua_pop(L, 1);

            moonL_newuclass(
                L, "DFileClass", "FileClass", DFileClass_funcs, &DFileClass, 1);
            REQUIRE(moonL_isclass(L, -1));
            lua_pop(L, 1);

            lua_pushnumber(L, 77);
            lua_pushstring(L, "Derived.moon");
            lua_pushnumber(L, 2);
            moonL_construct(L, 3, "DFileClass");
            REQUIRE(moonL_isobject(L, -1));
            REQUIRE(moonL_isinstance(L, -1, "DFileClass"));

            lua_pushnumber(L, 2);
            moonL_mcall(L, "foo", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 154);
            lua_pop(L, 1);

            moonL_mcall(L, "filename", 0, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Derived.moon");
            lua_pop(L, 1);

            moonL_mcall(L, "readline", 0, 1);
            REQUIRE(lua_type(L, -1) == LUA_TSTRING);
            REQUIRE(String(lua_tostring(L, -1)) == "Base = require \"Base\"");
            lua_pop(L, 1);

            lua_pushnumber(L, 0);
            lua_pushnumber(L, 37);
            moonL_mcall(L, "setint", 2, 0);

            lua_pushnumber(L, 1);
            lua_pushnumber(L, 99);
            moonL_mcall(L, "setint", 2, 0);

            lua_pushnumber(L, 0);
            moonL_mcall(L, "getint", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 37);
            lua_pop(L, 1);

            lua_pushnumber(L, 1);
            moonL_mcall(L, "getint", 1, 1);
            REQUIRE(lua_tonumber(L, -1) == 99);
            lua_pop(L, 2);
        }
    }

    lua_close(L);
}
