/// @file moonaux.h

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
 * @param p A pointer to the user data to destruct.
 */
typedef void (*luaC_Destructor)(void *p);

/// Header for luaC_Class objects.
#define LUAC_CLASS_HEADER              \
    /** The name of the class. */      \
    const char      *name;             \
    /** The class allocator */         \
    luaC_Constructor alloc;            \
    /** The class garbage collector */ \
    luaC_Destructor  gc;

/// Contains information about a user data class.
typedef struct {
    LUAC_CLASS_HEADER
} luaC_Class;

int moonL_dofile(lua_State *L, const char *file);

void moonL_print(lua_State *L, int index);

/**
 * @brief Call a method of an object, passing the object as the first argument.
 *
 * @param L The Lua state.
 * @param method The name of the method to call.
 * @param nargs The number of arguments.
 * @param nresults The number of results.
 */
void luaC_mcall(lua_State *L, const char *method, int nargs, int nresults);

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
int luaC_pmcall(
    lua_State  *L,
    const char *method,
    int         nargs,
    int         nresults,
    int         msgh);

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
 * @param name The class name.
 *
 * @return The type of the pushed value.
 */
int luaC_getclass(lua_State *L, const char *name);

/**
 * @brief Gets a pointer to the user data class associated with the class at the
 * given stack index.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return A pointer to the user data class, or NULL if none was found.
 */
luaC_Class *luaC_getuclass(lua_State *L, int index);

/**
 * @brief Adds the class at the given stack index to the moonlib
 * registry, along with its parents, if not present already.
 *
 * @param L The Lua state.
 * @param index The stack index of the class.
 *
 * @return 1 if the class was successfully registered, and 0 otherwise.
 */
int luaC_registerclass(lua_State *L, int index);

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
 * @brief Calls a parent class method, passing all values on the stack as
 * arguments. Leaves the stack in its previous state. Should only be used in C
 * class methods, in which the first stack index is the object on which the
 * method was invoked.
 *
 * @param L The Lua state.
 * @param name The name of the method.
 * @param nresults The number of results to return.
 */
void luaC_super(lua_State *L, const char *name, int nresults);

/**
 * @brief Creates and registers a new class.
 *
 * @param L The Lua state.
 * @param name The class name.
 * @param parent The parent class name. Must be in the registry.
 * @param methods The class methods.
 * @param uclass The uclass, if creating a user data class.
 * @param userCtor Whether to enable object construction by calling the class
 * table.
 *
 * @return 1 if the class was successfully created and registered, and 0
 * otherwise.
 */
int luaC_newuclass(
    lua_State      *L,
    const char     *name,
    const char     *parent,
    const luaL_Reg *methods,
    luaC_Class     *uclass,
    int             userCtor);

void luaopen_moonaux(lua_State *);

/**
 * @brief Calls the init function of the parent class.
 *
 * @param L The Lua state.
 */
#define luaC_superinit(L) luaC_super((L), "__init", 0);

/**
 * @brief Creates and registers a new Lua class.
 *
 * @param L The Lua state.
 * @param name The class name.
 * @param parent The parent class name. Must be in the registry.
 * @param methods The class methods.
 *
 * @return 1 if the class was successfully created and registered, and 0
 * otherwise.
 */
#define luaC_newclass(L, name, parent, methods) \
    luaC_newuclass((L), (name), (parent), (methods), NULL, 1);

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
 * @brief Pushes the class of the object at the given index onto the stack.
 *
 * @param L The Lua state.
 * @param index The index of the object on the stack.
 */
#define luaC_pushclass(L, index) lua_getfield((L), (index), "__class")
