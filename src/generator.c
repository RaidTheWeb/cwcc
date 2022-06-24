#include <stdlib.h>

#include "ast.h"
#include "codegen.h"
#include "data.h"
#include "misc.h"
#include "typing.h"

void spillallparams(void);

static int genWHILEAST(struct ASTNode *n) {
    int Lstart, Lend;

    Lstart = genlabel();
    Lend = genlabel();
    cglabel(Lstart);

    genAST(n->left, Lend, Lstart, Lend, n->op);
    genfreeregs(NOREG);

    genAST(n->right, NOLABEL, Lstart, Lend, n->op);
    genfreeregs(NOREG);

    cgjmp(Lstart);
    cglabel(Lend);
    return NOREG;
}

static int genSWITCHAST(struct ASTNode *n) {
    int *caseval, *caselabel;
    int Ljumptop, Lend;
    int i, reg, defaultlabel = 0, casecount = 0;
    struct ASTNode *c;

    caseval = (int *)malloc((n->a_intvalue + 1) * sizeof(int));
    caselabel = (int *)malloc((n->a_intvalue + 1) * sizeof(int));

    Ljumptop = genlabel();
    Lend = genlabel();
    defaultlabel = Lend;

    reg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, 0);
    cgjmp(Ljumptop);
    genfreeregs(NOREG);

    for(i = 0, c = n->right; c != NULL; i++, c = c->right) {
        caselabel[i] = genlabel();
        caseval[i] = c->a_intvalue;
        cglabel(caselabel[i]);
        if(c->op == AST_DEFAULT) defaultlabel = caselabel[i];
        else casecount++;

        if(c->left) genAST(c->left, NOLABEL, NOLABEL, Lend, 0);
        genfreeregs(NOREG);
    }

    cgjmp(Lend);

    cgswitch(reg, casecount, Ljumptop, caselabel, caseval, defaultlabel);
    cglabel(Lend);
    return NOREG;
}

static void updateline(struct ASTNode *n) {
    if(n->linenum != 0 && Line != n->linenum) {
        Line = n->linenum;
        cgenumline(Line);
    }
}

static int genIFAST(struct ASTNode *n, int looptoplabel, int loopendlabel) {
    int Lfalse, Lend;

    Lfalse = genlabel();
    if(n->right) Lend = genlabel();

    genAST(n->left, Lfalse, NOLABEL, NOLABEL, n->op);
    genfreeregs(NOREG);

    genAST(n->mid, NOLABEL, looptoplabel, loopendlabel, n->op);
    genfreeregs(NOREG);

    if(n->right) cgjmp(Lend);

    cglabel(Lfalse);

    if(n->right) {
        genAST(n->right, NOLABEL, NOLABEL, loopendlabel, n->op);
        genfreeregs(NOREG);
        cglabel(Lend);
    }

    return NOREG;
}

static int genfunccall(struct ASTNode *n) {
    // printf("genfunccall\n");
    struct ASTNode *gluetree = n->left;
    int reg;
    int numargs = 0;

    spillallregs();
    spillallparams();

    while(gluetree) {
        reg = genAST(gluetree->right, NOLABEL, NOLABEL, NOLABEL, gluetree->op);
        cgcopyarg(reg, gluetree->a_size);

        if(numargs == 0) numargs = gluetree->a_size;

        gluetree = gluetree->left;
    }

    return cgcall(n->sym, numargs);
}

static int genternary(struct ASTNode *n) {
    int Lfalse, Lend;
    int reg, expreg;

    Lfalse = genlabel();
    Lend = genlabel();

    genAST(n->left, Lfalse, NOLABEL, NOLABEL, n->op);
    // genfreeregs(NOREG);

    reg = allocregister();

    expreg = genAST(n->mid, NOLABEL, NOLABEL, NOLABEL, n->op);
    cgmove(expreg, reg);
    freeregister(expreg); // keep result
    cgjmp(Lend);
    cglabel(Lfalse);

    expreg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);
    cgmove(expreg, reg);
    freeregister(expreg); // keep result
    cglabel(Lend);
    return reg;
}

static int genlogandor(struct ASTNode *n) {
    int Lfalse = genlabel();
    int Lend = genlabel();
    int reg;

    reg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, 0);
    cgboolean(reg, n->op, Lfalse);
    genfreeregs(NOREG);

    reg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, 0);
    cgboolean(reg, n->op, Lfalse);
    genfreeregs(reg); // keep the result for right hand side evaluation
   
    if(n->op == AST_LOGAND) {
        cgloadboolean(reg, 1);
        cgjmp(Lend);
        cglabel(Lfalse);
        cgloadboolean(reg, 0);
    } else {
        cgloadboolean(reg, 0);
        cgjmp(Lend);
        cglabel(Lfalse);
        cgloadboolean(reg, 1);
    }
    
    cglabel(Lend);
    return reg; // return result for evaluation
}

static int genasm(struct ASTNode *n) {
    cginlineasm(n->inlinestr);
    return NOREG;
}

static int cursign = -1; // sign status for unsigned/signed

int genAST(struct ASTNode *n, int iflabel, int looptoplabel, int loopendlabel, int parentASTop) { 
    // int leftreg = NOREG, rightreg = NOREG;
    int leftreg = NOREG, rightreg = NOREG;

    if(n == NULL) return NOREG; // we do nothing if there is no tree

    updateline(n);

    switch(n->op) {
        case AST_ASM: return genasm(n);
        case AST_TERNARY: return genternary(n);
        case AST_IF: return genIFAST(n, looptoplabel, loopendlabel);
        case AST_WHILE: return genWHILEAST(n);
        case AST_SWITCH: return genSWITCHAST(n);
        case AST_FUNCCALL: return genfunccall(n);
        case AST_LOGOR:
        case AST_LOGAND:
            return genlogandor(n);
        case AST_GLUE: {
            if(n->left != NULL) genAST(n->left, iflabel, looptoplabel, loopendlabel, n->op);
            genfreeregs(NOREG);
            if(n->right != NULL) genAST(n->right, iflabel, looptoplabel, loopendlabel, n->op);
            genfreeregs(NOREG);
            return NOREG;
        }
        case AST_FUNCTION: {
            cgfuncpreamble(n->sym);
            genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op);
            cgfuncpostamble(n->sym);
            return NOREG;
        }
    }

    if(n->left) leftreg = genAST(n->left, NOLABEL, NOLABEL, NOLABEL, n->op); 
    if(n->right) rightreg = genAST(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);

    switch(n->op) {
        case AST_ADD: return cgadd(leftreg, rightreg);
        case AST_SUB: return cgsub(leftreg, rightreg);
        case AST_MUL: return cgmul(leftreg, rightreg);
        // TODO: Implement proper sign resolution so that expressions automatically know what signing they need to resolve to the left hand side's signing for div and mod
        // Currently all participating types need to be the same signing to work properly, idk if this is how proper C does it, but i'd rather not risk something breaking in functionality
        // uint32_t a = 1;
        // uint32_t b = 2;
        // int32_t c = 5;
        // 
        // a / b <--- works as intended (both use unsigned)
        // (int32_t)a / b <-- fails to work as generator grabs last identifier's type info
        // (int32_t)a / (int32_t)b <-- works as intended (both cast to signed)
        // a / c <-- doesn't work as last is signed meaning codegen will think it's meant to be an idiv instead of a div
        // (int32_t)a / c <-- works as intended as all participating data is either already or is cast to signed for idiv
        case AST_DIV: return cgdivmod(leftreg, rightreg, AST_DIV, cursign);
        case AST_MOD: return cgdivmod(leftreg, rightreg, AST_MOD, cursign);
        case AST_AND: return cgand(leftreg, rightreg);
        case AST_OR: return cgor(leftreg, rightreg);
        case AST_XOR: return cgxor(leftreg, rightreg);
        case AST_LSHIFT: return cgshl(leftreg, rightreg);
        case AST_RSHIFT: return cgshr(leftreg, rightreg);
        case AST_EQ:
        case AST_NE:
        case AST_LT:
        case AST_GT:
        case AST_LE:
        case AST_GE:
            if(parentASTop == AST_IF || parentASTop == AST_WHILE || parentASTop == AST_TERNARY)
                return cgcomparejmp(n->op, leftreg, rightreg, iflabel, n->left->type);
            else
                return cgcompareset(n->op, leftreg, rightreg, n->left->type);
        case AST_INTLIT: return cgloadint(n->a_intvalue, n->type);
        case AST_STRLIT: return cgloadglobstr(n->a_intvalue);
        case AST_IDENT:
            if(n->rvalue || parentASTop == AST_DEREF) {
                cursign = n->sym->signage; // so the codegen knows what signage we're using
                return cgloadvar(n->sym, n->op); 
            } else return NOREG;
        case AST_ASPLUS:
        case AST_ASMINUS:
        case AST_ASSTAR:
        case AST_ASSLASH:
        case AST_ASMOD:
        case AST_ASSIGN:
            switch(n->op) {
                case AST_ASPLUS:
                    leftreg = cgadd(leftreg, rightreg);
                    n->right = n->left;
                    break;
                case AST_ASMINUS:
                    leftreg = cgsub(leftreg, rightreg);
                    n->right = n->left;
                    break;
                case AST_ASSTAR:
                    leftreg = cgmul(leftreg, rightreg);
                    n->right = n->left;
                    break;
                case AST_ASSLASH:
                    if(n->left->op == AST_IDENT || n->left->op == AST_DEREF) cursign = n->left->sym->signage;
                    else cursign = SIGN_UNSIGNED; // we won't be able to do anything anyway as an error will be thrown
                    leftreg = cgdivmod(leftreg, rightreg, AST_DIV, cursign);
                    n->right = n->left;
                    break;
                case AST_ASMOD:
                    if(n->left->op == AST_IDENT || n->left->op == AST_DEREF) cursign = n->left->sym->signage;
                    else cursign = SIGN_UNSIGNED; // ditto
                    leftreg = cgdivmod(leftreg, rightreg, AST_MOD, cursign);
                    n->right = n->left;
                    break;
            }

            switch(n->right->op) {
                case AST_IDENT: {
                    if(n->right->sym->class == CLASS_GLOBAL || n->right->sym->class == CLASS_EXTERN || n->right->sym->class == CLASS_STATIC)
                        return cgstoreglob(leftreg, n->right->sym);
                    else
                        return cgstorelocal(leftreg, n->right->sym);
                }
                case AST_DEREF: return cgstorederef(leftreg, rightreg, n->right->type);
                default: fatald("Can't AST_ASSIGN in genAST(), op", n->op);
            } 
        case AST_WIDEN: return cgwiden(leftreg, n->left->type, n->type);
        case AST_RETURN:
            cgreturn(leftreg, Functionid);
            return NOREG; 
        case AST_ADDR:
            if(n->sym != NULL) return cgaddress(n->sym);
            else return leftreg;
        case AST_DEREF:
            if(n->rvalue)
                return cgderef(leftreg, n->left->type);
            else
                return leftreg;
        case AST_SCALE:
            switch(n->a_size) {
                case 2: return cgshlconst(leftreg, 1);
                case 4: return cgshlconst(leftreg, 2);
                case 8: return cgshlconst(leftreg, 3);
                default:
                    rightreg = cgloadint(n->a_size, PRIMITIVE_INT);
                    return cgmul(leftreg, rightreg);
            }
            return NOREG; // will not be reached
        case AST_POSTINC:
        case AST_POSTDEC:
            return cgloadvar(n->sym, n->op);
        case AST_PREINC:
        case AST_PREDEC:
            return cgloadvar(n->left->sym, n->op);
        case AST_NEGATE: return cgnegate(leftreg);
        case AST_INVERT: return cginvert(leftreg);
        case AST_LOGNOT: return cglognot(leftreg); 
        case AST_TOBOOL: return cgboolean(leftreg, parentASTop, iflabel);
        case AST_BREAK:
            cgjmp(loopendlabel);
            return NOREG;
        case AST_CONTINUE:
            cgjmp(looptoplabel);
            return NOREG;
        case AST_CAST:
            cursign = n->signage;
            return leftreg;
        default:
            fatald("Unknown AST operator", n->op); 
    }
    return NOREG; // will never be reached
}

void genpreamble(char *filename) { cgpreamble(filename); }
void genpostamble(void) { cgpostamble(); }
void genfreeregs(int keepreg) { freeallregisters(keepreg); }
void genglobsym(struct symtable *sym) { cgglobsym(sym); }
int genprimsize(int type) { return cgprimsize(type); }
static int labelid = 0;
int genlabel(void) {
    return labelid++;
}
int genglobstr(char *str, int append, int *Lend) {
    int l = genlabel();
    cgglobstr(l, str, append, Lend);
    return l;
}
void genglobstrend(int Lend) {
    cgglobstrend(Lend);
}
int gengetlocaloffset(int type) { return cggetlocaloffset(type); }
int genalign(int type, int offset, int direction) {
    return cgalign(type, offset, direction);
}
