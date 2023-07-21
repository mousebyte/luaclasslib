#include <lua.h>
#include <luaclasslib.h>
#include <string.h>

// to suppress warnings
#define UNUSED(...) (void)(__VA_ARGS__)

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

static int classlib_uvget(lua_State *L) {
    int uv = 1;
    if (lua_gettop(L) == 3) {
        uv = luaL_checknumber(L, 2);
        lua_remove(L, 2);
    }
    luaC_uvget(L, 1, uv);
    return 1;
}

static int classlib_uvset(lua_State *L) {
    int uv = 1;
    if (lua_gettop(L) == 4) {
        uv = luaL_checknumber(L, 2);
        lua_remove(L, 2);
    }
    luaC_uvset(L, 1, uv);
    return 0;
}

static int classlib_rawget(lua_State *L) {
    luaC_rawget(L, 1);
    return 1;
}

static int classlib_rawset(lua_State *L) {
    luaC_rawset(L, 1);
    return 0;
}

static int classlib_type(lua_State *L) {
    luaL_checkany(L, 1);
    lua_pushstring(L, luaC_typename(L, 1));
    return 1;
}

int luaC_isobject(lua_State *L, int index) {
    int ret = 0;
    if (lua_istable(L, index) || lua_isuserdata(L, index)) {
        ret = lua_getfield(L, index, "__class") == LUA_TTABLE;
        lua_pop(L, 1);
    }
    return ret;
}

int luaC_isclass(lua_State *L, int index) {
    int ret = 0;
    if (lua_istable(L, index)) {
        ret = lua_getfield(L, index, "__base") == LUA_TTABLE;
        lua_pop(L, 1);
    }
    return ret;
}

int luaC_isinstance(lua_State *L, int index, const char *name) {
    int top = lua_gettop(L), ret = 0;
    if (luaC_getclass(L, index)) {
        do {
            luaC_getname(L, -1);
            ret = strcmp(name, lua_tostring(L, -1)) == 0;
            lua_pop(L, 1);  // pop name
        } while (!ret && luaC_getparent(L, -1));
    }
    lua_settop(L, top);
    return ret;
}

void *luaC_checkuclass(lua_State *L, int arg, const char *name) {
    if (!lua_isuserdata(L, arg) || !luaC_isinstance(L, arg, name))
        luaL_error(L, "Value is not an instance of class %s", name);
    return lua_touserdata(L, arg);
}

int luaC_pushclass(lua_State *L, const char *name) {
    // check the registry first
    if (luaC_getregfield(L, name) == LUA_TTABLE) return LUA_TTABLE;
    else lua_pop(L, 1);

    // otherwise, try to `require` the module
    lua_pushfstring(L, "return require('%s')", name);
    luaL_loadstring(L, lua_tostring(L, -1));
    lua_remove(L, -2);

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        lua_pop(L, 1);
        const char *pos = strrchr(name, '.');

        if (!pos || strlen(pos + 1) == 0) {
            lua_pushnil(L);
            return LUA_TNIL;
        }

        lua_pushstring(L, "return require('");
        lua_pushlstring(L, name, pos - name);
        lua_pushstring(L, "')");
        lua_concat(L, 3);
        luaL_loadstring(L, lua_tostring(L, -1));
        lua_remove(L, -2);

        if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
            lua_pop(L, 1);
            lua_pushnil(L);
            return LUA_TNIL;
        } else if (lua_istable(L, -1)) {
            lua_getfield(L, -1, pos + 1);
            lua_remove(L, -2);
        }
    }

    if (!luaC_isclass(L, -1)) {
        lua_pop(L, 1);
        lua_pushnil(L);
        return LUA_TNIL;
    }

    // add class to registry for quick access
    lua_pushvalue(L, -1);
    luaC_setregfield(L, name);

    return LUA_TTABLE;
}

luaC_Class *luaC_uclass(lua_State *L, int index) {
    luaC_Class *ret = NULL;
    if (luaC_isclass(L, index)) {
        lua_pushvalue(L, index);
        luaC_getreg(L);
        if (lua_isuserdata(L, -1)) ret = (luaC_Class *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    return ret;
}

// gets the first allocator up the inheritance heirarchy
static luaC_Constructor get_alloc(lua_State *L, int idx) {
    int              top = lua_gettop(L);
    luaC_Constructor ret = NULL;
    lua_pushvalue(L, idx);
    do {
        luaC_Class *class = luaC_uclass(L, -1);
        if (class && class->alloc) ret = class->alloc;
    } while (!ret && luaC_getparent(L, -1));
    lua_settop(L, top);
    return ret;
}

// default class __call
static int default_class_call(lua_State *L) {
    // create the object
    luaC_Constructor alloc = get_alloc(L, 1);
    if (alloc) {
        alloc(L);
        lua_newtable(L);
        lua_setiuservalue(L, -2, 1);
    } else lua_newtable(L);
    if (!luaC_getbase(L, 1)) return 0;
    lua_setmetatable(L, -2);            // set object metatable to class base
    lua_pushvalue(L, -1);               // push a copy of object for call
    lua_rotate(L, 2, 2);                // rotate objects before other args
    lua_getfield(L, 1, "__init");       // get init
    lua_insert(L, 3);                   // insert before args
    lua_call(L, lua_gettop(L) - 3, 0);  // call init
    return 1;
}

int luaC_construct(lua_State *L, int nargs, const char *name) {
    if (luaC_pushclass(L, name) == LUA_TTABLE) {
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
        lua_pushcfunction(L, classlib_rawset);
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

    luaC_getclass(L, index);  // push its class
    while (depth > 0) {       // walk the heirarchy
        if (!luaC_getparent(L, -1)) {
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

void luaC_super(lua_State *L, const char *name, int nargs, int nresults) {
    if (luaC_getparentfield(L, 1, 1, name) != LUA_TFUNCTION) {
        lua_pop(L, 1);
        return;
    }
    lua_pushvalue(L, 1);           // push obj
    lua_rotate(L, -nargs - 2, 2);  // put method and obj before args
    lua_call(L, nargs + 1, nresults);
}

// default class __init
static int default_init(lua_State *L) {
    UNUSED(L);
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
        luaC_rawget(L, 1);
    }
    return 1;
}

static int index_invalid(lua_State *L) {
    return luaL_error(
        L, "attempt to index an object that was already garbage collected");
}

static int default_udata_gc(lua_State *L) {
    if (lua_type(L, 1) == LUA_TUSERDATA && luaC_getclass(L, 1)) {
        // loop through the class and all its parents and call their finalizers
        do {
            luaC_Class *class = luaC_uclass(L, -1);
            if (class && class->gc) class->gc(L, lua_touserdata(L, 1));
        } while (luaC_getparent(L, -1));
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

static int default_class_inherited(lua_State *L) {
    // get derived class __call metamethod
    lua_getmetatable(L, 2);
    lua_pushstring(L, "__base");
    lua_rawget(L, 2);
    int base = lua_gettop(L), class_mt = base - 1;
    lua_pushstring(L, "__call");
    lua_rawget(L, class_mt);  // get derived constructor

    // if there's an allocator in the heirarcy and the derived class
    // __call metamethod is not a C function, then we have a Moonscript
    // class derived from a userdata class
    if (get_alloc(L, 1) && !lua_isnil(L, -1) && !lua_iscfunction(L, -1)) {
        // check if the parent is a userdata class with its lua
        // constructor disabled
        if (luaL_getmetafield(L, 1, "__call") == LUA_TNIL)
            return luaL_error(L, "Parent constructor is not accessible.");

        lua_pop(L, 2);  // pop constructor and parent constructor

        // set derived inheritance callback
        lua_pushstring(L, "__inherited");  // key for rawset
        lua_pushstring(L, "__inherited");  // key for rawget
        lua_rawget(L, 2);                  // get existing callback from derived
        lua_pushcclosure(L, default_class_inherited, 1);  // wrap it in closure
        lua_rawset(L, 2);

        // set new derived constructor
        lua_pushstring(L, "__call");
        lua_pushcfunction(L, default_class_call);
        lua_rawset(L, class_mt);

        // set derived class __index
        lua_pushstring(L, "__index");
        lua_pushvalue(L, base);  // copy of derived base for closure
        lua_pushcclosure(L, default_class_index, 1);
        lua_rawset(L, class_mt);

        // set derived instance __index
        lua_pushstring(L, "__index");
        lua_pushvalue(L, base);  // copy of derived base for closure
        lua_pushcclosure(L, default_udata_index, 1);
        lua_rawset(L, base);

        // set derived instance __newindex
        lua_pushstring(L, "__newindex");
        lua_pushcfunction(L, classlib_rawset);
        lua_rawset(L, base);

        // set __class metafield
        // NOTE: we really shouldn't have to do this, it should be done by
        // moonscript?
        lua_pushstring(L, "__class");
        lua_pushvalue(L, 2);
        lua_rawset(L, base);

        // set derived instance __gc
        lua_pushstring(L, "__gc");
        lua_pushcfunction(L, default_udata_gc);
        lua_rawset(L, base);
    }

    lua_settop(L, 2);  // clean up

    // call encapsulated __inherited method
    lua_pushvalue(L, lua_upvalueindex(1));
    if (lua_isfunction(L, -1)) {
        lua_insert(L, 1);
        lua_call(L, 2, LUA_MULTRET);
        return lua_gettop(L);
    }

    return 0;
}

int luaC_register(lua_State *L, int idx) {
    luaC_Class *c = lua_touserdata(L, idx);
    if (!c) return 0;
    if (c->module) lua_pushfstring(L, "%s.%s", c->module, c->name);
    else lua_pushstring(L, c->name);
    if (luaC_getreg(L) != LUA_TNIL) return 1;
    lua_pop(L, 1);

    int uclass = lua_absindex(L, idx);

    lua_newtable(L);                  // base table
    luaL_setfuncs(L, c->methods, 0);  // load in methods
    lua_newtable(L);                  // class table
    lua_newtable(L);                  // class metatable
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
    lua_pushstring(L, c->name);
    lua_setfield(L, class, "__name");  // set class __name
    lua_pushcfunction(L, default_class_inherited);
    lua_setfield(L, class, "__inherited");  // set class __inherited

    lua_pushvalue(L, class);
    lua_setfield(L, base, "__class");  // set base __class

    if (c->alloc) {
        lua_pushvalue(L, base);
        lua_pushcclosure(L, default_udata_index, 1);
        lua_setfield(L, base, "__index");  // set base __index
        lua_pushcfunction(L, classlib_rawset);
        lua_setfield(L, base, "__newindex");  // set base __newindex
        lua_pushcfunction(L, default_udata_gc);
        lua_setfield(L, base, "__gc");  // set base __gc
    } else {
        lua_pushvalue(L, base);
        lua_setfield(L, base, "__index");  // set base __index to self
    }

    // handle constructor
    if (c->user_ctor) {
        lua_pushcfunction(L, default_class_call);
        lua_setfield(L, class_mt, "__call");  // set meta __call
    }

    // handle inheritance
    if (c->parent == NULL) {                   // no parent
        lua_pushvalue(L, base);                // push base
        lua_setfield(L, class_mt, "__index");  // set meta __index to base
    } else if (luaC_pushclass(L, c->parent) == LUA_TTABLE) {  // get parent
        lua_pushvalue(L, base);                               // push base
        lua_pushcclosure(L, default_class_index, 1);  // wrap it in a closure
        lua_setfield(L, class_mt, "__index");         // set meta __index
        lua_getfield(L, -1, "__base");                // get parent __base
        lua_setmetatable(L, base);  // set base metatable to parent base
        lua_setfield(L, class, "__parent");  // set class __parent to parent
    } else {                                 // else parent not registered
        lua_pop(L, 4);                       // clean up and return
        lua_remove(L, uclass);
        return 0;
    }

    lua_setmetatable(L, class);  // set class metatable

    if (luaC_getparent(L, class)) {
        if (lua_getfield(L, -1, "__inherited") != LUA_TNIL) {
            lua_insert(L, -2);        // put inherited behind parent
            lua_pushvalue(L, class);  // push our (derived) class
            lua_call(L, 2, 0);        // call inherited
        } else lua_pop(L, 2);         // else pop nil and parent
    } else lua_pop(L, 1);             // else pop nil

    lua_pushvalue(L, class);
    lua_pushvalue(L, uclass);
    luaC_setreg(L);  // register uclass

    lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    if (c->module) {
        lua_pushfstring(L, "%s.%s", c->module, c->name);
        lua_pushvalue(L, -1);
        lua_pushvalue(L, class);
        luaC_setreg(L);  // reg[module.name] = class
        lua_pushvalue(L, class);
        lua_settable(L, -3);  // package.loaded[module.name] = class
        luaL_getsubtable(L, -1, c->module);
        lua_pushvalue(L, class);
        lua_setfield(L, -2, c->name);  // package.loaded.module[name] = class
        lua_pop(L, 2);                 // pop module table and package.loaded
    } else {
        lua_pushvalue(L, class);
        lua_setfield(L, -2, c->name);  // package.loaded[name] = class
        lua_pushvalue(L, class);
        luaC_setregfield(L, c->name);  // reg[name] = class
        lua_pop(L, 1);                 // pop package.loaded
    }

    lua_remove(L, base);    // remove base from stack
    lua_remove(L, uclass);  // remove uclass from stack
    return 1;
}

void luaC_unregister(lua_State *L, const char *name) {
    if (luaC_getregfield(L, name) == LUA_TTABLE) {
        lua_pushnil(L);
        luaC_setreg(L);  // remove C class if present
        lua_pushnil(L);
        luaC_setregfield(L, name);  // remove class table
        lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
        lua_pushnil(L);
        lua_setfield(L, -2, name);  // remove from package.loaded

        // check for module table
        const char *pos = strrchr(name, '.');
        if (pos && strlen(pos + 1) > 0) {
            lua_pushlstring(L, name, pos - name);
            lua_gettable(L, -2);  // get module table
            lua_pushnil(L);
            lua_setfield(L, -2, pos + 1);  // remove from module table
            lua_pop(L, 1);                 // pop module table
        }
    }
    lua_pop(L, 1);  // pop nil or package.loaded
}

void luaC_setinheritcb(lua_State *L, int idx, lua_CFunction cb) {
    if (luaC_isclass(L, idx)) {
        lua_pushstring(L, "__inherited");
        lua_pushcfunction(L, cb);
        lua_pushcclosure(L, default_class_inherited, 1);
        lua_rawset(L, idx);
    }
}

int luaC_newclass(
    lua_State  *L,
    const char *name,
    const char *parent,
    luaL_Reg   *methods) {
    luaC_Class *cls = (luaC_Class *)lua_newuserdatauv(L, sizeof(luaC_Class), 0);
    cls->name       = name;
    cls->parent     = parent;
    cls->user_ctor  = 1;
    cls->alloc      = NULL;
    cls->gc         = NULL;
    cls->methods    = methods;
    return luaC_register(L, -1);
}

int luaopen_lcl(lua_State *L) {
    static const luaL_Reg classlib_funcs[] = {
        {"uvget",  classlib_uvget },
        {"uvset",  classlib_uvset },
        {"rawget", classlib_rawget},
        {"rawset", classlib_rawset},
        {NULL,     NULL           }
    };
    luaL_newlib(L, classlib_funcs);
    return 1;
}

void luaC_overrideglobals(lua_State *L) {
    lua_pushcfunction(L, classlib_rawget);
    lua_setglobal(L, "rawget");
    lua_pushcfunction(L, classlib_rawset);
    lua_setglobal(L, "rawset");
    lua_pushcfunction(L, classlib_type);
    lua_setglobal(L, "type");
}
