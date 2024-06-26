# socket_efuns

MudOS LPC Sockets Tutorial - version 1.0
1992 October 20
by Cynosure (Dave Richards)

Minor update 1994 Sept 16 by Robocoder (Anthon Pang)

One of the enhancements added to MudOS between 0.8.14 and 0.9.0 was the
inclusion of Internet sockets in LPC. It has been an ongoing dream of
the MudOS and TMI researchers to provide more tightly integrated MUDs
communicating over the Internet. Socket efuns (or LPC sockets) provide
the first level of MUD integration by allowing LPC developers to write
Internet sockets-based applications. For example, LPC objects already
exist for telnet, remote MUD finger, remote MUD tell, inter-MUD mail
delivery, and participation in the MUDWHO system.

This document is intended as a tutorial on how to use LPC sockets to
write network-based intercommunicating objects. It is intended for
intermediate to advanced LPC programmers, who already understand the
fundementals of LPC programming, and wish to write network-based LPC
services.

## Socket Modes

There are five different modes of communication, or socket modes:
MUD, STREAM, DATAGRAM, STREAM_BINARY, and DATAGRAM_BINARY. Definitions
for these modes can be obtained by including <socket.h> from the mudlib.

## MUD Mode

MUD mode is a connection-oriented communication mode where LPC data types
may be passed across the network to another MUD. For example, in MUD mode
one could send structured data, like arrays or mappings, across the network
to another MUD which is using a MUD mode socket. All LPC data types _except_
"objects" may be sent and received using MUD mode.

## STREAM Mode

STREAM mode is also a connection-oriented communication mode. It differs from
MUD mode however, in that all data is sent and received as strings. So,
using STREAM mode one can send streams of data across the network to other
MUDs. STREAM mode sockets are less powerful in that they do not
transparently send and receive all LPC data types. However, many
applications like telnet, for example, do not need to send data as
integers, or arrays. Instead, telnet views data as a stream of characters
going in each direction.

MUD mode sockets are effectively implemented as STREAM mode sockets with
special code to send and receive LPC data types. Therefore, it behooves
one to only use MUD mode if the application requires this extra data
abstraction. MUD mode is inherently slower and uses more memory buffer
space than STREAM mode. Note that when using STREAM mode, there is
no guarantee that the string being sent will arrive all at once;
instead, it may arrive in pieces which the receiving side may then have
to reassemble (the pieces will arrive in order).

## DATAGRAM Mode

Unlike MUD and STREAM modes, DATAGRAM mode is connectionless. No connection
is established between MUDs to transfer data. Instead, each piece of data
is sent to the destination MUD in a message called a datagram.

Because no connection is estsblished in DATAGRAM mode, it is possible that
the network could lose the DATAGRAM and neither MUD would realize it! For
example, if TMI sent a datagram to Portals using DATAGRAM mode and the network
lost the datagram, Portals would never receive the datagram and would be
ignorant of the datagram ever being sent at all. And TMI won't realize the
datagram was lost because no error is received if the datagram is lost.

## TCP and UDP

In MUD and STREAM mode, a TCP connection is established between the two MUDs.
TCP is a protocol that will re-transmit data if it detects that data has been
lost. It uses algorithms that send data, measure how long it takes to get a
reply, wait that long and re-transmit the data until an acknowledgement is
received. TCP also guarantees that the data packets arrive in order and
are not spuriously duplicated. (This is a very superficial description of
TCP, but does indicate the sort of work involved in making data transfer
reliable).

DATAGRAM sockets, on the other hand, use a datagram oriented protocol called
UDP. UDPs send datagrams between MUDs without the overhead of connections,
retransmission, etc. Now, since DATAGRAM mode is unreliable why would one
want to use it? Clearly TCP is better because it guarantees that data is
retransmitted if it doesn't arrive, and it deals with all the ugliness that
a network can throw at it. Simply stated, some applications really don't
care if all data arrives at the other end. Then why send it? Okay, okay.
This is a good question, but now is too soon to talk about it. Just take
it on faith that there is a need for DATAGRAM mode and we can fill in the
details a bit later.

## Creating Sockets

Ok, so let's start off by creating a MUD mode socket. We can write an
object to do this:

    #include <socket.h>

    void
    create()
    {
    	int s;

    	s = socket_create(MUD, "close_callback");
    	if (s < 0) {
    		write("socket_create: " + socket_error(s) + "\n");
    		return;
    	}

    	write("Created socket descriptor " + s + "\n");

    	socket_close(s);
    }

    void
    close_callback(int s)
    {
    	write("socket " + s + " has closed\n");
    }

Let's analyze this object to see how a socket is created. Be forewarned,
we have a long way to go before we can send data on a socket, and creation is
only one step along this trail. So be patient and be sure to understand
each example before moving.

The first thing we do is #include <socket.h>. All socket definitions
are contained in socket.h, remember? #defines exist, for example, for
MUD, STREAM and DATAGRAM. Although each name maps to a number, a well
written application will use this name instead; partly because the
#defines may one day change, but mostly because it is more obvious what
you mean to do.

We declare an integer variable s. In many sockets applocations s is used
as an abbreviation for socket. Then we call socket_create() with two
arguments. The first argument is the socket mode (which we discussed above),
and note that we use the synbolic name MUD for MUD mode. The second argument
in the above example is called the close callback function. It is the name
of a function within the object that MudOS will call when the connection is
closed. Callbacks are used often in LPC sockets efuns to notify the
object when important network events occur. Note, by the way, that we could
have passed STREAM or DATAGRAM to socket_create() to create STREAM or
DATAGRAM sockets.

All socket efuns return an exit status or return value. This value indicates
the completion status of the function. By convention all values less than 0
indicate errors or warnings. When an error is returned the application must
decide how to respond to it. In many cases there is no possibility for
success unless the MUD administrator makes changes to local config file or
the MudOS driver itself, so in many cases the application may decide to
just return on failure. In the above example, if an error is returned
(s is less than 0) then we use the socket efun socket_error() to write
an error message on the screen. This is useful during debegging, but should
probably be converted to log_file() calls eventually so the errors can be
logged and fixed.

If socket_create() succeeds, it returns an integer greater than or equal to

0.  This integer is known as a socket, a socket descriptor or a file
    descriptor. All three names have their origins in UNIX terminology. If
    socket_create() returns a value less than 0 then an error has occurred and
    no socket has been created. There really aren't any good reasons for
    getting an error. The most common errors would be specifiying an incorrect
    socket mode (which should not happen if you use the socket mode definitions
    in socket.h) and out of sockets. The MUD administrator can configure the
    number of LPC sockets that can be used by MudOS. By default, this number
    is 16, but should be changed to fit the MUD's requirements. Increasing this
    number will make more LPC sockets available. Note, that each active LPC
    socket takes away one socket that could be used to handle a player login
    or an open file. If the mud needs to handle many players and many open
    sockets simultaneously, the machine adminstrator may need to be convinced
    to increase the maximum number of open file descriptors allowed to a process.

All socket objects should be careful not to "lose" sockets. Sockets
are not like other LPC objects, there are only a finite number of them.
So in the above example, if we were able to create a socket, we close it
afterwards. Losing track of a researce is called "leaking". Socket
leaks occur when object create sockets, use them for a while and then
stop using them without closing them so that they can be used by other
objects. When an object is destructed, all LPC sockets are automatically
closed. One other thought about sockets: each socket in MudOS has it's own
unique socket descriptor (or socket number). So if one object created a
socket, and another created a second socket, neither object would receive
the same socket descriptor. Object may use this knowledge to their advantage.
It is common, for example, to use the socket descriptor as an index into
a mapping that notes various information for each open socket. Remember,
however, that once the socket is closed, it becomes available for re-use
by other socket_create() calls.

## Client/Server Model

Before continuing with the rest of the socket efuns now is probably a good
time to stop and review some basic networking concepts. Connection-oriented
communication is normally described in terms of the client/server model. In
this model each connection has a client and a server. The client is the
subject that initiates the connection and solicits some sort of service.
The server on the other hand waits for connection requests from a client,
and when they arrive provides the service requested. Ftp, for example,
operates this way. A user initiates a request by connecting to a server.
The server than acts on the requests of the client. A server
is unlike a client however in that is may be serving more than one client
at any point in time.

MUD and STREAM mode sockets use the client/server model. The client and
server each using slightly different calls to establish a connection.
Later we will discuss the peer-to-peer model when discussing DATAGRAMS,
which is slightly different than the client/server model and uses an
again slightly different method for establishing communication.

It is possible to have many different services available. Each service is
identified by a "well-known port". A port is simply an integer in the
range 1 to 65535. Most MUDs, however, can only use the range 1025 to
65535 because the first 1023 are reserved for applications like telnet,
ftp, etc that are standardized. In order for a client and server to
cummunicate the server my first create a socket, bind to a well-known port
and listen for connection requests. The client on the other hand must
create a socket and connect to the well-known port. The client connects
to the same port as the server has bound and listened on. That is why
it is called a "well-known" port. Because clients know the port a priori
and can therefore connect to it. In the Internet Request for Comments
documentation, for example, the port number is actually specified with
the text of the standard.

We will continue our discussion then in the order that socket efun
calls would normally be executed to establish a connection between the
client and server. The server must do some preperation before the
client can initiate a request, so we will start with the server.

## Binding to a Port

After the server has created a socket with socket_create() (and has verified
that the return value is greater than or equal to 0) the next logical step
is to bind to a port. This is done with socket_bind(). Let's add some more
code to the example above. First let's declare a new integer variable
called error, which is used to hold the return value from socket_bind().

    int error;

// Now let's add a call to socket_bind():

    error = socket_bind(s, 12345);
    if (error != EESUCCESS) {
    	write("socket_bind: " + socket_error(error) + "\n");
    	socket_close(s);
    	return;
    }

This should be added above socket_close(s). What does it do? Well
the first argument is no suprise, it's just the socket descriptor we
got back from socket_create(). We will have to pass s into every socket
call from now on so that MudOS knows which socket we are referring to.
Remember servers can and often do service more than one client socket at
one time, so we need to be able to keep them straight. The second argument is
simply the port number. Recall, it should be in the range 1024 to 65535.
(Actually 0 is legal too, but 0 will be discussed a bit later.)

After calling socket_bind() we check the return value to see if an error
occurred. In this case, however, we compare against EESUCCESS. In most
cases (excluding socket_create()) EESUCCESS indicates that the socket efun
completed successfully. Like socket_create() above, if an error is returned
we call socket_error() to display the error as a string. After writing the
error we simply return, right? Wrong! We discussed leaks above. If we
were to return, then an object would exist that is no longer using a socket
but stops others from using it. If we decided we cannot use the socket
anymore CLOSE IT so others can. Once socket_create() has been called and
until socket_close() has been called the socket remains open. So remember
to be a good socket citizen and close sockets when you are finished.

socket_bind() is notorious for returning EEADDRINUSE. What does it mean?
If one socket binds to port 12345 and another socket attempts to bind to the
same port (i.e. 12345) the second socket will fail to bind. This is simple
enough to understand. Once a socket binds to a port that socket owns that
port. Other attempts to bind to the same port will fail with EEADDRINUSE.
This is a very common error and can occur if two folks attempt to run the
same server demo, for example. The correct resolution to this problem is
to 1) determine if the same service is being started twice, in which case,
DON'T, once is fine, or 2) more than one developer has chosen the same
port number for multiple services. This won't work. One way to avoid this
is to have one port administrator that assign ports. Unfortunately, since
networks are generally not isolated, this port assignment must be agreed
upon by all MUDs that you intend to communicate with.

## Security

Before going on and to satiate those with crimical intent we should answer
a few questions about invalid sockets descriptors. What happens if we were
to pass in a bad socket descriptor value, just to be mean? Don't worry MudOS
will catch you doing it and tell you so. For example, if you passed in
a value that was less then 0 or greater or equal to the total number of
possible sockets in the driver, then MudOS would know you were lying though
your teeth and would return EEFDRANGE. Now if you were more sneaky you
might try to pass in a legal socket descriptor but one that was not
currently is use. MudOS would catch you again and would return EBADF. Ok
sneak that you are, you found out about a socket that in use by some other
object. What then? Well there is a 2-level security system built into
LPC sockets.

The first level of security uses the master object to validate which
objects can and cannot use sockets. It might make sense on some MUDs, for
example, for some developers to have access to LPC sockets and some others
not too. Or perhaps one developer is abusive of network priviledges and
should be banned from socket use. (In the latter case such a developer
should probably asked to leave.) To enforce such a policy, MudOS invokes
a function called valid_socket(). valid_socket() should return 0 or 1
indicating whether the requested socket operation should be allowed.
If no valid_socket() function exists, then the value 0 is assumed and all
LPC socket access is _denied_. On most MUDs however master.c would
contain the following valid_socket().

    int
    valid_socket(object eff_user, string fun, mixed *info)
    {
    return 1;
    }

The 2nd level of security is more rigid and is used to stop one
object from interfering with another object. When a socket is created
the socket becomes "owned" by the object that called socket_create().
Each time a socket efun is called the calling object is compared to the
owner object. If they are not the same, then the socket efun call is
aborted. Thus, heinous code like:

    int s;

    for (s = 0; s < 100; s++)
    	socket_close(s);

will not succeed in closing all sockets on the MUD. It will close all
sockets owned by that calling object, but all other sockets are
protected by tge 2nd-level security policy.

In either case if the 1st or 2nd level security policy is violated, then
EESECURITY is returned to the caller indicating that the socket efun was
aborted because of such a violation. If you encounter this error when
writing LPC socket code the mostly like reason would be that you passed in
the incorrect socket descriptor. This happens.

## Listening for Connections

Once a socket has been created and a port has been bound, a server must
begin listening for connections. This is done with socket_listen().
Like socket_bind() the first argument is the socket on which to listen.
The second argument is the "listen callback" function. Recall the
close callback function from socket_create()? socket_listen() specifies
a function within the object that will be called when a connection
request is received from a client. Within the listen callback function
a server can either accept the connection from the client or close it.
In most servers there is really no good reason for ever just closing a
connection. It is considered rude and should be avoided. If the
client and server implement some sort of authentication protocol
(i.e. password checking) the server should return some indication as
to why the socket is being closed. This is more a question of style,
of course, but it is difficult to debug a problem with a client or server
when a connection is made then dropped immediately. If you must do this,
be sure to log some indication as to why in a logfile so that an administrator
or developers can determine the cause and resolve the problem.

The following code starts listening for connection requests on a socket
that has been created and bound to a port.

    error = socket_listen(s, "listen_callback");
    if (error != EESUCCESS) {
    	write("socket_listen: " + socket_error(error) + "\n");
    	socket_close(s);
    	return;
    }

It is really just more of the same code, right? We call the socket_listen()
efun, check the return value for success, if an error occurs write an
error message out which includes a description of the specific error,
close the socket because we're done with it and return. In reality much
of the code necessary in sockets applications follow this pattern.

Now obviously the next thing to do is discuss the listen_callback function,
right? Right. But we won't. Instead we'll change gears here and look at
the client code for a bit. The reason for this is simple. Before a client
can initiate a connection to a server the steps we have discussed now for
the server must have occured. Until the client _does_ initiate a connection
though, no more code will be executed within the server. So it makes sense
to digress and discuss the client for a moment.

## Clients

So far we have discussed a connection from the server's perspective. Now
let's back up and walk through the client. Just like the server a client
must call socket_create() to create a socket. Since a client does not
intend that another client connect to it there is no need to bind the
port to socket. Does this mean that it cannot do so? No. It is possible
for either a client or server to bind to a port.

But why would a client wish to do so? Well the truth of the matter is this,
every socket must be bound before a connection can be established. Every one.
However, since clients don't really care what port they are bound to, a
special bind is used. It was alluded to above, we're just catching up to it
now. If a client calls socket_bind() with a second argument of 0, this
indicates that the caller doesn't care what port is selected, just pick
any one that is available. And this makes it easy for a client. If the
caller did bind to a specific port, what happens if another client is already
bound to it? The bind fails. So why not let the system do the work of
choosing the port?

Now there is one more trick up our sleeve, however. The operating system
is pretty smart. It knows whether a sosket is bound or not. It knows when
you do a connect (It knows when you've been bad or good). So, seeing how
common it would be for a client to wish to connect to a server the designers
of the 4.2/4.3BSD networking system put in a neat feature. If you connect
on a socket, and the socket is not yet bound, the system will do a
socket_bind(s, 0) for you automatically! In fact if you read BSD networking
applications you will notice that almost no sockets that are used to
initiate connect requests on ever bother doing the bind call. Laziness is
bliss.

## Initiating a Connect

Once a client has created a socket with socket_create() and optionally
bound to a port with socket_bind(), can then called socket_connect() to
initiate a connection request. socket_connect() requires four parameter:

1. the socket on which the connection to be performed, 2. the address and
   port to connection to, 3. the read callback function, and 4. the write callback
   function. There are several new concepts we need to cover so let's go
   slowly and review each argument in tern.

The first argument is old hat by now. It is just the socket or (socket
descriptor) that was returned from socket_create(). The second argument,
however, is new and exciting. It is a string representating of the address
and port to which we want to connect. Rather than waste your time talking
about Internet standard dot notation, and address classes, etc, lets just
say this about Internet (or IP) addresses. You have probably seem them
before. They are 4-byte addresses, with each byte being separated by a ".".
For example, the Internet address for eng3.sequent.com is 138.96.19.14.
There are many ways to find the IP address for a machine. The mud list
supplies the host name and IP address for the MUD machine (which do change
from time to time). The UNIX ping and nslookup commands can be used, as well.
From this point on, we will just assume you can determine the IP address
for a destination machine. One thing to think about though is that in
general most applications to not embed IP addresses within the code. It is
more common that the user would provide tha address as an argument to the
application, so don't panic.

We discussed ports above when we talked about binding. The port that
you specify to socket_connect() is the same "well-known" port number that
the server bound to above. That's the whole point, by the way, of binding.
The server and client rendezvous so to speak at the port. So how should
the second argument appear? Let's assume we wanted to connect to
eng3.sequent.com port 12345. We would write the following additional code:

    string address;

    address = "138.95.19.14 12345";
    error = socket_connect(s, address, "read_callback", "write_callback");
    if (error != EESUCCESS) {
    	write("socket_connect: " + socket_error(error) + "\n");
    	socket_close(s);
    	return;
    }

Notice the address variable. It is a string and we asssign it the
Internet address and port number with a space in between to seperate
them. This is the format that socket_connect() expects addresses and
ports to be specified.

So what are the read and write callback functions? Well we have already
talked about callbacks in general. MudOS calls these function when some
network event occurs. In these cases, MudOS calls the applications when
data becomes available to read (i.e. read callback) or that it is now
okay to write data (write callback). We will discuss how these callbacks
should work in just a bit.

The point is, once socket_connect() is called, the client initiates a
connect request to the server. Because of the way MudOS works this is
all we can do for now. The network has work to do. We have just requested
the network to send a connect request to a remote machine. That remote
machine will then inform the application that a connect request has
been received and that application will decided what to do about it. The
point is, all of this takes time. And while all this is going on MudOS
has other work to do. So rather than stop MudOS from doing useful work,
MudOS simply return EESUCCESS. Does this mean that the connection has
been made? No. Does it mean the remote machine will connect with us?
No. Do we even know if the remote machine is up? No. Do we know
anything? Yes, a little. We know that three possible things can happen
in the near future. 1. the read callback function could be called
indicating the arrival of data from the remote application, 2. the write
callback could be called indicating that it is okay to send data, or 3. the
close callback function (that was provided way back in socket_create())
could be closed indicating that the remote machine did not accept of
connection request.

Before going one step further though, make sure this is all clear.
socket_connect() tells TCP to start a connection request. When
socket_connect() returns we don't know anything about the state of
the connection as of yet. MudOS will eventually call back one of the
functions so we know what happened. This sort of programming is called
asynchronuous programming. It's opposite is known as synchronous
programming. In synchronous programming your application would wait
until the connection is either accepted or closed before returning.
But we do not have the leisure of synchronous programming in MUDs because
while we wait for the network, other things are being ignored which should
not be. So to be fair to everyone we use this asynchronous model. Which is
not really that complicated once you get the hang of it.

Of course, in the above discussion we assume you check the return value
from socket_connect() for EESUCCESS. If socket_connect() does not return
EESUCCESS then the connection request failed and no callbacks will be
called. A common mistake is to forget to check the return value and assume
one of the callbacks will eventually be called. Be careful. As in all cases
above if the connection request fails then be write an error message out to
the display, close the socket so it can be re-used and return.

Now we wait. Some time in the future either the read, write or close
callback will be called...

## Accepting a Connection

Meanwhile (back at the server) a connection request is received from the
client. When this occurs our listen callback function will be called.
This function was specified in the socket_listen() function, remember?
MudOS calls the listen callback with a single argument, that being the
socket descriptor of the socket that, socket_create(), socket_bind() and
socket_listen() was done on. This is useful if an object is listening
on multiple sockets at the same time.

The responsibility of the listen callback function is too either
accept the connection or close it. In general though, as was mentioned
above, we always accept incoming connection requests. The following
code accepts an incoming connection request:

    void
    listen_callback(int s)
    {
    	int ns;

    	ns = socket_accept(s, "read_callback", "write_callback");
    	if (ns < 0 && ns != EENOSOCKS) {
    		write("socket_connect: " + socket_error(error) + "\n");
    		return;
    	}
    }

Okay, we know what s is, right? It was passed as an argument to us.
It is the socket on which the connection indication came in on. So what
is ns? Aha! ns is an abbreviation for new socket. When a connection is
established a new socket is created for that connection. So what is s used
for? It is used to accept connections on. s will never be used, for
example, to actually send or receive data. Instead, it is used to tell
the socket application when connection requests arrive. ns, however, is
a socket that can communicate with the client. The server can send and
receive data on it etc. Make sense?

Now if you recall, we passed read and write callback function names to
the socket_connect() efun. We are doing the same thing here.
We haven't discussed them yet so don't worry, we'll get there. For now
just realize that the read and write callbacks are used for the same
purpose for socket_accept() as they are for socket_connect(). And don't
get too impatient we are almost ready to discuss them in gory detail.

It's important to note that our error handling is different here than in
other cases. If socket_accept() returns a value greater than or equal to
0 then the efun succeeded just like socket_create(). This means that the
connection has been established! This is major progress. However if it
did not succeed there is one case that is worth making an exception for.
Recall from way, way back that there are a limited number of sockets that
MudOS can use? Well, what would happen if all sockets are in use when a
connection request arrived? Well simple, stated the connection could not
be accepted because there are no sockets to accept it on. This is a shame
but it can happen. If it does EENOSOCKS is returned. This is more of
a warning than a really bad error. Sure the connection was closed because
not sockets were available, but if some other socket becomes free a new
connection could be established in the future, so this is an example of
a temporary error. In this case, it may make sense to just return.
However, in all other cases, the listen socket is closed. This means that
no new connections can be accepted, until the server is restarted. As
a result be sure to display an error message so an administrator knows
to restart the server!

---

## Flow Control

Before talking about data transfer which is sort of the climactic
section anyway, we need to discuss another paradigm. If we were
to look at networking technology today networks run at several orders
of magnitude of difference in performance. FDDI, for example, which is the
fiber opitc network standard of today runs at around 80 million bits per
second, IBM Token Ring runs around 16 million bits per second, Ethernet
around 10 millions bits per second, high-speed syncronous serial runs at
about 56 thousand bits per second and finally consumer asynchronous
modems run from 1.2 thousand to 14.4 thousand bits per second. These are
raw data rates, and one certainly cannot expect to use the entire
bandwidth of the various media. The point is this, if you ever expected
to find a hetrogeneous operating environment, networking is it. The same
protocols operate correctly at various data rates, and with different network
technologies. This is what the Internet model is all about.

Because of the variety in networks today it is difficult to make assumption
about how long things might take on networks. Recall from about that when
we initiated a connect request from a client we checked to be sure that
socket_connect() return EESUCCESS and then just returned and waited? This
was not just a cute metaphor. In reality we were. On an Ethernet we
probably waited about 3/1000s of a second for a reply, not really all that
long. So short in fact, we could have probably just hung around for the
response and delayed further processing within MudOS. But what if the
reply were to take several seconds, which is is likely to do on a SLIP or
Point-to-Point link? We don't know a priori how long things will take and
so must be prepared for the worst when dealing with networks.

There really is a point to all of this discussion. Computers are pretty
fast and are getting faster. Desktop computer can runs millions of
instructions per second. At best a similarly priced modem for such a
computer could run at around 19.2 thousand bits per second. That works
out to less than 2 thousand bytes per second. So if we compare the computer
speed to the network speed we find a vast difference in speed. The
conclusion one should reach is this: A computer can generate data much
faster than a network can send it. Therefore I could write a program
that sat in a loop and pounded the network with data, and it's very likely
that I would eventually run into a case where I had data ready to send and
the network is not ready to accept it. What should we do in this situation?
Well if we follow our previous example (i.e. socket_connect()) we would just
wait until the network is ready for more data. And so we do.

Realizing the difference in speed between computers and networks the
inventor of network software have designed the following kind of interface.
Each socket has a reasonably-sized memory buffer (typically around 4k
bytes of data) that is used to temporary hold data while is waits for the
network to send it. This temporary buffer will eventually fill, of course,
if we were to send data faster than the network can handle. When it does the
socket is said to be "flow controlled". This mean we are told that there is
no more room in this temporary buffer to hold any more data, so we should
stop sending more data. This flow controlled notion affects is directly
when writing sockets code. We have to be smart enough to send when we
can and wait when we cannot send. This may sound complicated; luckily
we can reduce this down to a few very simple rules.

Remember we said that socket_connect() generates a connection request and
returns immediately without waiting for the reply? This is true it does.
But what we didn't mention back then is that during the connection request
your application cannot send any data. This makes sense right? If the
connection has not been established then how can data be sent. This is
like dialing a phone number and starting to talk while the phone rings!
Well if we cannot send data after socket_connect(), when can we. We talked
about this before, remember? Once the connection is established then
either the read, write or close callback will be called.

So if we want to send data what should we do? Wait for the write callback
procedure! Simple enough, right? Let's forget about the close callback
for the moment because we know why it gets called and it not important any-
more. Let's just focus on the read and write callback functions. We said
when the connection has been established either the read or write callback
function will be called. Well gee, if we are waiting for the connection
to come up so we can send data and our read callack gets called what should
we do, send the data? No. Because no matter what, once the connection
comes up the write callback is guaranteed to be called at least once.
In other words, there are two directions of communcation the read direction
(data coming across the network towards us) and the write direction (data we
send out the network towards the other MUD). If we stick to our guns
and think of each direction as seperate we will avoid confusion.

Now remember the term flow-controlled? It means that we cannot send any
more data because we are waiting for the network to catch up. Well
after a connection request (i.e. socket_connect()) we are flow-
controlled. Once the connection has been accepted by the server then
our write callback function is called and we become, not flow-controlled!
This is our cue! Send data! Make sense?

Ok, so we take our chance and start sending data like mad. The network
sends this, and this, and this too. At some point the buffer inside is
going to fill up and we are going to become flow-controlled again. What do
we do then? Same as before. Wait for our write callback function to be
called again so we can start sending more data. Sending on a network is
like send bits of data in bursts. We send, we wait, we send some more.
A correctly written socket application then is one who keeps track
of whether it can or not. And only sends when MudOS says it can, and
waits when MudOS says it cannot.

Before you go off and get frustrated with LPC sockets, remember this: the
rules for flow control are not difficult to understand, but violating them
can end in disaster. Make sure you understand this flow control model. It
is fundamental to the asyncrhonous programming model. If it does not make
sense then please re-read this section or ask a sockets-savvy friend for
help, because this is very important.

## Sending data

Okay, now that we understand the constraints within which we live, it's time
to discuss actually sending data. It's been a long time in coming, but we
have convered a lot of important information along the way.

After a client's write callback is called the client is no longer
flow-controlled, which means it can begin writing data. What about the
server? Is the server flow-controlled after doing socket_accept()? This
is a good question. The answer is no, it is not. This is another difference
between clients and server. The client must wait for a write callback
before it can begin sending data, the server can begin as soon as the
connection has been accepted. Ignoring this final detail though, there
is no difference between client and servers send and receiving data. Both
client and server use the same socket efuns in the same way.

So let's do it, let's send a number in MUD mode.

    error = socket_write(s, 0);

Incredibly enough that's all it takes to send the LPC integer 0 to
the other MUD. One could do:

    error = socket_write(s, "Hello you other MUD1");

to send a hello message as a string to the other MUD. In fact in MUD mode
you can send any LPC data type except objects. This means arrays, mappings,
integers, etc. This is very powerful indeed! Ok we sent it, but what happened
on the other side so they can receive it? Remember the read callback
function, we specified it in the socket_connect() and socket_accept()
efuns? The read callback function is called when the network delivers to
the data to the socket. For example:

    void
    read_callback(int s, mixed message)
    {
    	write("Received " + message + "\n");
    }

would write the receive data! The read callback then is used by MudOS
to tell an object when new data arrives. In the above example we used
a second parameter of mixed. This is because in MUD mode any data type
can be sent across the network. It is the object's responsibility to make
sure the object type is correct. In fact, once you are able to send and
receive data correctly, you have solved the immediate problem of communication
and have opened up a whole new problem called protocol engineering. This
involves designing networking protocols that are reliable and can
interoperate with many type of computers, but that unfortunately is far
beyond the scope of the tutorial. Alas.

So have we finished with socket_write()? I hope you don't think so.
You should have noticed that we didn't check error after calling
socket_write(). Why is this? Well to be blunt, because there are four
different classes of return codes that socket_write() can return and I
figured we'd ease into the nitty gritty details. So here we go.

There are four things socket_write() can return. 1) because it sent
the data along and everything is just fine, 2) the data has been saved in
a buffer and will eventually be sent, but no more data can be accomodated
at the moment, 3) the data has not been saved in a buffer and will not
be sent since no more data can be accomodated at the moment, and

4. socket_write() is very confused and doesn't know what to do now. In the
   first case, socket_write() has sent the data and is ready for more. It will
   not call the write callback function because there is no need to, we are not
   flow-controlled. In other words, we should remember that it is still okay
   to send data. In the second case the data will be sent, it's sitting in a
   memory buffer ready to be sent when the network can send it, but the buffer
   is now full. Which means we are now flow-controlled. It would be
   inappropriate for us to try to send more at this point, so we should remember
   we cannot send any more right now. When the network empties the memory buffer
   the write callback function will be called and we can then start sending data
   again. In the third case, the data couldn't be sent at that moment,
   possibly due to socket traffic or network congestion. We are not flow-
   controlled so the write callback function must be called explicitly, to
   attempt resending the data. A call_out() is a better choice here over
   a direct function call since the delay gives the system a chance to recover,
   reduces the possibility of max eval'ing, and better simulates a callback.
   In the final case, some error has occurred either on the network connect or
   within the operating system such that the write can not be performed. In
   that case, however, it is likely nothing can be done to rectify the problem.
   In general, the best thing to do is simply close the connection. In general,
   this never happens.

Okay so much for the abstract, let's look at the specifics. socket_write()
returns EESUCCESS in case (1) above. This means that further writes to the
socket are still possible. If socket_write() returns EECALLBACK, this
indicates that the data has been buffered for output, but that further writes
should be suspended until the write callback function is called. Conversely,
if socket_write() returns EEWOULDBLOCK, this indicates that the data has
not been buffered for output, and that the write callback function must be
called again manually (to resend) before further writes are attempted.
EEALREADY means that the object has violated the flow control model,
i.e. a write was done while the socket was flow-controlled. In this event,
the data is _not_ buffered and the caller should again wait for the write
callback function. Of course, well written application will not see
EEALREADY. Any other return value should probably be interpreted as a fatal
error and the socket closed.

int socket_write(int, mixed, string|void);
int socket_release(int, object, string);
int socket_acquire(int, string, string, string);
string socket_address(int);
void dump_socket_status(void);
