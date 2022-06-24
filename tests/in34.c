#include "../include/std.h"

int main() {
    int a;

    puti(24 % 9);
    puti(31 % 11);
    a = 24;
    a %= 9;
    puti(a);
    a = 31;
    a %= 11;
    puti(a);

    return 0;
}
