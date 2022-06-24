#include "../include/std.h"

typedef char *charptr;

charptr charhere;

struct foo {
    struct blah {
        int g;
    } bl;
};

struct foo fb;

char a[4] = { 1, 2, 3, 4 };

int main(int argc, char **argv) {
    int x = 1, y = 2, z = 3;

    char here = 'A';
    charhere = &here;

    puti(x);
    puti(y);
    puti(z);

    putc(*charhere);
    //charhere = &here;
    //putc(*charhere);

    if(!1) puts("Hidden!\n");
    else puts("Shown!\n");

    switch(2 * 4) {
        case 2: {
            puts("2 * 4 = 2\n");
            break;
        }
        case 4: {
            puts("2 * 4 = 4\n");
            break;
        }
        case 8:
        default: {
            puts("2 * 4 = 8\n");
            break;
        }
    } 
    puts("a[1] = ");
    puti(a[1]);

    return 0;
}
