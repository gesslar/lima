# buffer

'buffer' is a cross between the LPC array type and the LPC string type.  
'buffer' is intended as a way to conveniently manipulate binary data.  
'buffer' is not zero-terminated (that is, it has an associated length). A
'buffer' is an array of bytes that is implemented using one byte per
element. buf[i] = x and x = buf[i] are allowed and do work. sizeof(buf)
works. bufferp(buf) is available. buf[i..j] should work as well. buff =
read_buffer(file_name, ...) (same args as read_bytes). also 'int
write_buffer(string file, int start, mixed source)', buf = buf1 + buf2;
buf += buf1, buf = allocate_buffer(size). The socket efuns have been
modified to accept and return the 'buffer' type. (STREAM_BINARY (3) and
DATAGRAM_BINARY (4) modes.)
