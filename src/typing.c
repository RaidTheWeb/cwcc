
#include <stddef.h>

#include "ast.h"
#include "codegen.h"
#include "typing.h"
#include "misc.h"


int inttype(int type) {
    return ((type & 0xf) == 0) && (type >= PRIMITIVE_CHAR && type <= PRIMITIVE_LONG); 
}

int ptrtype(int type) {
    return ((type & 0xf) != 0); 
}

struct ASTNode *modify_type(struct ASTNode *tree, int rtype, struct symtable *rctype, int op) {
    int ltype;
    int lsize, rsize;

    ltype = tree->type;

    if(op == AST_LOGOR || op == AST_LOGAND) {
        if(!inttype(ltype) && !ptrtype(ltype)) return NULL;
        if(!inttype(ltype) && !ptrtype(rtype)) return NULL;
        return tree;
    }

    if(ltype == PRIMITIVE_STRUCT || ltype == PRIMITIVE_UNION) fatald("Unimplemented type in modify_type()", ltype);
    if(rtype == PRIMITIVE_STRUCT || rtype == PRIMITIVE_UNION) fatald("Unimplemented type in modify_type()", rtype);

    if(inttype(ltype) && inttype(rtype)) {
        if(ltype == rtype) return tree;

        lsize = typesize(ltype, NULL); // TODO Fix
        rsize = typesize(rtype, NULL); // TODO: Fix

        if(lsize > rsize) return NULL;

        if(rsize > lsize) return mkastunary(AST_WIDEN, rtype, SIGN_UNSIGNED, NULL, tree, NULL, 0);
    }

    if(ptrtype(ltype) && ptrtype(rtype)) {
        if(op >= AST_EQ && op <= AST_GE) return tree;

        if(op == 0 && (ltype == rtype || ltype == pointer_to(PRIMITIVE_VOID))) return tree;
    }

    if(op == AST_ADD || op == AST_SUB || op == AST_ASPLUS || op == AST_ASMINUS) {
        if(inttype(ltype) && ptrtype(rtype)) {
            rsize = genprimsize(value_at(rtype));
            if(rsize > 1) return mkastunary(AST_SCALE, rtype, rctype->signage, rctype, tree, NULL, rsize);
            else return tree; 
        }
    }

    return NULL;
}

int pointer_to(int type) {
    if((type & 0xf) == 0xf) fatald("Unrecognised type in pointer_to(): type", type);
    return type + 1;
}

int value_at(int type) {
    if((type & 0xf) == 0x0) fatald("Unrecognised type in value_at(): type", type);
    return type - 1;
}

int typesize(int type, struct symtable *ctype) {
    if(type == PRIMITIVE_STRUCT || type == PRIMITIVE_UNION) return ctype->size;
    return genprimsize(type);
}
