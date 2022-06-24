#include "../include/std.h"

union fred {
    char w;
    int  x;
    int  y;
    int z;
};

union fred var1;
union fred *varptr;

int main() {
    var1.x = 65;
    puti(var1.x);
    var1.x = 66;
    puti(var1.x); 
    puti(var1.y);
    puti(var1.w);
    puti(var1.z);
    varptr = &var1;
    varptr->x = 67;
    puti(varptr->x);
    puti(varptr->y);
    return 0;
}

