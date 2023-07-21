DerivedFromUdata = require "DerivedFromUdata"

class DerivedFromUdata2 extends DerivedFromUdata
    new: (n, filename)=>
        super n, filename

    foo: (x)=>
        @number = x * @number
