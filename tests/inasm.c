#include "../include/std.h"

int main() {
    asm("mov dx, 'A'");
    asm("mov bx, 0x0F00");
    asm("int 0x10");
    return 0;
}
