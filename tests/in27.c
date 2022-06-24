// #include <stdio.h>
#include "../include/std.h"

int x, y, z1, z2;

int main() {
    for (x= 0; x <= 1; x++) {
        for (y= 0; y <= 1; y++) {
            z1= x || y; z2= x && y;
            puts("x ");
            printint(x);
            puts(", y ");
            printint(y);
            puts(", x || y ");
            printint(z1);
            puts(", x && y ");
            puti(z2);
        }
    }
  
    return(0);
}
