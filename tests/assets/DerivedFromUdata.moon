import File from require "LCL"

class DerivedFromUdata extends File
    new: (n, filename)=>
        super filename
        @number = n
