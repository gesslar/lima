# array

2D arrays can be done in LPC, quite simply. Just treat them as an array of
arrays. For example,

```c
a = allocate(10);
a[0] = allocate(10);
a[1] = allocate(10);
// ...etc...
```

Then you can reference array 0, element 0, via

    a[0][0]

You can't declare an array of more than 1 dimension (using the type \*
notation, if you have type checking on), but you can have an array of more
than one dimension. If you have type checking on, you will probably have
to declare them as type mixed.

This also works:

    mixed a;
    a = ({ ({ 1, 2, 3 }), ({ 1, 2, 3 }) });

In the above example, a[0] would be ({ 1, 2, 3 }), and a[0][2] would be 3.

Or this:

```c
mixed a;
a = ({ 0, 0, 0, 0 });  /* just to get the array to size 4 */
a[0] = ({ 1, 2, 3 });
a[1] = ({ 1, 2, 3 });
// ...etc...
```

> John Price a.k.a. Raistlin
