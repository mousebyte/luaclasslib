class Example
    new: (n)=>
        @mice = n
    add: (n)=>
        @mice = @mice + n
        "There are now #{@mice} mice!"
