# while

### The LPC while loop:

LPC's while loop is identical to that provided by C. Syntax is as follows:

    while (expression)
        statement;

where statement may be replaced by a block of statements delimited by
matching curly brackets. For example:

    while (expression) {
        statement0;
        statement1;
    }

The statements inside the body of the while loop will be executed
repeatedly for as long as the test expression evaluates to non-zero.
If the test expression is zero just prior to the execution of the loop,
then the body of the loop will not be executed. A 'break;' statement
in the body of the loop will terminate the loop (skipping any statements
in the loop that remain to be executed). A 'continue;' statement
in the body of the loop will continue the execution from the beginning
of the loop (skipping the remainder of the statements in the loop for
the current iteration).

```c
int test(int limit)
{
    int total = 0;
    int j = 0;
    while (j < limit) {
        j++;
        if ((j % 2) != 0)
            continue;
        else
            total += j;
    }
    return total;
}
```

The results of this code fragment will be to sum all of the even numbers
from 0 to to limit - 1.
