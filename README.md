# LuaClassLib
Extends the Lua C API to provide Moonscript-compatible class support, with or without Moonscript.

## Features

- Create, register, and construct Moonscript-like classes from C code
- Create classes for user data objects
- Introspect objects and classes
- Call methods on objects
- Inject (override) class methods

Full [documentation](https://mousebyte.github.io/LuaClassLib/) is available on Github Pages.

## Building

Example build process on Arch Linux:

```bash
git clone https://github.com/mousebyte/LuaClassLib.git
cd LuaClassLib
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make && sudo make install
```

**Next Steps**

- [ ] Expand documentation with examples
- [ ] Add to package managers

