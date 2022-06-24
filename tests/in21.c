#include "../include/std.h"

int main() {
    int x;
    for (x=0; x < 12; x++)
        if (x > 5)
            if (x > 10) {
                puts("10 < ");
                printint(x);
                puts("\n");
            }
        else {
            puts(" 5 < ");
            printint(x);
            puts("<= 10\n");
        }
    return 0;
}

