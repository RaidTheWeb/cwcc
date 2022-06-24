#include "../include/std.h"

int x, y, z;

int a = 1;
int *aptr;

int main() {

    for (x = 0; x <= 1; x++)
        for (y = 0; y <= 1; y++) {
            z = x && y;
            printint(x);
            printchar(' ');
            printint(y);
            puts(" | ");
            puti(z);
        }

    for (x = 0; x <= 1; x++)
        for (y = 0; y <= 1; y++) {
            z = x || y;
            printint(x);
            printchar(' ');
            printint(y);
            puts(" | ");
            puti(z);
        }

    aptr = NULL;
    if (aptr && *aptr == 1)
        puts("aptr points at 1\n");
    else
        puts("aptr is NULL or doesn't point at 1\n");

    aptr = &a;
    if (aptr && *aptr == 1)
        puts("aptr points at 1\n");
    else
        puts("aptr is NULL or doesn't point at 1\n");

    return 0;
}

