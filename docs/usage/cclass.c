#include <luaclasslib.h>
#include <lualib.h>
#include <moonauxlib.h>

int myclass_init(lua_State *L) {
    // first argument is the object being initialized
    // check if second argument is a number!
    luaL_checknumber(L, 2);
    lua_pushvalue(L, 2);
    lua_setfield(L, 1, "x");
    return 0;
}

int myclass_foo(lua_State *L) {
    // again, first argument is the object
    lua_getfield(L, 1, "x");
    int x = luaL_checknumber(L, -1);
    printf("In foo! x is %d\n", x);
    return 0;
}

luaL_Reg myclass_methods[] = {
    {"new", myclass_init}, // "new" indicates a constructor
    {"foo", myclass_foo },
    {NULL,  NULL        }
};

int main(void) {
    lua_State *L = luaL_newstate();

    luaC_newclass(
        L,
        "MyClass",         // class name
        NULL,              // parent name
        myclass_methods);  // methods

    // leaves a copy of the class on the stack which can be modified and/or
    // loaded as a package
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_insert(L, -2);
    lua_setfield(L, -2, "MyClass");
    lua_pop(L, 1);

    lua_pushnumber(L, 12);
    luaC_construct(L, 1, "MyClass");

    // call the "foo" method with 0 args and 0 results
    luaC_mcall(L, "foo", 0, 0);  // prints "In foo! x is 12"

    lua_close(L);
    return 0;
}
