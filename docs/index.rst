.. Moonlib documentation master file, created by
   sphinx-quickstart on Sat Apr  8 12:05:26 2023.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

***************************************
Welcome to LuaClassLib's documentation!
***************************************

.. toctree::
   :caption: Contents
   :hidden:

   self
   usage/index
   reference

Introduction
============

LuaClassLib is a small C library providing Moonscript-compatible class support to C applications.
It consists of two main components: ``luaclasslib.h``, which provides class support functions independently of Moonscript,
and ``moonauxlib.h``, which requires Moonscript to be available.

Installation
============

Currently, LuaClassLib can only be installed from source.
For Linux systems with Lua installed:

.. sourcecode:: console
   
   $ git clone --recurse-submodules https://github.com/mousebyte/LuaClassLib.git
   $ mkdir build && cd build
   $ cmake .. -DCMAKE_BUILD_TYPE=Release
   $ make && sudo make install

