# LuaClassLib
Extends the Lua C API to provide Moonscript-compatible class support, with or without Moonscript.

## Features

- Create, register, and construct Moonscript-like classes from C code
- Create classes for user data objects
- Introspect objects and classes
- Call methods on objects
- Inject (override) class methods

Full [documentation](https://mousebyte.github.io/luaclasslib/) is available on Github Pages.

## Installation

LCL is available on the Arch User Repository:

```bash
yay -S luaclasslib
```

You can also build from source:

```bash
git clone --recurse-submodules https://github.com/mousebyte/LuaClassLib.git
cd LuaClassLib
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
make && sudo make install
```

**Next Steps**

- [x] Expand documentation with examples
- [x] Add to AUR
- [ ] Add to other package managers

