import File from require "lcltests"

class DerivedFromUdata extends File
    new: (n, filename)=>
        super filename
        @number = n
