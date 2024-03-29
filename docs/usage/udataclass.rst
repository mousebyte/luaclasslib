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

To create the class object, push the `luaC_Class` as a light userdata and call `luaC_classfromptr`. The object can then either be
manipulated directly, or added to the `package.loaded <http://www.lua.org/manual/5.4/manual.html#pdf-package.loaded>`_ table where it
can be found by LCL.

.. sourcecode:: LCL

   // create a class
   lua_pushlightuserdata(L, &file_class);
   luaC_classfromptr(L);

   // put it in `package.loaded`
   luaC_setpackageloaded(L, "lcltests.File");

The class can now be constructed with a call to `luaC_construct`:

.. sourcecode:: LCL

   lua_pushstring(L, "myfile.txt");
   luaC_construct(L, 1, "lcltests.File");

Since we set `luaC_Class::user_ctor` to ``1``, our class can be constructed from Lua code by calling the class object.
