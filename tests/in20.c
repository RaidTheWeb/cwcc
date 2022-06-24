#include "../include/std.h"

int x;
int y;

int main() {

    x = 0; y = 1;

    for (; x<5; x++, y += 2) {
        printint(x);
        printchar(' ');
        puti(y);
        // x = x + 1;
        // y = y + 2;
    }

    return 0;
}

