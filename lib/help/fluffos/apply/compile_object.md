# compile_object

### NAME

    compile_object  - serves as the mudlib interface for the virtual object
    facility

### SYNOPSIS

    object compile_object( string pathname );

### DESCRIPTION

    The driver  calls  compile_object(3)  in  the  event  that  the  mudlib
    instructs  the driver to load a file that does not exist.  For example,
    the driver will call compile_object("/obj/file.r")  in  master  if  the
    mudlib     calls    call_other("/obj/file.r",    "some_function")    or
    new("/obj/file.r") and /obj/file.r.c names a file that does not  exist.
    The  compile_object()  function  is  expected to return 0 if the mudlib
    does not wish to associate an object with the file name  "/obj/file.r".
    If  the  mudlib  does  wish  to  associate  an object with the filename
    "/obj/file.r", then the mudlib should return the object it wishes asso‐
    ciated.  After an association is made between an object and a filename,
    then it will be as if the file, file.r.c, did exist (to the driver) and
    when loaded produced the object that compile_object() returned.

