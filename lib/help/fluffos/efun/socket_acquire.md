# socket_acquire

### NAME

    socket_acquire() - assume ownership of a socket

### SYNOPSIS

    #include <socket_err.h>

    int socket_acquire( int socket, string read_callback,
                        string write_callback,
                        string close_callback );

### DESCRIPTION

    socket_acquire()   is   called  to  complete  the  handshake  begun  by
    socket_release() for transferring ownership (and control) of  a  socket
    to  a new object.  socket_release() calls the release callback function
    within the new owner object to notify the object that it wishes to pass
    control  of  the  socket on.  It is the responsibility of the new owner
    socket to decide whether it wishes to accept the socket.  It  it  does,
    then socket_acquire() is called to complete the transfer.  If not, then
    the callback simply returns without completing the handshake.

    In the former case the  handshake  is  completed  and  the  new  object
    becomes  the socket owner.  The read, write and close callback function
    parameters refer to functions within the new object.  These are  speci‐
    fied  so that the MudOS driver will know which functions to call within
    the  new  object.   Decling  to   acquire   the   socket   will   cause
    socket_release()  to  return  EESOCKNOTRLSD  so  the  owner can perform
    appropriate clean-up.

    socket_acquire() may only be called within the context of  thr  release
    callback function and only with the socket specified.

### SEE ALSO

    socket_release(3)

