# query_privs

### NAME

    query_privs() - return the privs string for an object

### SYNOPSIS

    string query_privs( object ob );

### DESCRIPTION

    Returns the privs string for an object.  The privs string is determined
    at compile time via a call to privs_file() in the  master  object,  and
    changeable via the set_privs() efun.

    This efun is only available if PRIVS is defined at driver compile time.

### SEE ALSO

    privs_file(4), set_privs(3)

