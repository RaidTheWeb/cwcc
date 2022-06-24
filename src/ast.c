#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "data.h"
#include "misc.h"
#include "typing.h"

struct ASTNode *mkastnode(int op, int type, int signage, struct symtable *ctype, struct ASTNode *left, struct ASTNode *mid, struct ASTNode *right, struct symtable *sym, int intvalue) {
    struct ASTNode *n;

    n = (struct ASTNode *)malloc(sizeof(struct ASTNode));
    if(n == NULL) {
        fprintf(stderr, "Unable to malloc in mkastnode()\n");
        exit(1);
    }

    n->op = op;
    n->type = type;
    n->ctype = ctype;
    n->left = left;
    n->mid = mid;
    n->right = right;
    n->sym = sym;
    n->a_intvalue = intvalue;
    n->linenum = 0;
    n->inlinestr = NULL;
    return n;
}

struct ASTNode *mkastleaf(int op, int type, int signage, struct symtable *ctype, struct symtable *sym, int intvalue) {
    return (mkastnode(op, type, signage, ctype, NULL, NULL, NULL, sym, intvalue));
}

struct ASTNode *mkastunary(int op, int type, int signage, struct symtable *ctype, struct ASTNode *left, struct symtable *sym, int intvalue) {
    return (mkastnode(op, type, signage, ctype, left, NULL, NULL, sym, intvalue));
}

static int gendumplabel(void) {
    static int id = 1;
    return id++;
}

/*void dumpAST(struct ASTNode *n, int label, int level) {
    int Lfalse, Lstart, Lend;


    switch (n->op) {
        case AST_IF:
            Lfalse = gendumplabel();
            for (int i=0; i < level; i++) fprintf(stdout, " ");
            fprintf(stdout, "AST_IF");
            if (n->right) { Lend = gendumplabel();
                fprintf(stdout, ", end L%d", Lend);
            }
            fprintf(stdout, "\n");
            dumpAST(n->left, Lfalse, level+2);
            dumpAST(n->mid, NOLABEL, level+2);
            if (n->right) dumpAST(n->right, NOLABEL, level+2);
            return;
        case AST_WHILE:
        Lstart = gendumplabel();
        for (int i=0; i < level; i++) fprintf(stdout, " ");
        fprintf(stdout, "AST_WHILE, start L%d\n", Lstart);
        Lend = gendumplabel();
        dumpAST(n->left, Lend, level+2);
        dumpAST(n->right, NOLABEL, level+2);
        return;
    }

    if (n->op==AST_GLUE) level= -2;

    if (n->left) dumpAST(n->left, NOLABEL, level+2);
    if (n->right) dumpAST(n->right, NOLABEL, level+2);


    for (int i=0; i < level; i++) fprintf(stdout, " ");
    switch (n->op) {
        case AST_GLUE:
            fprintf(stdout, "\n\n"); return;
        case AST_FUNCTION:
            fprintf(stdout, "AST_FUNCTION %s\n", Symtable[n->id].name); return;
        case AST_ADD:
            fprintf(stdout, "AST_ADD\n"); return;
        case AST_SUB:
            fprintf(stdout, "AST_SUBTRACT\n"); return;
        case AST_MUL:
            fprintf(stdout, "AST_MULTIPLY\n"); return;
        case AST_DIV:
            fprintf(stdout, "AST_DIVIDE\n"); return;
        case AST_EQ:
            fprintf(stdout, "A_EQ\n"); return;
        case AST_NE:
            fprintf(stdout, "AST_NE\n"); return;
        case AST_LT:
            fprintf(stdout, "AST_LE\n"); return;
        case AST_GT:
            fprintf(stdout, "AST_GT\n"); return;
        case AST_LE:
            fprintf(stdout, "AST_LE\n"); return;
        case AST_GE:
            fprintf(stdout, "AST_GE\n"); return;
        case AST_INTLIT:
            fprintf(stdout, "AST_INTLIT %d\n", n->a_intvalue); return;
        case AST_IDENT:
            if (n->rvalue)
                fprintf(stdout, "AST_IDENT rval %s\n", Symtable[n->id].name);
            else
                fprintf(stdout, "AST_IDENT %s\n", Symtable[n->id].name);
            return;
        case AST_ASSIGN:
            fprintf(stdout, "AST_ASSIGN\n"); return;
        case AST_WIDEN:
            fprintf(stdout, "AST_WIDEN\n"); return;
        case AST_RETURN:
            fprintf(stdout, "AST_RETURN\n"); return;
        case AST_FUNCCALL:
            fprintf(stdout, "AST_FUNCCALL %s\n", Symtable[n->id].name); return;
        case AST_ADDR:
            fprintf(stdout, "AST_ADDR %s\n", Symtable[n->id].name); return;
        case AST_DEREF:
            if (n->rvalue)
                fprintf(stdout, "A_DEREF rval\n");
            else
                fprintf(stdout, "AST_DEREF\n");
            return;
        case AST_SCALE:
            fprintf(stdout, "AST_SCALE %d\n", n->a_size); return;
        default:
            fatald("Unknown dumpAST operator", n->op);
    }
}
*/
