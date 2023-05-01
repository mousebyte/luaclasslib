Method injection
================

Method injection lets you replace an existing method of a class with a new method, containing the
original method as an `upvalue <http://www.lua.org/manual/5.4/manual.html#4.2>`_. This can be used
to change the behavior of a class method or to alter the ``__index`` or ``__newindex`` metamethods
of a class to provide extra functionality.

The following example is taken from the `unit tests <https://github.com/mousebyte/LuaClassLib/tree/main/tests>`_.
Suppose we want to provide an object with properties that have custom getters and setters. We'll call our example
property ``n``. When we access ``n``, such as in ``i = obj.n``, we want to return the value of a getter method, ``get_n``.
When we set ``n``, we want to call a setter method, ``set_n``. The getter and setter methods are shown below.

.. literalinclude:: ../../tests/methodinjection.cpp
   :language: LCL
   :lines: 34-44

As you can see, the getter method simply returns our actual value, stored in ``x``. The setter sets ``x``
to double the provided value.

We can add a properties table to our class, which will contain subtables for each property. Each subtable can contain a
get method and a set method. Assuming our class is at the top of the stack:

.. literalinclude:: ../../tests/methodinjection.cpp
   :language: LCL
   :lines: 55-62
   :dedent: 4

Now we must create modified ``__index`` and ``__newindex`` metamethods that check our properties table before deferring to
regular access. Note the usage of the special helper functions `luaC_deferindex` and `luaC_defernewindex`, which call
the ``__index`` or ``__newindex`` function stored in the upvalue, respectively.

.. literalinclude:: ../../tests/methodinjection.cpp
   :language: LCL
   :lines: 3-32
   :emphasize-lines: 12,28

To override the ``__index`` and ``__newindex`` metamethods on our class, we call the helper methods, `luaC_injectindex` and
`luaC_injectnewindex`.

.. literalinclude:: ../../tests/methodinjection.cpp
   :language: LCL
   :lines: 64,66
   :dedent: 4

And that's it! Accessing ``n`` will now defer to our accessor methods in our property table, while accessing any field that
doesn't have an entry in our property table will just go through our usual metamethods. Note that any method can be overridden
with method injection, not just ``__index`` and ``__newindex`` (see `luaC_injectmethod`).
