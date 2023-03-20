Base = require "Base"

class Derived extends Base
    squeak: (n)=> @func(super n)
    new: (s, f)=>
        super s
        @func = f
