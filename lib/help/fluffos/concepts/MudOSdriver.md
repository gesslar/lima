# MudOSdriver

- What is the MudOS driver?

The MudOS driver is the program (written in C in the case of LPmud) which
provides the lowlevel support that makes a mud possible. The driver
does many things including:

0. accepts connections from remote machines (via a communications port) and
   attaches those connections to the login object (/adm/login.c on TMI).

1. provides a set of external functions (efuns) that may be called from
   within LPC objects.

1. compiles files into a compact internal tokenized form via the new(filename)
   (or clone_object(filename)) efun.

1. interprets (executes) objects represented in the tokenized form. The
   two main ways in which code gets executed are as follows:

   a) the driver calls functions in objects based on input received from
   users (via the communications port). The specific functions that get
   called depend on what associations the objects of the mud have specified
   between player-typed commands and functions (via the
   add_action(function_name,command_name) efun). The driver also
   calls functions in LPC objects from within certain efuns (such as "init",
   "create", "clean_up", etc.).
   b) objects can cause the driver to execute code in other objects via
   the call_other(object,function_name,args,...) efun. An alternate
   form of the call_other efun is object->function_name(args,...).
