#ifndef AST_H
#define AST_H

#include "symbols.h"

enum {
    AST_ASSIGN = 1, AST_ASPLUS, AST_ASMINUS, AST_ASSTAR, AST_ASSLASH,
    AST_ASMOD,
    AST_TERNARY, AST_LOGOR, AST_LOGAND, AST_OR, AST_XOR, AST_AND,
    AST_EQ, AST_NE, AST_LT, AST_GT, AST_LE, AST_GE, AST_LSHIFT, AST_RSHIFT,
    AST_ADD, AST_SUB, AST_MUL, AST_DIV, AST_MOD,
    
    AST_INTLIT, AST_STRLIT,
    AST_IDENT, AST_PRINT, AST_GLUE, 
        AST_IF, AST_WHILE, AST_FOR, AST_FUNCTION, AST_WIDEN,
        AST_FUNCCALL, AST_RETURN, AST_ADDR, AST_DEREF, AST_SCALE,
        AST_PREINC, AST_PREDEC, AST_POSTINC, AST_POSTDEC,
        AST_NEGATE, AST_INVERT, AST_LOGNOT, AST_TOBOOL, AST_BREAK,
        AST_CONTINUE, AST_SWITCH, AST_CASE, AST_DEFAULT, AST_CAST,
        AST_ASM
};

struct ASTNode {
    int op;                     // AST operation
    int type;                   // expression type
    int signage;                // expression signage
    struct symtable *ctype;     // pointer to type (struct/union)
    int rvalue;                 // node is an rvalue
    struct ASTNode *left;       // left node
    struct ASTNode *mid;        // middle node (if statements)
    struct ASTNode *right;      // right node
    struct symtable *sym;       // symbol table
    #define a_intvalue a_size   // integer value
    int a_size;                 // scale size
    int linenum;                // for gdb
    char *inlinestr;            // for inline functions that take a string as input
};

enum {
    NOREG = -1,
    NOLABEL = 0
};

struct ASTNode *mkastnode(int op, int type, int signage, struct symtable *ctype, struct ASTNode *left, struct ASTNode *mid, struct ASTNode *right, struct symtable *sym, int intvalue);
struct ASTNode *mkastleaf(int op, int type, int signage, struct symtable *ctype, struct symtable *sym, int intvalue);
struct ASTNode *mkastunary(int op, int type, int signage, struct symtable *ctype, struct ASTNode *left, struct symtable *sym, int intvalue);
struct ASTNode *binexpr(int ptp);
struct ASTNode *expressionlist(int endtoken);
struct ASTNode *funccall(void);
void dumpAST(struct ASTNode *n, int label, int level);

#endif
