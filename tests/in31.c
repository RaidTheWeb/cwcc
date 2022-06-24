// #include <stdio.h>
#include "../include/std.h"

int main() {

    int i;
    int ary[5];
    char z;

    z = 'H';

    for (i = 0; i < 5; i++)
        ary[i] = i * i;

    i = 14;

    for (i = 0; i < 5; i++)
        puti(ary[i]);

    printint(i);
    printchar(' ');
    putc(z);
    return(0);
}

