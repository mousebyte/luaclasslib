#include <lua.h>
#include <luaclasslib.h>
#include <string.h>

#define CLASSLIB_REGISTRY_KEY "luaclass.lib"

static void luaC_setreg(lua_State *L) {
    if (lua_gettop(L) >= 2) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, CLASSLIB_REGISTRY_KEY);
        lua_insert(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);
    }
}

static int luaC_getreg(lua_State *L) {
    int type = LUA_TNIL;
    if (lua_gettop(L) >= 1) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, CLASSLIB_REGISTRY_KEY);
        lua_insert(L, -2);
        type = lua_gettable(L, -2);
        lua_remove(L, -2);
    } else lua_pushnil(L);
    return type;
}

static void luaC_setregfield(lua_State *L, const char *key) {
    if (lua_gettop(L) >= 1) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, CLASSLIB_REGISTRY_KEY);
        lua_insert(L, -2);
        lua_setfield(L, -2, key);
        lua_pop(L, 1);
    }
}

static int luaC_getregfield(lua_State *L, const char *key) {
    luaL_getsubtable(L, LUA_REGISTRYINDEX, CLASSLIB_REGISTRY_KEY);
    int type = lua_getfield(L, -1, key);
    lua_remove(L, -2);
    return type;
}

int luaC_uvget(lua_State *L, int idx, int uv) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);          // put uv behind key
        ret = lua_gettable(L, -2);  // get the value
    } else lua_pushnil(L);          // otherwise push nil
    lua_remove(L, -2);              // remove the uv
    return ret;
}

int luaC_uvset(lua_State *L, int idx, int uv) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -3);    // put uv before key and value
        lua_settable(L, -3);  // set the value
        return 1;
    }
    lua_pop(L, 3);  // otherwise pop uv, key, and value
    return 0;
}

int luaC_getuvfield(lua_State *L, int idx, int uv, const char *k) {
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        ret = lua_getfield(L, -1, k);  // get value if uv is table
    } else lua_pushnil(L);             // otherwise push nil
    lua_remove(L, -2);                 // remove the uv
    return ret;
}

int luaC_setuvfield(lua_State *L, int idx, int uv, const char *k) {
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);       // put uv behind value
        lua_setfield(L, -2, k);  // set the value
        lua_pop(L, 1);           // pop the uv
        return 1;
    }
    lua_pop(L, 2);  // otherwise pop uv and value
    return 0;
}

static int classlib_uvget(lua_State *L) {
    luaC_uvget(L, 1, lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 1);
    return 1;
}

static int classlib_uvset(lua_State *L) {
    luaC_uvset(L, 1, lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 1);
    return 0;
}

void luaC_mcall(lua_State *L, const char *method, int nargs, int nresults) {
    lua_getfield(L, -nargs - 1, method);  // get the method
    lua_pushvalue(L, -nargs - 2);         // push a copy of the object
    lua_rotate(L, -nargs - 2, 2);         // rotate args to top
    lua_call(L, nargs + 1, nresults);
}

int luaC_pmcall(
    lua_State  *L,
    const char *method,
    int         nargs,
    int         nresults,
    int         msgh) {
    msgh = lua_absindex(L, msgh);
    lua_getfield(L, -nargs - 1, method);  // get the method
    lua_pushvalue(L, -nargs - 2);         // push a copy of the object
    lua_rotate(L, -nargs - 2, 2);         // rotate args to top
    return lua_pcall(L, nargs + 1, nresults, msgh);
}

int luaC_isobject(lua_State *L, int index) {
    int top = lua_gettop(L);
    int ret = lua_getmetatable(L, index) &&
              (lua_getfield(L, -1, "__class") == LUA_TTABLE) &&
              (lua_getfield(L, -1, "__base") == LUA_TTABLE) &&
              lua_rawequal(L, -1, -3);
    lua_settop(L, top);
    return ret;
}

int luaC_isclass(lua_State *L, int index) {
    int top = lua_gettop(L);
    lua_pushvalue(L, index);
    int ret = (lua_getfield(L, -1, "__base") == LUA_TTABLE) &&
              (lua_getfield(L, -1, "__class") == LUA_TTABLE) &&
              lua_rawequal(L, -1, -3);
    lua_settop(L, top);
    return ret;
}

int luaC_isinstance(lua_State *L, int index, const char *name) {
    if (lua_getfield(L, index, "__class") == LUA_TTABLE) {
        while (lua_getfield(L, -1, "__name") == LUA_TSTRING) {
            if (strcmp(name, lua_tostring(L, -1)) != 0) {
                lua_pop(L, 1);  // pop name
                if (lua_getfield(L, -1, "__parent") == LUA_TNIL) {
                    lua_pop(L, 1);  // pop nil
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

void *luaC_checkuclass(lua_State *L, int arg, const char *name) {
    if (!lua_isuserdata(L, arg) || !luaC_isinstance(L, arg, name))
        luaL_error(L, "Value is not an instance of class %s", name);
    return lua_touserdata(L, arg);
}

int luaC_getclass(lua_State *L, const char *name) {
    return luaC_getregfield(L, name);
}

luaC_Class *luaC_getuclass(lua_State *L, int index) {
    luaC_Class *ret = NULL;
    if (luaC_isclass(L, index)) {
        lua_pushvalue(L, index);
        if (luaC_getreg(L) == LUA_TLIGHTUSERDATA)
            ret = (luaC_Class *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    return ret;
}

int luaC_registerclass(lua_State *L, int index) {
    if (!luaC_isclass(L, index)) return 0;
    lua_pushvalue(L, index);                                // push the class
    while (lua_getfield(L, -1, "__name") == LUA_TSTRING) {  // get name
        const char *name = lua_tostring(L, -1);
        if (luaC_getregfield(L, name) == LUA_TNIL) {  // class not registered
            lua_pop(L, 1);
            lua_pushvalue(L, -2);       // push class
            luaC_setregfield(L, name);  // register class
            lua_pop(L, 1);              // pop name
            if (lua_getfield(L, -1, "__parent") == LUA_TNIL) {
                lua_pop(L, 2);  // pop nil and class
                return 1;
            } else lua_remove(L, -2);  // parent found, remove previous class
        } else {
            lua_pop(L, 2);  // class registered, pop name and class
            return 1;
        }
    }
    lua_pop(L, 2);
    return 0;
}

// default class __call
static int default_class_call(lua_State *L) {
    // create the object
    luaC_Class *class = luaC_getuclass(L, 1);
    if (!class) lua_newtable(L);
    else {
        class->alloc(L);
        lua_newtable(L);
        lua_setiuservalue(L, -2, 1);
    }
    if (lua_getfield(L, 1, "__base") != LUA_TTABLE) return 0;
    lua_setmetatable(L, -2);            // set object metatable to class base
    lua_pushvalue(L, -1);               // push a copy of object for call
    lua_rotate(L, 2, 2);                // rotate objects before other args
    lua_getfield(L, 1, "__init");       // get init
    lua_insert(L, 3);                   // insert before args
    lua_call(L, lua_gettop(L) - 3, 0);  // call init
    return 1;
}

int luaC_construct(lua_State *L, int nargs, const char *name) {
    if (luaC_getclass(L, name) == LUA_TTABLE) {
        lua_pushcfunction(L, default_class_call);
        lua_insert(L, -nargs - 2);  // insert ctor before args
        lua_insert(L, -nargs - 1);  // insert class after ctor
        lua_call(L, nargs + 1, 1);  // call ctor
        return 1;
    }
    lua_pop(L, 1);
    return 0;
}

int luaC_injectmethod(
    lua_State    *L,
    int           index,
    const char   *method,
    lua_CFunction f) {
    index = lua_absindex(L, index);
    if (f && luaC_isclass(L, index)) {
        lua_pushstring(L, "__base");
        lua_rawget(L, index);       // grab base
        lua_pushstring(L, method);  // key for rawset
        lua_pushstring(L, method);  // key for rawget
        lua_rawget(L, -3);          // grab method from base
        lua_pushcclosure(L, f, 1);  // push into closure
        lua_rawset(L, -3);          // overwrite method
        lua_pop(L, 1);              // pop base
        return 1;
    }
    return 0;
}

int luaC_deferindex(lua_State *L) {
    lua_pushvalue(L, lua_upvalueindex(1));  // grab original __index
    int ret = LUA_TNIL;
    switch (lua_type(L, -1)) {
        case LUA_TTABLE:  // grab value from table
            lua_pushvalue(L, 2);
            ret = lua_gettable(L, -2);
            break;
        case LUA_TFUNCTION:  // grab value from function
            lua_pushvalue(L, 1);
            lua_pushvalue(L, 2);
            lua_call(L, 2, 1);
            ret = lua_type(L, -1);
            break;
        default:  // if it's anything else, push nil
            lua_pop(L, 1);
            lua_pushnil(L);
            break;
    }
    lua_remove(L, -2);  // remove original __index
    return ret;
}

void luaC_defernewindex(lua_State *L) {
    if (lua_type(L, lua_upvalueindex(1)) != LUA_TFUNCTION) {
        luaL_getmetafield(L, 1, "__class");
        if (luaC_getuclass(L, -1)) lua_pushcfunction(L, classlib_uvset);
        else lua_getglobal(L, "rawset");
        lua_remove(L, -2);
    } else lua_pushvalue(L, lua_upvalueindex(1));  // grab original __newindex
    // push copies of all the args and call it
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_call(L, 3, 0);
}

int luaC_getparentfield(lua_State *L, int index, int depth, const char *name) {
    if (depth < 1 || !luaC_isobject(L, index)) {
        lua_pushnil(L);
        return LUA_TNIL;
    }

    luaC_pushclass(L, index);  // push its class
    while (depth > 0) {        // walk the heirarchy
        if (lua_getfield(L, -1, "__parent") == LUA_TNIL) {
            lua_remove(L, -2);  // remove previous class
            return LUA_TNIL;
        }
        depth--;
        lua_remove(L, -2);  // remove previous class
    }

    int ret = lua_getfield(L, -1, name);
    lua_remove(L, -2);  // remove class
    return ret;
}

void luaC_super(lua_State *L, const char *name, int nresults) {
    if (luaC_getparentfield(L, 1, 1, name) != LUA_TFUNCTION) {
        lua_pop(L, 1);
        return;
    }

    int nargs = lua_gettop(L) - 1;
    luaL_checkstack(L, nargs, "Too many arguments for super invocation.");
    for (int i = 1; i <= nargs; i++)  // push a copy of the whole stack
        lua_pushvalue(L, i);
    lua_call(L, nargs, nresults);
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
    if (luaC_deferindex(L) == LUA_TNIL) {
        lua_pop(L, 1);
        classlib_uvget(L);  // check user value for key
    }
    return 1;
}

static int index_invalid(lua_State *L) {
    return luaL_error(
        L, "attempt to index an object that was already garbage collected");
}

static int default_udata_gc(lua_State *L) {
    if (lua_type(L, 1) == LUA_TUSERDATA && luaC_pushclass(L, 1) == LUA_TTABLE) {
        // loop through the class and all its parents and call their finalizers
        do {
            luaC_Class *class = luaC_getuclass(L, -1);
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

int luaC_newuclass(
    lua_State      *L,
    const char     *name,
    const char     *parent,
    const luaL_Reg *methods,
    luaC_Class     *uclass,
    int             userCtor) {
    if (luaC_getregfield(L, name) != LUA_TNIL) return 0;
    lua_pop(L, 1);

    lua_newtable(L);               // base table
    luaL_setfuncs(L, methods, 0);  // load in methods
    lua_newtable(L);               // class table
    lua_newtable(L);               // class metatable
    int class_mt       = lua_gettop(L);
    int class          = class_mt - 1;
    int base           = class - 1;

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

    lua_pushcfunction(L, init);
    lua_setfield(L, class, "__init");  // set class __init
    lua_pushvalue(L, base);
    lua_setfield(L, class, "__base");  // set class __base
    lua_pushstring(L, name);
    lua_setfield(L, class, "__name");  // set class __name

    lua_pushvalue(L, class);
    lua_setfield(L, base, "__class");  // set base __class

    if (uclass) {
        lua_pushvalue(L, base);
        lua_pushcclosure(L, default_udata_index, 1);
        lua_setfield(L, base, "__index");  // set base __index
        lua_pushcfunction(L, classlib_uvset);
        lua_setfield(L, base, "__newindex");  // set base __newindex
        lua_pushcfunction(L, default_udata_gc);
        lua_setfield(L, base, "__gc");  // set base __gc

        lua_pushvalue(L, class);
        lua_pushlightuserdata(L, uclass);
        luaC_setreg(L);  // register uclass
    } else {
        lua_pushvalue(L, base);
        lua_setfield(L, base, "__index");  // set base __index to self
    }

    // handle constructor
    if (userCtor) {
        lua_pushcfunction(L, default_class_call);
        lua_setfield(L, class_mt, "__call");  // set meta __call
    }

    // handle inheritance
    if (parent == NULL) {                      // no parent
        lua_pushvalue(L, base);                // push base
        lua_setfield(L, class_mt, "__index");  // set meta __index to base
    } else if (luaC_getregfield(L, parent) == LUA_TTABLE) {  // else get parent
        lua_pushvalue(L, base);                              // push base
        lua_pushcclosure(L, default_class_index, 1);  // wrap it in a closure
        lua_setfield(L, class_mt, "__index");         // set meta __index
        lua_getfield(L, -1, "__base");                // get parent __base
        lua_setmetatable(L, base);  // set base metatable to parent base
        lua_setfield(L, class, "__parent");  // set class __parent to parent
    } else {                                 // else parent not registered
        lua_pop(L, 3);                       // clean up and return
        return 0;
    }

    lua_setmetatable(L, class);  // set class metatable

    if (lua_getfield(L, -1, "__parent") != LUA_TNIL) {
        if (lua_getfield(L, -1, "__inherited") != LUA_TNIL) {
            lua_insert(L, -2);        // put inherited behind parent
            lua_pushvalue(L, class);  // push our (derived) class
            lua_call(L, 2, 0);        // call inherited
        } else lua_pop(L, 2);         // else pop nil and parent
    } else lua_pop(L, 1);             // else pop nil

    lua_pushvalue(L, class);
    luaC_setregfield(L, name);  // register class
    lua_remove(L, base);        // remove base from stack
    return 1;
}

void luaopen_class(lua_State *L) {
    lua_register(L, "uvget", classlib_uvget);
    lua_register(L, "uvset", classlib_uvset);
}
