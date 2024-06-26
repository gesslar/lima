# ed

### NAME

    ed() - edit a file

### SYNOPSIS

    void ed( string file, string exit_fn, int restricted );
    void ed( string file, string write_fn, string exit_fn, int restricted );

### DESCRIPTION

    This efun is only available if __OLD_ED__ is defined.

    This  is  a funny function. It will start a local editor on an optional
    file.  This editor is almost UNIX ed compatible.  When  in  the  editor
    type 'h' for help.

    The  <write_fn>  function  allows  the  mudlib to handle file locks and
    administrative logging of files modified.  When the editor writes to  a
    file,  the  driver  will  callback  the <write_fn> function twice.  The
    first time, the function is called before  the  write  takes  place  --
    <flag>  will  be  0.  If the function returns TRUE, the write will con‐
    tinue, otherwise it will abort.   The  second  time,  the  function  is
    called  after the write has completed -- <flag> will be non-zero.  This
    callback function should have the form:

        int write_fn(string fname, int flag)

    When the editor is exited, the driver will callback the <exit_fn> func‐
    tion.   This  function  allows  the  mudlib to clean up.  This callback
    function has the form:

        void exit_fn()

    The optional <restricted> flag limits the editor's  capabilities,  such
    as inserting a file, and saving using an alternate file name.

### SEE ALSO

    regexp(3), valid_read(4), valid_write(4), get_save_file_name(4)

