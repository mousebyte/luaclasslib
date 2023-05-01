Basic Usage
===========

Consider the following Moonscript class:

.. literalinclude:: MyClass.moon
   :language: moonscript


Loading and running the file leaves a Moonscript class object on the stack, which
can then be registered with the library using `luaC_register`.

.. literalinclude:: basic.c
   :language: LCL
   :lines: 7-17
   :dedent: 4


Once registered, a class can be constructed by name using `luaC_construct`. Its methods can also be called
by name using `luaC_mcall` and `luaC_pmcall`.

.. literalinclude:: basic.c
   :language: LCL
   :lines: 19-28
   :dedent: 4

