Creating a userdata class
=========================

LCL enables the creation of Lua classes for userdata objects. Consider the following structure representing a file handle:

.. literalinclude:: ../../tests/classes/file.c
   :language: LCL
   :lines: 5-8

In order to create a Lua class for :code:`file_t` objects, we must provide an allocator function and a garbage collector function.
The allocator function should push onto the stack a userdata object with at least one user value. The garbage collector function
should perform any necessary cleanup of resources used by the userdata object.

.. warning::

   The garbage collector function should not free the userdata itself. This is done by the Lua garbage collector.

.. literalinclude:: ../../tests/classes/file.c
   :language: LCL
   :lines: 10-29

The methods for the class should be defined just like the previous example. The function `luaC_checkuclass`
can be used to check if an object on the Lua stack is an instance of a class, and provide a pointer to the userdata if it is.

.. literalinclude:: ../../tests/classes/file.c
   :language: LCL
   :lines: 31-61
   :emphasize-lines: 4,16,23

Put the methods in a `luaL_Reg <http://www.lua.org/manual/5.4/manual.html#luaL_Reg>`_ and then throw everything into a `luaC_Class`.

.. literalinclude:: ../../tests/classes/file.c
   :language: LCL
   :lines: 63-76

To register the class, push the `luaC_Class` as a light userdata and call `luaC_register`.

.. sourcecode:: LCL

   lua_pushlightuserdata(L, &file_class);
   luaC_register(L, -1);

The class can now be constructed with a call to `luaC_construct`:

.. sourcecode:: LCL

   lua_pushstring(L, "myfile.txt");
   luaC_construct(L, 1, "File");

Since we set `luaC_Class::user_ctor` to ``1``, our class can be constructed from Lua code by calling the class object,
assuming it is made accessible (e.g. by a call to `lua_register <http://www.lua.org/manual/5.4/manual.html#lua_register>`_).
