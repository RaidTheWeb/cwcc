#ifndef TYPING_H
#define TYPING_H

#include "ast.h"

// bottom 4 bits represent indirection
enum {
    PRIMITIVE_NONE,
    PRIMITIVE_VOID      = 16,
    PRIMITIVE_CHAR      = 32,
    PRIMITIVE_SHORT     = 48,
    PRIMITIVE_INT       = 64,
    PRIMITIVE_LONG      = 80,
    PRIMITIVE_STRUCT    = 96, 
    PRIMITIVE_UNION     = 112
};


// TODO: somehow the value of this affects the actual existence of the variable

enum {
    SIGN_SIGNED,
    SIGN_UNSIGNED 
};

struct ASTNode *modify_type(struct ASTNode *tree, int rtype, struct symtable *rctype, int op);
//int type_compat(int *left, int *right, int onlyright);
int value_at(int type);
int pointer_to(int type);
int inttype(int type);
int ptrtype(int type);
int typesize(int type, struct symtable *ctype);

#endif
