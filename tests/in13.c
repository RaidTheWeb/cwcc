#include "../include/std.h"

struct fred {
    int x;
    char y;
    int z;
};

struct fred var2;
struct fred *varptr;

int main() {
    int result;

    var2.x = 12;
    puti(var2.x);
    var2.y = 'c';
    puti(var2.y);
    var2.z = 4005;
    puti(var2.z);

    result = var2.x + var2.y + var2.z;
    puti(result);

    varptr = &var2;
    result = varptr->x + varptr->y + varptr->z;
    puti(result);
    return 0;
}

