import DerivedFromUdata from require "LCL"

class DerivedFromUdata2 extends DerivedFromUdata
    new: (n, filename)=>
        super n, filename

    foo: (x)=>
        @number = x * @number
