#ifndef TOKEN_H
#define TOKEN_H

#include "symbols.h"

struct token {
    int token;
    char *tokstr;
    int intvalue;
};

enum {
    TOK_EOF,

    /* Binary Operators */
    TOK_ASSIGN, TOK_ASPLUS, TOK_ASMINUS, TOK_ASSTAR,
    TOK_ASSLASH, TOK_ASMOD, TOK_QUESTION,
    TOK_LOGOR, TOK_LOGAND,
    TOK_OR, TOK_XOR, TOK_AMPER,
    TOK_EQ, TOK_NE, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_LSHIFT, TOK_RSHIFT,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_MOD,

    /* Other Operators */
    TOK_INC, TOK_DEC, TOK_INVERT, TOK_LOGNOT,

    /* Types */
    TOK_VOID, TOK_CHAR, TOK_INT, TOK_SHORT, TOK_LONG,

    /* Keywords */
    TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_RETURN,
    TOK_STRUCT, TOK_UNION, TOK_ENUM, TOK_TYPEDEF, TOK_EXTERN,
    TOK_BREAK, TOK_CONTINUE, TOK_CASE, TOK_DEFAULT, TOK_SWITCH,
    TOK_SIZEOF, TOK_STATIC, TOK_ASM, TOK_UNSIGNED, TOK_SIGNED,

    /* Structure Tokens */
    TOK_INTLIT, TOK_STRLIT, TOK_SEMI, TOK_IDENT,
    TOK_LBRACE, TOK_RBRACE, TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACKET, TOK_RBRACKET, TOK_COMMA, TOK_DOT, TOK_ARROW,
    TOK_COLON
    
};

int scan(struct token *t);
void rejecttoken(struct token *t);
int parse_type(struct symtable **ctype, int *class, int *signage); // actually exists in declaration.c

#endif
