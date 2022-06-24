#ifndef _STD_H_
#define _STD_H_

// Standard Headers

/** NULL Pointer */
#define NULL (void *)0

/** Grab char from input */
char getchar(void);

/** Grab string from input */
char *gets(void);

/** Dump string to output */
void puts(char *str);

/** Dump integer to output */
void printint(int i);

/** Dump unsigned integer to output */
void printuint(unsigned int i);

/** Dump char to output */
void printchar(char c);

/** Dump integer to output with newline */
void puti(int i) {
    printint(i);
    printchar('\n');
}

void putui(unsigned int i) {
    printuint(i);
    printchar('\n');
}

/** Dump char to output with new line */
void putc(char c) {
    printchar(c);
    printchar('\n');
}

#endif
