#include "../include/std.h"

char *Tstring[] = {
    "EOF", "=", "||", "&&", "|", "^", "&",
    "==", "!=", ",", ">", "<=", ">=", "<<", ">>",
    "+", "-", "*", "/", "++", "--", "~", "!",
    "void", "char", "int", "long",
    "if", "else", "while", "for", "return",
    "struct", "union", "enum", "typedef",
    "extern", "break", "continue", "switch",
    "case", "default",
    "intlit", "strlit", ";", "identifier",
    "{", "}", "(", ")", "[", "]", ",", ".",
    "->", ":", ""
};

int main() {
    int i;
    char *str;

    i = 0;
    while (1) {
        str = Tstring[i];
        if (*str == 0) break;
        puts(str);
        printchar('\n');
        i++;
    }
    return(0);
}

