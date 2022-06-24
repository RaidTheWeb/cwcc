#include "../include/std.h"

int main() {
    int x;
    x = 0;
    while (x < 100) {
        if (x == 5) { x += 2; continue; }
        puti(x);
        if (x == 14) { break; }
        x += 1;
    }
    puts("Done!\n");
    return (0);
}
