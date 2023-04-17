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

.. literalinclude:: ../examples/basic/MyClass.moon
   :language: moonscript

Loading and running the file leaves a Moonscript class object on the stack, which
can then be registered with the library using `luaC_register`.

.. literalinclude:: ../examples/basic/main.c
   :lines: 7-17

Once registered, a class can be constructed by name using `luaC_construct`. Its methods can also be called
by name using `luaC_mcall` and `luaC_pmcall`.

.. literalinclude:: ../examples/basic/main.c
   :lines: 19-26
