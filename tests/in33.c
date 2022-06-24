// #include <stdio.h>
#include "../include/std.h"

char *str= "qwertyuiop";

int list[]= { 3, 5, 7, 9, 11, 13, 15 };

int *lptr;

int main() {
    putc(*str);
    str = str + 1; putc(*str);
    str += 1; putc(*str);
    str += 1; putc(*str);
    str -= 1; putc(*str);

    lptr = list;
    puti(*lptr);
    lptr= lptr + 1; puti(*lptr);
    lptr += 1; puti(*lptr);
    lptr += 1; puti(*lptr);
    lptr -= 1; puti(*lptr);
    return 0;
}
