#include <stdio.h>

#include "ast.h"
#include "misc.h"
#include "token.h"
#include "typing.h"

// UNARY
static struct ASTNode *fold1(struct ASTNode *n) {
    int val;

    val = n->left->a_intvalue;
    switch(n->op) {
        case AST_WIDEN: break;
        case AST_INVERT:
            val = ~val;
            break;
        case AST_LOGNOT:
            val = !val;
            break;
        default: return n;
    }

    return mkastleaf(AST_INTLIT, n->type, SIGN_UNSIGNED, NULL, NULL, val);
}

// BINARY
static struct ASTNode *fold2(struct ASTNode *n) {
    int val, leftval, rightval;

    leftval = n->left->a_intvalue;
    rightval = n->right->a_intvalue;

    switch(n->op) {
        case AST_ADD:
            val = leftval + rightval;
            break;
        case AST_SUB:
            val = leftval - rightval;
            break;
        case AST_MUL:
            val = leftval * rightval;
            break;
        case AST_DIV:
            if(rightval == 0) fatal("Attempted to divide by zero in expression!"); // just exit because divide by zero shouldn't be allowed
            val = leftval / rightval;
            break;
        default: return n;
    }

    return mkastleaf(AST_INTLIT, n->type, SIGN_UNSIGNED, NULL, NULL, val);
}

// FOLD TREE
static struct ASTNode *fold(struct ASTNode *n) {
    if(n == NULL) return NULL;

    n->left = fold(n->left);
    n->right = fold(n->right);

    if(n->left && n->left->op == AST_INTLIT) {
        if(n->right && n->right->op == AST_INTLIT) n = fold2(n); // binary fold
        else n = fold1(n); // unary fold
    }
    
    return n;
}

struct ASTNode *optimisetree(struct ASTNode *n) {
    n = fold(n);
    return n;
}
