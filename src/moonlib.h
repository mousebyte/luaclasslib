#include <lauxlib.h>
#include <lua.h>

#define moonL_pushclass(L, index) lua_getfield((L), (index), "__class")
typedef void *(*moonL_Constructor)(lua_State *);
typedef void (*moonL_Destructor)(void *);

#define MOONL_UCLASS_HEADER  \
    const char       *name;  \
    moonL_Constructor alloc; \
    moonL_Destructor  gc;

typedef struct moonL_UClass {
    MOONL_UCLASS_HEADER
} moonL_UClass;

int           moonL_dofile(lua_State *, const char *);
int           moonL_isobject(lua_State *, int);
int           moonL_isclass(lua_State *, int);
void          moonL_checkclass(lua_State *, int, const char *);
int           moonL_getclass(lua_State *, const char *);
moonL_UClass *moonL_getuclass(lua_State *, int);
int           moonL_registerclass(lua_State *, int);
int           moonL_newclass(
              lua_State *,
              const char *,
              const char *,
              const luaL_Reg *,
              moonL_UClass *);
void moonL_openlib(lua_State *);
