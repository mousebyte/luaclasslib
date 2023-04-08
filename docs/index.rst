.. Moonlib documentation master file, created by
   sphinx-quickstart on Sat Apr  8 12:05:26 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to LuaClassLib's documentation!
=======================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:



Indices and tables
==================

* :ref:`genindex`

Introduction
============

LuaClassLib is a small C library providing Moonscript-compatible class support to C applications.
It consists of two main components: luaclasslib.h, which provides class support functions independently of Moonscript,
and moonauxlib.h, which requires Moonscript to be available.

LuaClassLib
===========

.. doxygenfile:: luaclasslib.h
   :project: LuaClassLib
   :sections: briefdescription innerclass public-attrib func define 
