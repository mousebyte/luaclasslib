#include <lua.h>
#include <moonaux.h>
#include <string.h>

#define MOONLIB_REGISTRY_KEY "moonscript.lib"

static void moonL_setreg(lua_State *L) {
    if (lua_gettop(L) >= 2) {
        lua_getfield(L, LUA_REGISTRYINDEX, MOONLIB_REGISTRY_KEY);
        lua_insert(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);
    }
}

static int moonL_getreg(lua_State *L) {
    int type = LUA_TNIL;
    if (lua_gettop(L) >= 1) {
        lua_getfield(L, LUA_REGISTRYINDEX, MOONLIB_REGISTRY_KEY);
        lua_insert(L, -2);
        type = lua_gettable(L, -2);
        lua_remove(L, -2);
    } else lua_pushnil(L);
    return type;
}

static void moonL_setregfield(lua_State *L, const char *key) {
    if (lua_gettop(L) >= 1) {
        lua_getfield(L, LUA_REGISTRYINDEX, MOONLIB_REGISTRY_KEY);
        lua_insert(L, -2);
        lua_setfield(L, -2, key);
        lua_pop(L, 1);
    }
}

static int moonL_getregfield(lua_State *L, const char *key) {
    lua_getfield(L, LUA_REGISTRYINDEX, MOONLIB_REGISTRY_KEY);
    int type = lua_getfield(L, -1, key);
    lua_remove(L, -2);
    return type;
}

static int moonL_uvget(lua_State *L) {
    if (lua_type(L, 1) == LUA_TUSERDATA &&
        lua_getiuservalue(L, 1, 1) == LUA_TTABLE) {
        lua_insert(L, 2);
        lua_gettable(L, 2);
    } else lua_pushnil(L);
    return 1;
}

static int moonL_uvset(lua_State *L) {
    if (lua_type(L, 1) == LUA_TUSERDATA &&
        lua_getiuservalue(L, 1, 1) == LUA_TTABLE) {
        lua_insert(L, 2);
        lua_settable(L, 2);
    }
    return 0;
}

int moonL_dofile(lua_State *L, const char *name) {
    lua_getfield(L, LUA_REGISTRYINDEX, "moonscript.base");
    lua_getfield(L, -1, "dofile");
    lua_remove(L, -2);
    lua_pushstring(L, name);
    return lua_pcall(L, 1, LUA_MULTRET, 0);
}
/**
 * @brief Checks if the value at the given index is an instance of a Moonscript
 * class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is an instance of a Moonscript class, and 0 otherwise.
 */
int moonL_isobject(lua_State *L, int index) {
    int top = lua_gettop(L);
    int ret = lua_getmetatable(L, index) &&
              (lua_getfield(L, -1, "__class") == LUA_TTABLE) &&
              (lua_getfield(L, -1, "__base") == LUA_TTABLE) &&
              lua_rawequal(L, -1, -3);
    lua_settop(L, top);
    return ret;
}

/**
 * @brief Checks if the value at the given index is a Moonscript class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is a Moonscript class, and 0 otherwise.
 */
int moonL_isclass(lua_State *L, int index) {
    int top = lua_gettop(L);
    lua_pushvalue(L, index);
    int ret = (lua_getfield(L, -1, "__base") == LUA_TTABLE) &&
              (lua_getfield(L, -1, "__class") == LUA_TTABLE) &&
              lua_rawequal(L, -1, -3);
    lua_settop(L, top);
    return ret;
}

/**
 * @brief Checks if the value at the given index is an instance of the class
 * named *name*.
 *
 * @param L The Lua state.
 * @param arg The stack index to check.
 * @param name The name of the class.
 *
 * @return 1 if the value is an instance of *name*, and 0 otherwise.
 */
int moonL_isinstance(lua_State *L, int index, const char *name) {
    if (lua_getfield(L, index, "__class") == LUA_TTABLE) {
        while (lua_getfield(L, -1, "__name") == LUA_TSTRING) {
            if (strcmp(name, lua_tostring(L, -1)) != 0) {
                lua_pop(L, 1);  // pop name
                if (lua_getfield(L, -1, "__parent") == LUA_TNIL) {
                    lua_pop(L, 1);  // pop parent class
                    break;
                } else lua_remove(L, -2);  // remove previous class
            } else {
                lua_pop(L, 2);  // pop name and class
                return 1;
            }
        }
        lua_pop(L, 1);  // pop name
    }
    lua_pop(L, 1);  // pop class
    return 0;
}

/**
 * @brief Checks if the function argument *arg* is an instance of the userdata
 * class named *name* and returns the userdata's memory-block address.
 *
 * @param L The Lua state.
 * @param arg The arg to check.
 * @param name The name of the class.
 *
 * @return A pointer to the userdata.
 */
void *moonL_checkuclass(lua_State *L, int arg, const char *name) {
    if (!lua_isuserdata(L, arg) || !moonL_isinstance(L, arg, name))
        luaL_error(L, "Value is not an instance of class %s", name);
    return lua_touserdata(L, arg);
}

/**
 * @brief Pushes onto the stack the class registered under the given *name*.
 *
 * @param L The Lua state.
 * @param name The class name.
 *
 * @return The type of the pushed value.
 */
int moonL_getclass(lua_State *L, const char *name) {
    return moonL_getregfield(L, name);
}

/**
 * @brief Gets a pointer to the user data class associated with the Moonscript
 * class at the given stack index.
 *
 * @param L The Lua state.
 * @param index The stack index of the Moonscript class.
 *
 * @return A pointer to the user data class, or NULL if none was found.
 */
moonL_UClass *moonL_getuclass(lua_State *L, int index) {
    moonL_UClass *ret = NULL;
    if (moonL_isclass(L, index)) {
        lua_pushvalue(L, index);
        if (moonL_getreg(L) == LUA_TLIGHTUSERDATA)
            ret = (moonL_UClass *)lua_touserdata(L, -1);
        lua_pop(L, 2);
    }
    return ret;
}

/**
 * @brief Adds the Moonscript class at the given stack index to the moonlib
 * registry.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise.
 */
int moonL_registerclass(lua_State *L, int index) {
    if (!moonL_isclass(L, index)) return 0;
    lua_pushvalue(L, index);                             // push the class
    if (lua_getfield(L, -1, "__name") == LUA_TSTRING) {  // get name
        const char *name = lua_tostring(L, -1);
        if (moonL_getregfield(L, name) == LUA_TNIL) {  // class not registered
            lua_pop(L, 1);
            lua_insert(L, -2);           // put class in front of name
            moonL_setregfield(L, name);  // register class
            lua_pop(L, 1);               // pop name
            return 1;
        }
        lua_pop(L, 3);
        return 1;
    }
    lua_pop(L, 2);
    return 0;
}

/**
 * @brief Construct an instance of a Moonscript class.
 *
 * @param L The Lua state.
 * @param nargs The number of arguments on the stack to pass to the constructor.
 * @param name The name of the Moonscript class.
 *
 * @return 1 if the object was successfully constructed, and 0 otherwise.
 */
int moonL_construct(lua_State *L, int nargs, const char *name) {
    if (moonL_getclass(L, name) == LUA_TTABLE) {
        lua_insert(L, -nargs);
        lua_call(L, nargs, 1);
        return 1;
    }
    lua_pop(L, 1);
    return 0;
}

int moonL_injectmethod(
    lua_State    *L,
    int           index,
    const char   *method,
    lua_CFunction f) {
    if (f && moonL_isclass(L, index)) {
        lua_getfield(L, index, "__base");  // grab base
        lua_getfield(L, -1, method);       // grab method from base
        lua_pushcclosure(L, f, 1);         // push into closure
        lua_setfield(L, -1, method);       // overwrite method
        lua_pop(L, 1);                     // pop base
        return 1;
    }
    return 0;
}

int moonL_deferindex(lua_State *L) {
    lua_pushvalue(L, lua_upvalueindex(1));
    int ret = LUA_TNIL;
    switch (lua_type(L, -1)) {
        case LUA_TTABLE:
            lua_pushvalue(L, 2);
            ret = lua_gettable(L, -2);
            break;
        case LUA_TFUNCTION:
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            ret = lua_type(L, -1);
            break;
        default:
            lua_pop(L, 1);
            lua_pushnil(L);
            break;
    }
    lua_remove(L, -2);
    return ret;
}

void moonL_defernewindex(lua_State *L) {
    lua_pushvalue(L, lua_upvalueindex(1));
    if (lua_type(L, -1) == LUA_TFUNCTION) {
        lua_pushvalue(L, 1);
        lua_pushvalue(L, 2);
        lua_pushvalue(L, 3);
        lua_call(L, 3, 0);
    }
}

// default class __call
static int default_class_call(lua_State *L) {
    // create the object
    moonL_UClass *class = moonL_getuclass(L, 1);
    if (!class) lua_newtable(L);
    else {
        class->alloc(L);
        lua_newtable(L);
        lua_setiuservalue(L, -2, 1);
    }
    if (lua_getfield(L, 1, "__base") != LUA_TTABLE) return 0;  // get base
    lua_setmetatable(L, -2);            // set object metatable to class base
    lua_pushvalue(L, -1);               // push a copy of object for call
    lua_rotate(L, 3, 2);                // rotate objects before other args
    lua_getfield(L, 1, "__init");       // get init
    lua_insert(L, 3);                   // insert before args
    lua_call(L, lua_gettop(L) - 3, 0);  // call init
    return 1;
}

// default class __init
static int default_init(lua_State *L) {
    return 0;
}

// default subclass __index
static int default_class_index(lua_State *L) {
    // check upvalue (base) for key
    lua_pushvalue(L, 2);
    if (lua_rawget(L, lua_upvalueindex(1)) == LUA_TNIL) {
        lua_pop(L, 1);
        lua_pushstring(L, "__parent");
        if (lua_rawget(L, 1) != LUA_TNIL) {  // get parent from arg 1 (self)
            lua_insert(L, -2);               // put parent behind key
            lua_gettable(L, -2);             // get value (or nil) from parent
        }
    }
    return 1;
}

// default __index for userdata classes
static int default_udata_index(lua_State *L) {
    // check upvalue (base) for key
    if (moonL_deferindex(L) == LUA_TNIL) {
        lua_pop(L, 1);
        moonL_uvget(L);  // check user value for key
    }
    return 1;
}

static int index_invalid(lua_State *L) {
    return luaL_error(
        L, "attempt to index an object that was already garbage collected");
}

static int default_udata_gc(lua_State *L) {
    if (lua_type(L, 1) == LUA_TUSERDATA &&
        moonL_pushclass(L, 1) == LUA_TTABLE) {
        // loop through the class and all its parents and call their finalizers
        do {
            moonL_UClass *class = moonL_getuclass(L, -1);
            if (class && class->gc) class->gc(lua_touserdata(L, 1));
        } while (lua_getfield(L, -1, "__parent") != LUA_TNIL);
    }

    // clear the metatable
    lua_newtable(L);
    lua_pushcfunction(L, index_invalid);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, index_invalid);
    lua_setfield(L, -2, "__newindex");
    lua_setmetatable(L, 1);
    return 0;
}

/**
 * @brief Creates and registers a new Moonscript class.
 *
 * @param L The Lua state.
 * @param name The class name.
 * @param parent The parent class name. Must be in the registry.
 * @param methods The class methods.
 * @param uclass The uclass, if creating a user data class.
 *
 * @return 1 if the class was successfully created and registered, and 0
 * otherwise.
 */
int moonL_newclass(
    lua_State      *L,
    const char     *name,
    const char     *parent,
    const luaL_Reg *methods,
    moonL_UClass   *uclass) {
    if (moonL_getregfield(L, name) != LUA_TNIL) return 0;
    lua_pop(L, 1);

    lua_newtable(L);  // make base table
    int base = lua_gettop(L);
    luaL_setfuncs(L, methods, 0);  // load in methods

    // find init function
    lua_CFunction init = default_init;
    lua_pushnil(L);  // first key

    while (lua_next(L, base) != 0) {  // push next key/value pair in base
        if ((strcmp(lua_tostring(L, -2), "new") == 0) &&
            lua_iscfunction(L, -1)) {       // compare key to "new"
            init = lua_tocfunction(L, -1);  // grab the value as a func
            lua_pop(L, 1);                  // pop the value
            lua_pushnil(L);                 // push nil
            lua_rawset(L, base);            // (un)set "new" on base
            break;                          // and break
        }
        lua_pop(L, 1);  // otherwise pop the value, leaving the key
    }

    lua_newtable(L);  // make class table
    int class = lua_gettop(L);
    lua_pushcfunction(L, init);
    lua_setfield(L, class, "__init");  // set class __init
    lua_pushvalue(L, base);
    lua_setfield(L, class, "__base");  // set class __base
    lua_pushstring(L, name);
    lua_setfield(L, class, "__name");  // set class __name

    lua_pushvalue(L, class);
    lua_setfield(L, base, "__class");  // set base __class

    lua_newtable(L);  // make class metatable
    int class_mt = lua_gettop(L);
    lua_pushvalue(L, base);                      // push base
    lua_pushcclosure(L, default_class_call, 1);  // wrap it in a closure
    lua_setfield(L, class_mt, "__call");         // set meta __call

    // handle inheritance
    if (parent == NULL) {                      // no parent
        lua_pushvalue(L, base);                // push base
        lua_setfield(L, class_mt, "__index");  // set meta __index to base
    } else if (moonL_getregfield(L, parent) == LUA_TTABLE) {  // else get parent
        lua_pushvalue(L, base);                               // push base
        lua_pushcclosure(L, default_class_index, 1);  // wrap it in a closure
        lua_setfield(L, class_mt, "__index");         // set meta __index
        lua_getfield(L, -1, "__base");                // get parent __base
        lua_setmetatable(L, base);  // set base metatable to parent base
        lua_pushvalue(L, -1);
        lua_setfield(L, class, "__parent");  // set class __parent to parent

        // check parent for __inherited
        if (lua_getfield(L, -1, "__inherited") != LUA_TNIL) {
            lua_insert(L, -2);        // put inherited behind parent
            lua_pushvalue(L, class);  // push our (derived) class
            lua_call(L, 2, 0);        // call inherited
        } else lua_pop(L, 1);         // else pop the nil
    } else {                          // else parent not registered
        lua_pop(L, 3);
        return 0;  // clear stack and return 0
    }

    lua_setmetatable(L, class);  // set class metatable

    if (uclass) {
        lua_pushvalue(L, base);
        lua_pushcclosure(L, default_udata_index, 1);
        lua_setfield(L, base, "__index");  // set base __index
        lua_pushcfunction(L, moonL_uvset);
        lua_setfield(L, base, "__newindex");  // set base __newindex
        lua_pushcfunction(L, default_udata_gc);
        lua_setfield(L, base, "__gc");  // set base __gc

        lua_pushvalue(L, class);
        lua_pushlightuserdata(L, uclass);
        moonL_setreg(L);  // register uclass
    } else {
        lua_pushvalue(L, base);
        lua_setfield(L, base, "__index");  // set base __index to self
    }

    lua_remove(L, base);  // remove base from stack

    lua_pushvalue(L, class);
    moonL_setregfield(L, name);  // register class
    return 1;
}

void luaopen_moonaux(lua_State *L) {
    lua_newtable(L);
    luaL_dostring(L, "return require('moonscript.base')");
    lua_pop(L, 1);
    lua_setfield(L, LUA_REGISTRYINDEX, "moonscript.base");
    lua_setfield(L, LUA_REGISTRYINDEX, MOONLIB_REGISTRY_KEY);
    lua_register(L, "uvget", moonL_uvget);
    lua_register(L, "uvset", moonL_uvset);
}
