# include

A line of the form:

    #include "filename"

will cause the compiler to pretend that the entire contents of 'filename'
are actually contained in the file being compiled. If you want to include
a certain function in many different objects, use the 'inherit' statement
instead. #including causes each object to have it's own copy of the
code, while inheriting causes many objects to share code.
