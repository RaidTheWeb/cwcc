#include "../include/std.h"

char *test = "Yes!";

void fred(char *ptr) {
    puts(ptr);
}

int main() {
    fred(test);
    return 0;
}
