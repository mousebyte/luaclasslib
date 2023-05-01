Handling inheritance
====================

Inheritance in LCL is specified when registering a class, either with the ``parent`` parameter to `luaC_newclass`,
or with the `luaC_Class::parent` member. The parent class must be registered in order for inheritance to work properly.
When registering an existing class object, such as one loaded from Moonscript code, the parent classes are registered
automatically. Some useful utility methods are provided for working with derived classes in C code.

.. attention::

   Userdata classes cannot inherit from classes definied in Moonscript. This is an edge case and is not planned to be
   supported in the future.

* `luaC_getparent` will push a copy of a class's parent onto the stack.
* `luaC_super` calls the parent method with the given name. Useful for overriding methods in derived classes.
* `luaC_superinit` is a shorthand function to call the parent class's init method.

