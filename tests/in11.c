#include "../include/std.h"

int param8(int a, int b, int c, int d, int e, int f) {
    printint(a);
    printint(b);
    printint(c);
    printint(d);
    printint(e);
    printint(f);
    return (0);
}

int fred(int a, int b, int c) {
    return (a + b + c);
}

int main() {
    int x;
    param8(1, 2, 3, 5, 8, 13);
    x = fred(2, 3, 4);
    printint(x);
    return (0);
}

