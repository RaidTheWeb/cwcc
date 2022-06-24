#include "../include/std.h"

struct x {
    char x;
    char y;
};

int main() {

    puti(sizeof(char)); // 1
    puti(sizeof(struct x)); // 2
    puti(sizeof(int)); // 4

    return 0;
}
