class Base
    squeak: (n)=>
        @n = n
        "n is now #{n}, squeak!"
    new: (s)=>
        @str = s
    @__inherited: (cls)=>
        cls.var = "Eek!"
