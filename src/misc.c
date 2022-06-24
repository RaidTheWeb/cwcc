#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "data.h"
#include "misc.h"
#include "token.h"

void match(int t, char *what) {
    if(Token.token == t) {
        scan(&Token);
    } else {
        fatals("Expected", what);
    }
}

void semi(void) {
    match(TOK_SEMI, ";");
}

void lbrace(void) {
    match(TOK_LBRACE, "{");
}

void rbrace(void) {
    match(TOK_RBRACE, "}");
}

void lparen(void) {
    match(TOK_LPAREN, "(");
}

void rparen(void) {
    match(TOK_RPAREN, ")");
}

void ident(void) {
    match(TOK_IDENT, "identifier");
}

void comma(void) {
    match(TOK_COMMA, "comma");
}

void fatal(char *s) {
    fprintf(stderr, "%s on line %d in %s\n", s, Line, Infilename);
    fclose(Outfile);
    unlink(Outfilename);
    exit(1);
}

void fatals(char *s1, char *s2) {
    fprintf(stderr, "%s:%s on line %d in %s\n", s1, s2, Line, Infilename);
    fclose(Outfile);
    unlink(Outfilename);
    exit(1);
}

void fatald(char *s, int d) {
    fprintf(stderr, "%s:%d on line %d in %s\n", s, d, Line, Infilename);
    fclose(Outfile);
    unlink(Outfilename);
    exit(1);
}

void fatalc(char *s, int c) {
    fprintf(stderr, "%s:%c on line %d in %s\n", s, c, Line, Infilename);
    fclose(Outfile);
    unlink(Outfilename);
    exit(1);
}
