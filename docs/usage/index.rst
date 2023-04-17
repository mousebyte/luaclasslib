Usage
=====

This section shows the basic usage of the library.
The examples in this section assume a basic understanding of the
`Lua C API <http://www.lua.org/manual/5.4/manual.html#4>`_.

.. note::

   For more examples, check the `unit tests <https://github.com/mousebyte/LuaClassLib/tree/main/tests>`_.

.. toctree::
   :hidden:

   cclass
   udataclass
   inheritance
   methodinjection

Basic Usage
-----------

Consider the following Moonscript class:

.. literalinclude:: Example.moon
   :language: moonscript

Loading and running the file leaves a Moonscript class object on the stack, which
can then be registered with the library using `luaC_register`.

.. sourcecode:: c

   lua_State *L = luaL_newstate();
   moonL_dofile(L, "Example.moon");
   luaC_register(L, -1);

   // leaves a copy of the class on the stack
   // which can be modified or removed
   assert(luaC_isclass(L, -1));
   lua_pop(L, 1);

Once registered, a class can be constructed by name using `luaC_construct`. Its methods can also be called
by name using `luaC_mcall` and `luaC_pmcall`.

.. sourcecode:: c

   lua_pushnumber(L, 6);
   luaC_construct(L, 1, "Example"); // leaves an Example object on the stack
   assert(luaC_isobject(L, -1));
   assert(luaC_isinstance(L, -1, "Example"));

   lua_pushnumber(L, 1);
   luaC_mcall(L, "add", 1, 1); // return value: "There are now 7.0 mice!"
