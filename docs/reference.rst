*********
Reference
*********

LuaClassLib
===========
Contents of the header file ``luaclasslib.h``.

Class Registration
------------------
Functions supporting the creation and registration of Lua classes.

.. doxygenstruct:: luaC_Class
   :project: LuaClassLib
   :members:

.. doxygenfunction:: luaC_classfromptr
   :project: LuaClassLib

.. doxygenfunction:: luaC_unregister
   :project: LuaClassLib

.. doxygenfunction:: luaC_setinheritcb
   :project: LuaClassLib

.. doxygenfunction:: luaC_newclass
   :project: LuaClassLib

.. doxygentypedef:: luaC_Constructor
   :project: LuaClassLib

.. doxygentypedef:: luaC_Destructor
   :project: LuaClassLib

.. doxygendefine:: LUAC_CLASS_HEADER
   :project: LuaClassLib

Utility
-------
Utility functions for Lua classes and objects.

.. doxygenfunction:: luaC_construct
   :project: LuaClassLib

.. doxygenfunction:: luaC_super
   :project: LuaClassLib

.. doxygendefine:: luaC_superinit
   :project: LuaClassLib

.. doxygenfunction:: luaC_getparentfield
   :project: LuaClassLib

.. doxygenfunction:: luaC_mcall
   :project: LuaClassLib

.. doxygenfunction:: luaC_pmcall
   :project: LuaClassLib

.. doxygenfunction:: luaC_pushclass
   :project: LuaClassLib

.. doxygenfunction:: luaC_uclass
   :project: LuaClassLib

.. doxygendefine:: luaC_getclass
   :project: LuaClassLib

.. doxygendefine:: luaC_getbase
   :project: LuaClassLib

.. doxygendefine:: luaC_getparent
   :project: LuaClassLib

.. doxygendefine:: luaC_getname
   :project: LuaClassLib

.. doxygenfunction:: luaC_typename
   :project: LuaClassLib

.. doxygenfunction:: luaC_rawget
   :project: LuaClassLib

.. doxygenfunction:: luaC_rawset
   :project: LuaClassLib

Introspection
-------------
Functions providing introspection into Lua classes and objects.

.. doxygenfunction:: luaC_isclass
   :project: LuaClassLib

.. doxygenfunction:: luaC_isobject
   :project: LuaClassLib

.. doxygenfunction:: luaC_isinstance
   :project: LuaClassLib

.. doxygenfunction:: luaC_checkuclass
   :project: LuaClassLib

Method Injection
----------------
Functions for overriding class methods.

.. doxygenfunction:: luaC_injectmethod
   :project: LuaClassLib

.. doxygendefine:: luaC_injectindex
   :project: LuaClassLib

.. doxygendefine:: luaC_injectnewindex
   :project: LuaClassLib

.. doxygenfunction:: luaC_deferindex
   :project: LuaClassLib

.. doxygenfunction:: luaC_defernewindex
   :project: LuaClassLib

User Value Access
-----------------
Functions allowing access to tables stored in the user values of a userdata.

.. doxygenfunction:: luaC_uvget
   :project: LuaClassLib

.. doxygenfunction:: luaC_uvset
   :project: LuaClassLib

.. doxygenfunction:: luaC_getuvfield
   :project: LuaClassLib

.. doxygenfunction:: luaC_setuvfield
   :project: LuaClassLib

.. doxygenfunction:: luaC_uvrawget
   :project: LuaClassLib

.. doxygenfunction:: luaC_uvrawset
   :project: LuaClassLib

.. doxygenfunction:: luaC_uvrawgetp
   :project: LuaClassLib

.. doxygenfunction:: luaC_uvrawsetp
   :project: LuaClassLib

MoonAuxLib
==========
Contents of the header file ``moonauxlib.h``.

Compiler API
------------
Functions from the `Moonscript Compiler API <https://moonscript.org/reference/api.html>`_.

.. doxygenfunction:: moonL_loadstring
   :project: LuaClassLib

.. doxygenfunction:: moonL_loadfile
   :project: LuaClassLib

.. doxygenfunction:: moonL_dofile
   :project: LuaClassLib

Moonscript Library
------------------
Functions from the `Moonscript Standard Library <https://moonscript.org/reference/standard_lib.html>`_.

.. doxygenfunction:: moonL_print
   :project: LuaClassLib

Lua Library
===========
Functions provided by LCL to Lua code.

.. doxygenfunction:: luaC_overrideglobals
   :project: LuaClassLib

.. lua:module:: lcl

.. lua:function:: uvget(obj[, uv], idx)

   Gets the value of ``t[idx]``, where ``t`` is the table stored
   in the user value ``uv`` of ``obj``. If no user value is specified, uses the
   first user value. Uses the ``__index`` metamethod.

   :param obj: The object.
   :param uv: ``[optional]`` The user value to access.
   :param idx: The index to get.

.. lua:function:: uvset(obj[, uv], idx, value)

   Sets the value of ``t[idx]`` to ``value``, where ``t`` is the table stored
   in the user value ``uv`` of ``obj``. If no user value is specified, uses the
   first user value. Uses the ``__newindex`` metamethod.

   :param obj: The object.
   :param uv: ``[optional]`` The user value to access.
   :param idx: The index to set.
   :param value: The value.

.. lua:function:: rawget(obj, idx)

   If ``obj`` is a table, gets the value of ``obj[idx]``. If ``obj`` is a
   userdata, gets the value of ``t[idx]``, where ``t`` is the table stored in the
   first user value of ``obj``. Does not use the ``__index`` metamethod.

   Replaces the default version of this function when
   `luaC_overrideglobals` is called.

   :param obj: The object.
   :param idx: The index to get.

.. lua:function:: rawset(obj, idx, value)

   If ``obj`` is a table, sets the value of ``obj[idx]`` to ``value``. If ``obj``
   is a userdata, sets the value of ``t[idx]`` to ``value``, where ``t`` is the table
   stored in the first user value of ``obj``. Does not use the ``__newindex``
   metamethod.
   
   Replaces the default version of this function when
   `luaC_overrideglobals` is called.

   :param obj: The object.
   :param idx: The index to set.
   :param value: The value.

.. lua:function:: type(obj)

   If ``obj`` is an instance of a named class, returns the name of the
   class it belongs to. If ``obj`` is a class, returns "class". Otherwise, returns
   the standard Lua typename.
   
   Replaces the default version of this function when
   `luaC_overrideglobals` is called.

   :param obj: The object.
