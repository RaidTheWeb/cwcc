/**
 * Minimal Stripped Brainfuck Implementation
 * Only implements `+`, `-`, `.`, `,`, `<` and `>`
 *
 * (For Audr32)
 */


#include <std.h>



int main() {

    int array[100];

    // Brainfuck Program
    // char *input = "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++..>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.>++++++++++++++++++++++++++++++++.>+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.<<.>>>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++.<<<<.>>>>>>++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++."; // print Hello World

    char *input = ",.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,.,."; // echo user input
    int curr = 0, ptr = 0;

    while(*input) {
        switch(*input) {
            case '+': // increment pointer location
                array[ptr] = array[ptr] + 1; 
                break;
            case '-': // decrement pointer location
                array[ptr] = array[ptr] - 1;
                break;
            case '>': // increment pointer
                ptr++;
                break;
            case '<': // decrement pointer
                ptr--;
                break;
            case '.': // print character from pointer location
                printchar(array[ptr]);
                break;
            case ',': // input character into pointer location
                array[ptr] = getchar();
                break;
            default: // ignore everything else
                break;
        }
        input++; // reference next char
    }


    return 0;
}
