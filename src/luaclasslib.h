/// @file luaclasslib.h

#ifndef LUACLASSLIB_H
#define LUACLASSLIB_H

#include <lauxlib.h>
#include <lua.h>

/**
 * @brief A user data class constructor. Implementations of this function should
 * push one value onto the stack, a userdata with at least one user value.
 *
 * @param L The Lua state.
 */
typedef void (*luaC_Constructor)(lua_State *L);

/**
 * @brief A user data class destructor. Implementations of this function should
 * perform any necessary resource cleanup for the user data object represented
 * by `p`. Do not free p, that is handled by the Lua garbage collector.
 *
 * @param L The Lua state.
 * @param p A pointer to the user data to destruct.
 */
typedef void (*luaC_Destructor)(lua_State *L, void *p);

/// Header for luaC_Class objects.
#define LUAC_CLASS_HEADER                \
    /** The name of the class. */        \
    const char      *name;               \
    /** The name of the parent. */       \
    const char      *parent;             \
    /** Whether to allow construction */ \
    /** by calling the class object. */  \
    int              user_ctor;          \
    /** The class allocator. */          \
    luaC_Constructor alloc;              \
    /** The class garbage collector. */  \
    luaC_Destructor  gc;                 \
    /** The class methods. */            \
    luaL_Reg        *methods;

/// Contains information about a user data class.
typedef struct {
    LUAC_CLASS_HEADER
} luaC_Class;

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * value at the top of the stack. The access is raw (does not use the `__index`
 * metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvrawget(lua_State *L, int idx, int uv) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);        // put uv behind key
        ret = lua_rawget(L, -2);  // get the value
    } else lua_pushnil(L);        // otherwise push nil
    lua_remove(L, -2);            // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the value just below the top. The access is raw
 * (does not use the `__index` metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvrawset(lua_State *L, int idx, int uv) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -3);  // put uv before key and value
        lua_rawset(L, -3);  // set the value
        lua_pop(L, 1);      // pop the uv
        return 1;
    }
    lua_pop(L, 3);  // otherwise pop uv, key, and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * pointer `p` represented as a light userdata. The access is raw (does not use
 * the `__index` metavalue).
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param p The pointer to use as a key.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvrawgetp(lua_State *L, int idx, int uv, const void *p) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        ret = lua_rawgetp(L, -1, p);  // get the value
    } else lua_pushnil(L);            // otherwise push nil
    lua_remove(L, -2);                // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the pointer `p` represented as a light userdata.
 * The access is raw (does not use the `__index` metavalue).
 *
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param p The pointer to use as a key.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvrawsetp(lua_State *L, int idx, int uv, const void *p) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);      // put uv before value
        lua_rawsetp(L, -2, p);  // set the value
        lua_pop(L, 1);          // pop the uv
        return 1;
    }
    lua_pop(L, 2);  // otherwise pop uv and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index, and `k` is the
 * value at the top of the stack.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return The type of the pushed value.
 */
static inline int luaC_uvget(lua_State *L, int idx, int uv) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);          // put uv behind key
        ret = lua_gettable(L, -2);  // get the value
    } else lua_pushnil(L);          // otherwise push nil
    lua_remove(L, -2);              // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, `v` is the value on
 * top of the stack, and `k` is the value just below the top.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int luaC_uvset(lua_State *L, int idx, int uv) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -3);    // put uv before key and value
        lua_settable(L, -3);  // set the value
        lua_pop(L, 1);        // pop the uv
        return 1;
    }
    lua_pop(L, 3);  // otherwise pop uv, key, and value
    return 0;
}

/**
 * @brief Pushes onto the stack the value `t[k]` where `t` is the table stored
 * in the given user value of the userdata at the given index.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param k The name of the field.
 *
 * @return The type of the pushed value.
 */
static inline int
luaC_getuvfield(lua_State *L, int idx, int uv, const char *k) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    int ret = LUA_TNIL;
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        ret = lua_getfield(L, -1, k);  // get value if uv is table
    } else lua_pushnil(L);             // otherwise push nil
    lua_remove(L, -2);                 // remove the uv
    return ret;
}

/**
 * @brief Does the equivalent of `t[k] = v`, where `t` is the table stored in
 * the given user value of the userdata at the given index, and `v` is the value
 * on top of the stack.
 *
 * @param L The Lua state.
 * @param idx The index of the userdata.
 * @param uv The user value to access.
 * @param k The name of the field.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
static inline int
luaC_setuvfield(lua_State *L, int idx, int uv, const char *k) {
    if (!lua_isuserdata(L, idx))
        return luaL_error(L, "Object at index %d is not a userdata.", idx);
    if (lua_getiuservalue(L, idx, uv) == LUA_TTABLE) {
        lua_insert(L, -2);       // put uv behind value
        lua_setfield(L, -2, k);  // set the value
        lua_pop(L, 1);           // pop the uv
        return 1;
    }
    lua_pop(L, 2);  // otherwise pop uv and value
    return 0;
}

/**
 * @brief Improved rawget. Works with user data classes.
 *
 * @param L The Lua state.
 * @param idx The index of the object on the stack.
 *
 * @return The type of the value pushed onto the stack.
 */
static inline int luaC_rawget(lua_State *L, int idx) {
    if (lua_istable(L, idx)) return lua_rawget(L, idx);
    return luaC_uvrawget(L, idx, 1);
}

/**
 * @brief Improved rawset. Works with user data classes.
 *
 * @param L The Lua state.
 * @param idx The index of the object on the stack.
 */
static inline void luaC_rawset(lua_State *L, int idx) {
    if (lua_istable(L, idx)) lua_rawset(L, idx);
    else luaC_uvrawset(L, idx, 1);
}

/**
 * @brief Call a method of an object, passing the object as the first argument.
 *
 * @param L The Lua state.
 * @param method The name of the method to call.
 * @param nargs The number of arguments.
 * @param nresults The number of results.
 */
static inline void
luaC_mcall(lua_State *L, const char *method, int nargs, int nresults) {
    lua_getfield(L, -nargs - 1, method);  // get the method
    lua_pushvalue(L, -nargs - 2);         // push a copy of the object
    lua_rotate(L, -nargs - 2, 2);         // rotate args to top
    lua_call(L, nargs + 1, nresults);
}

/**
 * @brief Call a method of an object in protected mode, passing the object as
 * the first argument.
 *
 * @param L The Lua state.
 * @param method The name of the method to call.
 * @param nargs The number of arguments.
 * @param nresults The number of results.
 * @param msgh The stack index of the message handler, or 0 if none is to be
 * used.
 *
 * @return The pcall status code.
 */
static inline int luaC_pmcall(
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

/**
 * @brief Checks if the value at the given index is an instance of a class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is an instance of a class, and 0 otherwise.
 */
int luaC_isobject(lua_State *L, int index);

/**
 * @brief Checks if the value at the given index is a class.
 *
 * @param L The Lua state.
 * @param index The stack index to check.
 *
 * @return 1 if the value is a class, and 0 otherwise.
 */
int luaC_isclass(lua_State *L, int index);

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
int luaC_isinstance(lua_State *L, int arg, const char *name);

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
void *luaC_checkuclass(lua_State *L, int arg, const char *name);

/**
 * @brief Pushes onto the stack the class registered under the given *name*.
 *
 * @param L The Lua state.
 * @param name The fully qualified (with module prefix) class name.
 *
 * @return The type of the pushed value.
 */
int luaC_pushclass(lua_State *L, const char *name);

/**
 * @brief Returns a pointer to the user data class associated with the class at
 * the given stack index.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return A pointer to the user data class, or NULL if none was found.
 */
luaC_Class *luaC_uclass(lua_State *L, int index);

/**
 * @brief Construct an instance of a class.
 *
 * @param L The Lua state.
 * @param nargs The number of arguments on the stack to pass to the constructor.
 * @param name The name of the class.
 *
 * @return 1 if the object was successfully constructed, and 0 otherwise.
 */
int luaC_construct(lua_State *L, int nargs, const char *name);

/**
 * @brief Replaces a class method with a closure of the given C function *f*,
 * with the previous method as its only upvalue.
 *
 * @param L The Lua state.
 * @param index The index of the class object.
 * @param method The method to replace.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
int luaC_injectmethod(
    lua_State    *L,
    int           index,
    const char   *method,
    lua_CFunction f);

/**
 * @brief When called from an injected index function, calls (or indexes) the
 * original index and pushes the result onto the stack.
 *
 * @param L The Lua state.
 *
 * @return The type of the value pushed onto the stack.
 */
int luaC_deferindex(lua_State *L);

/**
 * @brief When called from an injected newindex function, calls the original
 * newindex if it exists.
 *
 * @param L The Lua state.
 */
void luaC_defernewindex(lua_State *L);

/**
 * @brief Pushes onto the stack the value of a field at the given depth up the
 * heirarchy. If *depth* is greater than the number of parents above the target
 * object, pushes nil.
 *
 * @param L The Lua state.
 * @param index The index of the object in the stack.
 * @param depth The parent depth.
 * @param name The name of the field.
 *
 * @return The type of the value pushed onto the stack.
 */
int luaC_getparentfield(lua_State *L, int index, int depth, const char *name);

/**
 * @brief Calls a parent class method, passing the given number of arguments
 * from the top of the stack. Leaves the stack in its previous state. Should
 * only be used in C class methods, in which the first stack index is the object
 * on which the method was invoked.
 *
 * @param L The Lua state.
 * @param name The name of the method.
 * @param nargs The number of arguments to pass.
 * @param nresults The number of results to return.
 */
void luaC_super(lua_State *L, const char *name, int nargs, int nresults);

/**
 * @brief Adds the class represented by the user data at the given stack index
 * to the class registry. The class will also be added to *package.loaded*.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise.
 */
int luaC_register(lua_State *L, int index);

/**
 * @brief Removes the class with the given name from the class registry.
 *
 * @param L The Lua state.
 * @param name The name of the class to unregister.
 */
void luaC_unregister(lua_State *L, const char *name);

/**
 * @brief Sets the __inherited callback on a class. When the class is
 * derived from, the function wll be called with the parent class
 * and derived class as its arguments.
 *
 * @param L The Lua state.
 * @param index The index of the class.
 * @param cb The callback function.
 */
void luaC_setinheritcb(lua_State *L, int index, lua_CFunction cb);

/**
 * @brief Helper method for creating and registering a simple luaC_Class as a
 * full userdata. Useful for when you're using stock classes and don't want to
 * define your luaC_Class with static linkage.
 *
 * @param L The Lua state.
 * @param name The class name.
 * @param module The module to add the class to. Can be null.
 * @param parent The parent class name. Must be in the registry. Can be null.
 * @param methods The class methods.
 *
 * @return 1 if the class was successfully created and registered, and 0
 * otherwise.
 */
int luaC_newclass(
    lua_State  *L,
    const char *name,
    const char *parent,
    luaL_Reg   *methods);

/**
 * @brief Pushes the Lua class library onto the stack.
 *
 * @param L The Lua state.
 */
int luaopen_lcl(lua_State *L);

/**
 * @brief Overrides the `rawget`, `rawset`, and `type` functions with additional
 * functionality.
 * @see @rstref{luaC_rawget}, @rstref{luaC_rawset}, and @rstref{luaC_typename}
 *
 * @param L The Lua state.
 */
void luaC_overrideglobals(lua_State *L);

/**
 * @brief Calls the init function of the parent class.
 *
 * @param L The Lua state.
 */
#define luaC_superinit(L) luaC_super((L), "__init", lua_gettop(L) - 1, 0);

/**
 * @brief Replaces the index method of a class with a closure of the given C
 * function *f*, with the previous index as its only upvalue.
 *
 * @param L The Lua state.
 * @param i The index of the class object.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
#define luaC_injectindex(L, i, f) luaC_injectmethod((L), (i), "__index", (f))

/**
 * @brief Replaces the newindex method of a class with a closure of the given C
 * function *f*, with the previous newindex as its only upvalue.
 *
 * @param L The Lua state.
 * @param i The index of the class object.
 * @param f The C function to replace the method with.
 *
 * @return 1 if the operation was successful, and 0 otherwise.
 */
#define luaC_injectnewindex(L, i, f) \
    luaC_injectmethod((L), (i), "__newindex", (f))

/**
 * @brief Pushes onto the stack the class table of the object at the given stack
 * index.
 *
 * @param L The Lua state.
 * @param index The index of the object on the stack.
 *
 * @return 1 if the class was fetched successfully, and 0 otherwise.
 */
#define luaC_getclass(L, index) \
    (lua_getfield((L), (index), "__class") == LUA_TTABLE)

/**
 * @brief Pushes onto the stack the base table of the class at the given stack
 * index.
 *
 * @param L The Lua state.
 * @param index The index of the class on the stack.
 *
 * @return 1 if the base was fetched successfully, and 0 otherwise.
 */
#define luaC_getbase(L, index) \
    (lua_getfield((L), (index), "__base") == LUA_TTABLE)

/**
 * @brief Pushes onto the stack the parent class table of the class at the given
 * stack index.
 *
 * @param L The Lua state.
 * @param index The index of the class on the stack.
 *
 * @return 1 if the parent was fetched successfully, and 0 otherwise.
 */
#define luaC_getparent(L, index) \
    (lua_getfield((L), (index), "__parent") == LUA_TTABLE)

/**
 * @brief Pushes onto the stack the name of the class at the given stack index.
 *
 * @param L The Lua state.
 * @param index The index of the class on the stack.
 *
 * @return 1 if the parent was fetched successfully, and 0 otherwise.
 */
#define luaC_getname(L, index) \
    (lua_getfield((L), (index), "__name") == LUA_TSTRING)

/**
 * @brief Improved typename function.
 *
 * @param L The Lua state.
 * @param idx The index of the object on the stack.
 *
 * @return If the object belongs to a class, returns the classname. Otherwise,
 * returns the regular typename.
 */
static inline const char *luaC_typename(lua_State *L, int idx) {
    int         top  = lua_gettop(L);
    int         type = lua_type(L, idx);
    const char *name;

    switch (type) {
        case LUA_TTABLE:
            if (luaC_getbase(L, idx)) {
                name = "class";
                break;
            } else lua_pop(L, 1);
        case LUA_TUSERDATA:
            if (luaC_getclass(L, idx) && luaC_getname(L, -1)) {
                name = lua_tostring(L, -1);
                break;
            }
        default:
            name = lua_typename(L, type);
    }

    lua_settop(L, top);
    return name;
}

#endif
