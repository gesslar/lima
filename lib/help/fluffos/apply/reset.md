# reset

### NAME

    reset - allows an object to do self-maintenance

### SYNOPSIS

    void reset( void );

### DESCRIPTION

    After  every  reset interval (whose exact length is determined on a mud
    by mud basis, but averages around every 2 hours), reset() is called  in
    every  object  that  currently  exists.   If  LAZY_RESETS is defined in
    options.h in the driver, reset() will only be called  in  objects  when
    they  are  touched (near players), so unused objects will not be loaded
    from the swap file to reset.  The reset() call is usually used to check
    if  any treasures and/or monsters in the room have been removed so they
    can be replaced.

### SEE ALSO

    set_reset(3), create(4)

