#include "../include/std.h"

int main() {
    int i; int j;
    i = 6; j = 12;
    if(i < j) {
        printint(i);
    } else {
        printint(j);
    }

    i = 1;
    
    while(i <= 10) {
        printint(i);
        i += 1;
    }

    i = 0;
    for (i = 1; i <= 10; i += 1) {
        printint(i);
    }

    return 0;
}
