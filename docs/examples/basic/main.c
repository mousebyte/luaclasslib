#include <assert.h>
#include <luaclasslib.h>
#include <lualib.h>
#include <moonauxlib.h>

int main() {
    lua_State *L = luaL_newstate();

    // need to open Lua libraries before we use moonauxlib.h
    luaL_openlibs(L);

    moonL_dofile(L, "../basic/MyClass.moon");
    luaC_register(L, -1);

    // leaves a copy of the class on the stack which can be modified or removed
    assert(luaC_isclass(L, -1));
    lua_pop(L, 1);

    lua_pushnumber(L, 6);
    luaC_construct(L, 1, "MyClass");  // leaves a MyClass object on the stack
    assert(luaC_isobject(L, -1));
    assert(luaC_isinstance(L, -1, "MyClass"));

    lua_pushnumber(L, 1);
    luaC_mcall(L, "add", 1, 1);
    moonL_print(L, -1);  // prints "There are now 7.0 mice!"

    lua_close(L);

    return 0;
}
