#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ast.h"
#include "data.h"
#include "codegen.h"
#include "misc.h"
#include "typing.h"

/*
 * Audr32 Codegen Backend (Audr32 Assembly)
 * 
 * Primary focus for compiler construction. (Compiler is intended to target this with higher priority than x86_64 and any other target that may come later)
 */


#define NUMFREEREGS 6
#define FIRSTPARAMREG 13
static int freereg[NUMFREEREGS];
static char *reglist[] = { "r11", "r12", "r13", "r14", "r15", "r8", "r7", "r6", "r5", "r4", "r3", "r2", "r1", "r0" };

static void pushreg(int r) {
    fprintf(Outfile, "\tpush\t%s\n", reglist[r]);
}

static void popreg(int r) {
    fprintf(Outfile, "\tpop\t%s\n", reglist[r]);
}

void freeallregisters(int keepreg) {
    for(int i = 0; i < NUMFREEREGS; i++) {
        if(i != keepreg) freereg[i] = 1;
    } 
}

static int spillreg = 0;

int allocregister(void) {
    int reg;
    for(reg = 0; reg < NUMFREEREGS; reg++) {
        if(freereg[reg]) {
            freereg[reg] = 0;
            return reg;
        }
    }

    reg = (spillreg % NUMFREEREGS);
    spillreg++;
    fprintf(Outfile, "; spilling reg %d\n", reg);
    pushreg(reg);
    return reg;
}

void freeregister(int reg) {
    if(freereg[reg] != 0) {
        fprintf(Outfile, "; error trying to free register %d\n", reg);
        fatald("Error trying to free register", reg);
    }

    if(spillreg > 0) {
        spillreg--;
        reg = (spillreg % NUMFREEREGS);
        fprintf(Outfile, "; unspilling reg %d\n", reg);
        popreg(reg);
    } else {
        freereg[reg] = 1;
    }
}

void spillallregs(void) {
    for(int i = 0; i < NUMFREEREGS; i++) pushreg(i);
}

static void unspillallregs(void) {
    for(int i = NUMFREEREGS - 1; i >= 0; i--) popreg(i);
}

void spillallparams(void) {
    for(int i = FIRSTPARAMREG; i >= NUMFREEREGS; i--) pushreg(i);
}

static void unspillallparams(void) {
    for(int i = NUMFREEREGS; i < FIRSTPARAMREG + 1; i++) popreg(i);
}

static int localOffset; // account for base pointer and return address
static int stackOffset;

void cgpreamble(char *filename) {
    int loop = genlabel(); 
    freeallregisters(NOREG);
    fprintf(Outfile,
        "\t.text\n"
        "\tcall main\n" // TODO: Allow multiple files and only implement jumping to main on main executable?
        "L%d:\n"
        "\tjmp L%d\n" 
        "\n\n"
        "printint:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n"
        "\tsub\tsp, 8\n"
        "\tmov\tr10, 0x07\n"
        "\tmov\tdx, r0\n"
        "\tmov\tcx, 1\n"
        "\tmov\tbx, 0x0F00\n"
        "\tint\t0x10\n"
        "\tadd\tsp, 8\n"
        "\tpop\tbp\n"
        "\tret\n"
        "printuint:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n"
        "\tsub\tsp, 8\n"
        "\tmov\tr10, 0x07\n"
        "\tmov\tdx, r0\n"
        "\tmov\tcx, 0\n"
        "\tmov\tbx, 0x0F00\n"
        "\tint\t0x10\n"
        "\tadd\tsp, 8\n"
        "\tpop\tbp\n"
        "\tret\n"
        "\n\n"
        "printchar:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n"
        "\tsub\tsp, 8\n"
        "\tmov\tr10, 0x01\n"
        "\tmov\tdx, r0\n"
        "\tmov\tbx, 0x0F00\n"
        "\tint\t0x10\n"
        "\tadd\tsp, 8\n"
        "\tpop\tbp\n"
        "\tret\n"
        "\n\n"
        "puts:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n"
        "\tsub\tsp, 8\n"
        "\tputs_print_loop:\n"
        "\tmov\tr10, 0x01\n"
        "\tcmp\t[8:r0], 0\n"
        "\tje\tputs_print_loop_end\n"
        "\tmov\tdx, [8:r0]\n"
        "\tpush\tr0\n"
        "\tmov\tbx, 0x0F00\n"
        "\tint\t0x10\n"
        "\tpop\tr0\n"
        "\tadd\tr0, 1\n"
        "\tjmp\tputs_print_loop\n"
        "\tputs_print_loop_end:\n"
        "\tadd\tsp, 8\n"
        "\tpop\tbp\n"
        "\tret\n"
        "\n\n"
        "getchar:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n"
        "\tsub\tsp, 8\n"
        "\tgetc_loop:\n"
        "\tmov\tr10, 0x01\n"
        "\tint\t0x16\n" // keyboard query interrupt
        "\tjnz\tgetc_loop_end\n" // exit loop if found character
        "\tjmp\tgetc_loop\n"
        "\tgetc_loop_end:\n"
        "\tmov\tr10, 0x02\n"
        "\tint\t0x16\n"
        "\tcmp\tr9, 0x00\n"
        "\tje\tgetc_loop\n"
        "\tmov\tax, r9\n"
        "\tadd\tsp, 8\n"
        "\tpop\tbp\n"
        "\tret\n",
    loop, loop);
}

void cgpostamble() {
}

void cgfuncpreamble(struct symtable *sym) {
    char *name = sym->name;
    struct symtable *parm, *locvar;
    int cnt;
    int paramOffset = 16;
    int paramReg = FIRSTPARAMREG;

    localOffset = 0;

    fprintf(Outfile,
        "\t.text\n"
        "%s:\n"
        "\tpush\tbp\n"
        "\tmov\tbp, sp\n", name);

    for(parm = sym->member, cnt = 1; parm != NULL; parm = parm->next, cnt++) {
        if(cnt > 6) {
            parm->st_posn = paramOffset;
            paramOffset += 8;
        } else {
            parm->st_posn = gengetlocaloffset(parm->size);
            cgstorelocal(paramReg--, parm);
        }
    }

    for(locvar = Loclhead; locvar != NULL; locvar = locvar->next) {
        locvar->st_posn = gengetlocaloffset(locvar->size);
    } 

    stackOffset = (localOffset + 15) & ~15;
    fprintf(Outfile, "\tsub\tsp, %d\n", stackOffset);
}

void cgfuncpostamble(struct symtable *sym) {
    cglabel(sym->st_endlabel);
    fprintf(Outfile, "\tadd\tsp, %d\n", stackOffset);
    fputs(
        "\tpop\tbp\n"
        "\tret\n", Outfile);
} 

int cgloadint(int value, int type) {
    int r = allocregister();

    fprintf(Outfile, "\tmov\t%s, %d\n", reglist[r], value);
    return r;
}

int cgloadvar(struct symtable *sym, int op) {
    int r, postreg, offset = 1;

    r = allocregister();

    if (ptrtype(sym->type))
        offset= typesize(value_at(sym->type), sym->ctype);

    if (op == AST_PREDEC || op == AST_POSTDEC)
        offset= -offset;

    if (op == AST_PREINC || op == AST_PREDEC) {
        if (sym->class == CLASS_LOCAL || sym->class == CLASS_PARAM)
            fprintf(Outfile, "\tlea\t%s, [32:bp:%d]\n", reglist[r], sym->st_posn);
        else
            fprintf(Outfile, "\tlea\t%s, [32:%s]\n", reglist[r], sym->name);

        switch (sym->size) {
            case 1: fprintf(Outfile, "\tadd\t[8:%s], %d\n", reglist[r], offset); break;
            case 4: fprintf(Outfile, "\tadd\t[32:%s], %d\n", reglist[r], offset); break;
            case 8: fprintf(Outfile, "\tadd\t[32:%s], %d\n", reglist[r], offset); break;
        }
    }

    if (sym->class == CLASS_LOCAL || sym->class == CLASS_PARAM) {
        switch (sym->size) {
            case 1: fprintf(Outfile, "\tmov\t%s, [8:bp:%d]\n", reglist[r], sym->st_posn); break;
            case 4: fprintf(Outfile, "\tmov\t%s, [32:bp:%d]\n", reglist[r], sym->st_posn); break;
            case 8: fprintf(Outfile, "\tmov\t%s, [32:bp:%d]\n", reglist[r], sym->st_posn);
        }
    } else {
        switch (sym->size) {
            case 1: fprintf(Outfile, "\tmov\t%s, [8:%s]\n", reglist[r], sym->name); break;
            case 4: fprintf(Outfile, "\tmov\t%s, [32:%s]\n", reglist[r], sym->name); break;
            case 8: fprintf(Outfile, "\tmov\t%s, [32:%s]\n", reglist[r], sym->name);
        }
    }

    if (op == AST_POSTINC || op == AST_POSTDEC) {
        postreg = allocregister();

        if (sym->class == CLASS_LOCAL || sym->class == CLASS_PARAM)
            fprintf(Outfile, "\tlea\t%s, [32:bp:%d]\n", reglist[postreg], sym->st_posn);
        else
            fprintf(Outfile, "\tlea\t%s, [32:%s]\n", reglist[postreg], sym->name);

        switch (sym->size) {
            case 1: fprintf(Outfile, "\tadd\t[8:%s], %d\n", reglist[postreg], offset); break;
            case 4: fprintf(Outfile, "\tadd\t[32:%s], %d\n", reglist[postreg], offset); break;
            case 8: fprintf(Outfile, "\tadd\t[32:%s], %d\n", reglist[postreg], offset); break;
        }
        freeregister(postreg);
    }

    return(r);
}

int cgloadglobstr(int label) {
    int r = allocregister();
    fprintf(Outfile, "\tlea\t%s, [32:L%d]\n", reglist[r], label);
    return r;
}

int cgadd(int r1, int r2) {
    fprintf(Outfile, "\tadd\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgsub(int r1, int r2) {
    fprintf(Outfile, "\tsub\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgmul(int r1, int r2) {
    fprintf(Outfile, "\tmul\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgdivmod(int r1, int r2, int op, int signage) {
    if(signage == SIGN_UNSIGNED) fprintf(Outfile, "\tdiv\t%s, %s\n", reglist[r1], reglist[r2]);
    else if(signage == SIGN_SIGNED) fprintf(Outfile, "\tidiv\t%s, %s\n", reglist[r1], reglist[r2]);
    else fatal("Invalid signage for divmod");
    if(op == AST_DIV) {
        ; // do nothing as we already do a bit of trolling with the result being in r1 already
    } else fprintf(Outfile, "\tmov\t%s, ax\n", reglist[r1]); // move remainder into r1 instead (because we want that)
    // fprintf(Outfile, "\tdiv\t%s\n")
    freeregister(r2);
    return r1;
}

int cgand(int r1, int r2) {
    fprintf(Outfile, "\tand\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgor(int r1, int r2) {
    fprintf(Outfile, "\tor\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgxor(int r1, int r2) {
    fprintf(Outfile, "\txor\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r2);
    return r1;
}

int cgnegate(int r) {
    fprintf(Outfile, "\tneg\t%s\n", reglist[r]);
    return r;
}

int cginvert(int r) {
    fprintf(Outfile, "\tnot\t%s\n", reglist[r]);
    return r;
}

int cgshl(int r1, int r2) {
    fprintf(Outfile, "\tshl\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r1);
    return r2;
}

int cgshr(int r1, int r2) {
    fprintf(Outfile, "\tshr\t%s, %s\n", reglist[r1], reglist[r2]);
    freeregister(r1);
    return r2;
}

int cglognot(int r) {
    fprintf(Outfile, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
    fprintf(Outfile, "\tseteq\t%s\n", reglist[r]);
    return r;
}

void cgloadboolean(int r, int val) {
    fprintf(Outfile, "\tmov\t%s, %d\n", reglist[r], val);
}

int cgboolean(int r, int op, int label) {
    fprintf(Outfile, "\ttest\t%s, %s\n", reglist[r], reglist[r]);
    switch(op) {
        case AST_IF:
        case AST_WHILE:
        case AST_LOGAND:
            fprintf(Outfile, "\tje\tL%d\n", label);
            break;
        case AST_LOGOR:
            fprintf(Outfile, "\tjne\tL%d\n", label);
            break;
        default:
            fprintf(Outfile, "\tsetne\t%s\n", reglist[r]);
    }

    return r;
}

int cgcall(struct symtable *sym, int numargs) {
    int outr;
    fprintf(Outfile, "\tcall\t%s\n", sym->name);

    //if(numargs > 6) fprintf(Outfile, "\tadd\tsp, %d\n", 8 * (numargs - 6));

    unspillallparams();
    unspillallregs();

    outr = allocregister();
    fprintf(Outfile, "\tmov\t%s, ax\n", reglist[outr]);

    return outr;
}

void cgcopyarg(int r, int argposn) {
    if(argposn > 6) {
        fprintf(Outfile, "\tpush\t%s\n", reglist[r]);
    } else {
        fprintf(Outfile, "\tmov\t%s, %s\n", reglist[FIRSTPARAMREG - argposn + 1], reglist[r]);
    }
    freeregister(r);
}

int cgshlconst(int r, int val) {
    fprintf(Outfile, "\tshl\t%s, %d\n", reglist[r], val);
    return r;
}

int cgstoreglob(int r, struct symtable *sym) {
    if(cgprimsize(sym->type) == 4) {
        fprintf(Outfile, "\tmov\t[32:%s], %s\n", sym->name, reglist[r]);
    } else {
        switch(sym->type) {
            case PRIMITIVE_CHAR:
                fprintf(Outfile, "\tmov\t[8:%s], %s\n", sym->name, reglist[r]);
                break;
            case PRIMITIVE_SHORT:
                fprintf(Outfile, "\tmov\t[16:%s], %s\n", sym->name, reglist[r]);
            case PRIMITIVE_LONG: 
                fatald("Unimplemented type in cgstoreglob():", sym->type);
            default:
                fatald("Bad type in cgstoreglob():", sym->type);
        }
    }

    return r;
}

int cgstorelocal(int r, struct symtable *sym) {
    if(cgprimsize(sym->type) == 4) {
        fprintf(Outfile, "\tmov\t[32:bp:%d], %s\n", sym->st_posn, reglist[r]);
    } else {
        switch(sym->type) {
            case PRIMITIVE_CHAR:
                fprintf(Outfile, "\tmov\t[8:bp:%d], %s\n", sym->st_posn, reglist[r]);
                break;
            case PRIMITIVE_SHORT:
                fprintf(Outfile, "\tmov\t[16:bp:%d], %s\n", sym->st_posn, reglist[r]);
                break;
            case PRIMITIVE_INT:
                fprintf(Outfile, "\tmov\t[32:bp:%d], %s\n", sym->st_posn, reglist[r]);
                break;
            case PRIMITIVE_LONG: 
                fatald("Unimplemented type in cgstorelocal():", sym->st_posn);
            default:
                fatald("Bad type in cgstorelocal():", sym->st_posn);
        }
    }

    return r;
}

void cgglobsym(struct symtable *node) {

    int size, type;
    int initvalue;
    int i;
   
    if(node == NULL) return;

    if(node->stype == SYM_FUNC) return; // do not add functions (labels used instead)
    
    if(node->stype == SYM_ARRAY) {
        size = typesize(value_at(node->type), node->ctype);
        type = value_at(node->type);
    } else {
        size = node->size;
        type = node->type;
    }


    int Lend = genlabel();
    fprintf(Outfile, "\tjmp\tL%d\n", Lend);
    fprintf(Outfile, "\t.data\n");
    fprintf(Outfile, "%s:", node->name); 

    for(i = 0; i < node->nelems; i++) {
        initvalue = 0;
        if(node->initlist != NULL) initvalue = node->initlist[i];

        switch(size) {
            case 1:
                fprintf(Outfile, "\t.byte\t%d\n", initvalue);
                break;
            case 2:
                fprintf(Outfile, "\t.word\t%d\n", initvalue);
                break;
            case 4:
                if(node->initlist != NULL && type == pointer_to(PRIMITIVE_CHAR) && initvalue != 0)
                    fprintf(Outfile, "\t.dword\tL%d\n", initvalue);
                else
                    fprintf(Outfile, "\t.dword\t%d\n", initvalue);
                break;
            case 8:
                fprintf(Outfile, "\t.dword\t%d\n\t.dword\t%d\n", (int32_t)((initvalue & 0xFFFFFFFF00000000) >> 32), (initvalue & 0x00000000FFFFFFFF)); // split 64 bit into two 32 bits
                break;
            default:
                for(int i = 0; i < size; i++) fprintf(Outfile, "\t.byte\t0\n");
        }
    }
    fprintf(Outfile, "\t.text\n");
    fprintf(Outfile, "L%d:\n", Lend);
}

static char *cmplist[] = { "seteq", "setne", "setlt", "setgt", "setle", "setge" };

int cgcompareset(int ASTop, int r1, int r2, int type) {
    int size = cgprimsize(type);

    if(ASTop < AST_EQ || ASTop > AST_GE) fatal("Bad ASTop in cgcompareset()");

    // no need to compare
    fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
    fprintf(Outfile, "\t%s\t%s\n", cmplist[ASTop - AST_EQ], reglist[r2]);
    freeregister(r1);
    return r2;
}

void cglabel(int l) {
    fprintf(Outfile, "L%d:\n", l);
}

void cgjmp(int l) {
    fprintf(Outfile, "\tjmp\tL%d\n", l);
}

static char *invcmplist[] = { "jne", "je", "jge", "jle", "jg", "jl" };

int cgcomparejmp(int ASTop, int r1, int r2, int label, int type) {
    if(ASTop < AST_EQ || ASTop > AST_GE) fatal("Bad ASTop in cgcomparejmp()");

    fprintf(Outfile, "\tcmp\t%s, %s\n", reglist[r1], reglist[r2]);
    fprintf(Outfile, "\t%s\tL%d\n", invcmplist[ASTop - AST_EQ], label);
    freeregister(r1);
    freeregister(r2);
    return NOREG;
}

int cgwiden(int r, int oldtype, int newtype) {
    return r; // si si, we do nothing
}

int cgprimsize(int type) {
    if(ptrtype(type)) return 4; // pointer size
    switch(type) {
        case PRIMITIVE_CHAR: return 1;
        case PRIMITIVE_SHORT: return 2;
        case PRIMITIVE_INT: return 4;
        case PRIMITIVE_LONG: return 8; // unimplemented globally
        default: fatald("Bad type in cgprimsize():", type);
    }
    return 0;
}



void cgreturn(int reg, struct symtable *sym) {
    if(reg != NOREG) {
        if(ptrtype(sym->type))
            fprintf(Outfile, "\tmov\tax, %s\n", reglist[reg]);
        else {
            switch(sym->type) {
                case PRIMITIVE_CHAR:
                    fprintf(Outfile, "\tmov\tax, %s\n", reglist[reg]);
                    break;
                case PRIMITIVE_SHORT:
                    fprintf(Outfile, "\tmov\tax, %s\n", reglist[reg]);
                        break;
                case PRIMITIVE_INT:
                    fprintf(Outfile, "\tmov\tax, %s\n", reglist[reg]);
                    break;
                case PRIMITIVE_LONG:
                    fatald("Unimplemented type in cgstoreglob:", sym->type);
                default:
                    fatald("Bad type in cgreturn:", sym->type);
            }
        }
    }

    cgjmp(sym->st_endlabel);
}

int cgaddress(struct symtable *sym) {
    int r = allocregister();
    

    // TODO: Ensure local is correct
    if(sym->class == CLASS_GLOBAL || sym->class == CLASS_EXTERN || sym->class == CLASS_STATIC) fprintf(Outfile, "\tlea\t%s, [32:%s]\n", reglist[r], sym->name);
    else {
        fprintf(Outfile, "\tlea\t%s, [32:bp:%d]\n", reglist[r], sym->st_posn);
    }
    return r;
}

int cgderef(int r, int type) {
    int newtype = value_at(type);

    int size = cgprimsize(newtype);

    switch(size) {
        case 1:
            fprintf(Outfile, "\tmov\t%s, [8:%s]\n", reglist[r], reglist[r]); 
            break;
        case 2:
            fprintf(Outfile, "\tmov\t%s, [16:%s]\n", reglist[r], reglist[r]);
            break;
        case 4:
        case 8: // unimplemented globally
            fprintf(Outfile, "\tmov\t%s, [32:%s]\n", reglist[r], reglist[r]);
            break;
        default: fatald("Can't cgderef() on type:", type);
    }

    return r;
}



int cgstorederef(int r1, int r2, int type) {

    // TODO: Implement `long` (qword), variables ever use pointers, perhaps I can make a special pointer mode for 64 bit pointers?
    
    int size = cgprimsize(type);

    switch(size) {
        case 1:
            fprintf(Outfile, "\tmov\t[8:%s], %s\n", reglist[r2], reglist[r1]);
            break;
        case 2:
            fprintf(Outfile, "\tmov\t[16:%s], %s\n", reglist[r2], reglist[r1]);
            break;
        case 4:
        case 8: // unimplemented globally (use higher + lower)
            fprintf(Outfile, "\tmov\t[32:%s], %s\n", reglist[r2], reglist[r1]);
            break;
        default:
            fatald("Can not cgstorederef() on type:", type);
    }

    return r1;
}

void cgglobstr(int l, char *str, int append, int *Lend) {
    
    if(!append) {
        fprintf(Outfile, "\tjmp L%d\n", *Lend);
        fprintf(Outfile, "\t.data\n");
        cglabel(l);
    }
    char *cptr;

    for(cptr = str; *cptr; cptr++) {
        fprintf(Outfile, "\t.byte\t0x%02x\n", *cptr);
    }
   
}

void cgglobstrend(int l) {
    fprintf(Outfile, "\t.byte\t0x00\n"); // null terminate
    fprintf(Outfile, "\t.text\n");
    fprintf(Outfile, "L%d:\n", l);
} 



int cggetlocaloffset(int size) {
    localOffset += (size > 4) ? size : 4;
    return -localOffset;
}



int cgalign(int type, int offset, int direction) {
    int alignment;

    switch(type) {
        case PRIMITIVE_CHAR: break;
        default:
            alignment = 4;
            offset = (offset + direction * (alignment - 1)) & ~(alignment - 1);
    }
    return offset;
}

void cgswitch(int reg, int casecount, int toplabel, int *caselabel, int *caseval, int defaultlabel) {
    int i;

    if(casecount == 0) {
        caseval[0] = 0;
        caselabel[0] = defaultlabel;
        casecount = 1;
    } 

    int Lprocess = genlabel();
    cglabel(Lprocess);
    for(i = 0; i < casecount; i++) {
        fprintf(Outfile, "\tcmp\t%s, %d\n", reglist[reg], caseval[i]); // compare values
        fprintf(Outfile, "\tje\tL%d\n", caselabel[i]); // jump to label if the passed in value equals the case condition value
    }
    fprintf(Outfile, "\tjmp\tL%d\n", defaultlabel); // jump to default case if nothing matches

    cglabel(toplabel);
    fprintf(Outfile, "\tjmp\tL%d\n", Lprocess); // jump to switch case checks
}

void cgmove(int r1, int r2) {
    fprintf(Outfile, "\tmov\t%s, %s\n", reglist[r2], reglist[r1]); // reverse order as that is what is expected
}

void cgenumline(int line) {
    // we do not support debug lines
}

void cginlineasm(char *data) {
    fprintf(Outfile, "\t%s\n", data);
}
