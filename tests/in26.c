#include "../include/std.h"

int x;
int y = 3;

int main() {
    for (y= 0; y < 10; y++) {
        x = y > 4 ? y + 2 : y + 9;
        puti(x);
    }
    return 0;
}
