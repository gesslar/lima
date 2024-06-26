# inherit

### The LPC inherit statement

Syntax:

    inherit pathname;

where pathname is a full path delimited by quotes (e.g. "/std/Object").

The 'inherit' statement provides the inheritance capability (a concept from
object-oriented programming) to LPC objects. Inheritance lets an object
inherit functions and variables from other objects. Because the MudOSdriver
internally stores global data and compiled code separately, many different
objects can use inheritance to share the same piece of compiled code. Each of
these objects will have its own local copy of any global variables defined
by the object. Suppose that two object A and B inherit object C. Recompiling
object either of A or B will not cause C to be recompiled. However, it will
cause any global variables provided by C to lose whatever data they had
(remember that A and B each have their own copy of the global variables
provided by C. Thus updating A will not effect the global variables of B
(even those provided by C) and vice versa).

Suppose object A inherits object B. Object A may define variables and functions
having the same names as those defined by B. If object A defines a function
of the same name as one defined by B, then the definition provided by A
overrides the definition provided by B. If A wishes to access the definition
provided by B, then it may do so. For example suppose that object A defines
its own function named query_long and yet wishes to call the query_long
function provided by the /std/Object.c object. Then A may refer to the
query_long in Object.c as `Object::query_long()`. If A defines a variable
of the same name as a global variable defined in B, then the only way that A
can access that variable is via functions provided by B. If B defines
a global variable that is not declared in A, then by default A may use that
global variable as if the global variable were defined in A (assuming B does
not choose to restrict access). Note: if object B is recompiled, object A
will continue to use the old version of object B until object A is also
recompiled.

Multiple inheritance is allowed. That is, an object may inherit more than
one other object. Suppose special.c inherits weapon.c and armor.c and that
both weapon.c and armor.c each provide their own version of query_long().
We may assume that special.c wants to sometimes act like a weapon and
sometimes act like armor. When special.c is to look like armor it
can use `armor::query_long()` and when it is to look like a weapon it
can use `weapon::query_long()`.

See the tutorial named 'types/modifiers' for more information on how
inherited objects may hide data and function definitions from objects that
inherit them.
