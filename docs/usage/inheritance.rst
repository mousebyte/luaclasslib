Handling inheritance
====================

Inheritance in LCL is specified when registering a class, either with the ``parent`` parameter to `luaC_newclass`,
or with the `luaC_Class::parent` member. The parent class must be registered in order for inheritance to work properly.
When registering an existing class object, such as one loaded from Moonscript code, make sure any parent classes defined
in LCL are already registered. Parent classes defined in Moonscript will be registered automatically. Some useful utility
methods are provided for working with inheritance in C code.

.. note::

   Classes without a constructor cannot be inherited in Moonscript code. See `luaC_Class::user_ctor`.

* `luaC_getparent` will push a copy of a class's parent onto the stack.
* `luaC_super` calls the parent method with the given name. Useful for overriding methods in derived classes.
* `luaC_superinit` is a shorthand function to call the parent class's init method.
* `luaC_packageadd` loads a class as a Lua module, allowing it to be imported with a require statement.
* `luaC_setinheritcb` sets the inheritance callback of a class.
