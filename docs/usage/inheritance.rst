Handling inheritance
====================

Inheritance in LCL is specified when registering a class, either with the ``parent`` parameter to `luaC_newclass`,
or with the `luaC_Class::parent` member. When specifying a parent class, you should use the full module path, as you
would with a require statement in Lua. Some useful utility methods are provided for working with inheritance in C code.

.. note::

   Classes without a constructor cannot be inherited in Moonscript code. See `luaC_Class::user_ctor`.

* `luaC_getparent` will push a copy of a class's parent onto the stack.
* `luaC_super` calls the parent method with the given name. Useful for overriding methods in derived classes.
* `luaC_superinit` is a shorthand function to call the parent class's init method.
* `luaC_setinheritcb` sets the inheritance callback of a class.
