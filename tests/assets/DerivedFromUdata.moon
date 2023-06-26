File = require "File"

class DerivedFromUdata extends File
    new: (n, filename)=>
        super filename
        @number = n
