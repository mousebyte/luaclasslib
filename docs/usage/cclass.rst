Creating a class from C code
============================

To create a Lua class from a set of C functions, first put them in a
`luaL_Reg <http://www.lua.org/manual/5.4/manual.html#luaL_Reg>`_. In the case
of simple classes (non-userdata), you can use the `luaC_newclass` helper method to
define the `luaC_Class`.

.. literalinclude:: cclass.c
   :language: LCL
