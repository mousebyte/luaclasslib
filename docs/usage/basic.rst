Basic Usage
===========

Consider the following Moonscript class:

.. literalinclude:: MyClass.moon
   :language: moonscript

Assuming the file is on your package search path, `luaC_pushclass` will be able to find it.

.. literalinclude:: basic.c
   :language: LCL
   :lines: 7-16
   :dedent: 4


The class can be constructed by name using `luaC_construct`. Its methods can also be called
by name using `luaC_mcall` and `luaC_pmcall`.

.. literalinclude:: basic.c
   :language: LCL
   :lines: 18-28
   :dedent: 4

