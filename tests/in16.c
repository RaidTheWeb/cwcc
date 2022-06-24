#include "../include/std.h"

typedef int FOO;
FOO var1;

struct bar { int x; int y; };
typedef struct bar BAR;
BAR var2;

int main() {
    var1 = 5;
    printint(var1);
    printchr(10);
    var2.x = 7;
    var2.y = 10;
    printint(var2.x + var2.y);
    printchr(10);
    return(0);
}
