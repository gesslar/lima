# clean_up

### NAME

    clean_up - periodically called in inactive objects

### SYNOPSIS

    int clean_up( int inherited );

### DESCRIPTION

    The  clean_up()  function is called by the driver on a regular basis in
    all objects  that  have  been  inactive  for  the  time  specified  for
    clean_up()  in  the  runtime configuration file.  One flag is passed to
    the function, specifying whether or not the object has been  inheritted
    by anything. The inherited passed as argument will be 0 for clone objects,
    1 for a simple loaded object, and greater when the object is cloned or 
    inherited by some existing object. It is recommended not to self_destruct 
    the object when the reference count is greater than one.
    
    If clean_up() returns 0, clean_up() will never be called
    again on that object.  If it returns 1, it will be  called  again  when
    the object remains inactive for the specified clean_up() delay.

    One  thing that might be commonly done by an object in this function is
    destructing itself to conserve memory.

