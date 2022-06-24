#include <string.h>
#include <stdlib.h>

#include "ast.h"
#include "codegen.h"
#include "data.h"
#include "declaration.h"
#include "misc.h"
#include "optimisation.h"
#include "token.h"
#include "typing.h"

#define O_dumpAST 0

static struct symtable *composite_declaration(int type);
static int typedef_declaration(struct symtable **ctype);
static int typeoftypedef(char *name, struct symtable **ctype);
static void enum_declaration(void);

int parse_type(struct symtable **ctype, int *class, int *signage) {
    int type, exstatic = 1;

    while(exstatic) {
        switch(Token.token) { 
            case TOK_EXTERN:
                if(*class == CLASS_STATIC) fatal("A type cannot have extern and static modifiers at once");
                *class = CLASS_EXTERN;
                scan(&Token);
                break;
            case TOK_STATIC:
                if(*class == CLASS_LOCAL) fatal("Static local is unimplemented"); // TODO: Static local declarations
                if(*class == CLASS_EXTERN) fatal("A type cannot have extern and static modifiers at once");
                *class = CLASS_STATIC;
                scan(&Token);
                break;
            default: exstatic = 0;
        }
    }

    exstatic = 1;
    while(exstatic) {
        switch(Token.token) {
            case TOK_UNSIGNED:
                // if(*signage == SIGN_SIGNED) fatal("A type cannot have signed and unsigned modifiers at once");
                *signage = SIGN_UNSIGNED;
                scan(&Token);
                break;
            case TOK_SIGNED:
                // if(*signage == SIGN_UNSIGNED) fatal("A type cannot have signed and unsigned modifiers at once");
                *signage = SIGN_SIGNED;
                scan(&Token);
                break;
            default: exstatic = 0;
        }
    } 

    switch(Token.token) {
        case TOK_VOID:
            type = PRIMITIVE_VOID;
            scan(&Token);
            break;
        case TOK_CHAR:
            if(*signage == -1) {
               *signage = SIGN_UNSIGNED;
            }
            type = PRIMITIVE_CHAR;
            scan(&Token);
            break;
        case TOK_SHORT:
            if(*signage == -1) {
               *signage = SIGN_SIGNED;
            }
            type = PRIMITIVE_SHORT;
            scan(&Token);
            break;
        case TOK_INT:
            if(*signage == -1) {
               *signage = SIGN_SIGNED;
            }
            type = PRIMITIVE_INT;
            scan(&Token);
            break;
        case TOK_LONG:
            if(*signage == -1) {
               *signage = SIGN_SIGNED;
            }
            type = PRIMITIVE_LONG;
            scan(&Token);
            break;

        case TOK_STRUCT:
            type = PRIMITIVE_STRUCT;
            *ctype = composite_declaration(PRIMITIVE_STRUCT);
            if(Token.token == TOK_SEMI) type = -1;
            break;
        case TOK_UNION:
            type = PRIMITIVE_UNION;
            *ctype = composite_declaration(PRIMITIVE_UNION);
            if(Token.token == TOK_SEMI) type = -1;
            break;
        case TOK_ENUM:
            type = PRIMITIVE_INT;
            enum_declaration();
            if(Token.token == TOK_SEMI) type = -1;
            break;
        case TOK_TYPEDEF:
            type = typedef_declaration(ctype);
            if(Token.token == TOK_SEMI) type = -1;
            break;
        case TOK_IDENT:
            *signage = signageoftypedef(Text);
            type = typeoftypedef(Text, ctype);
            break;
        default: fatals("Illegal type, token", Token.tokstr);
    } 

    return type;
}

int parse_stars(int type) {
    while(1) {
        if(Token.token != TOK_STAR) break;
        type = pointer_to(type);
        scan(&Token);
    }
    return type;
}

int parse_cast(struct symtable **ctype, int signage) {
    int type, class = 0;

    type = parse_stars(parse_type(ctype, &class, &signage));

    if(type == PRIMITIVE_STRUCT || type == PRIMITIVE_UNION || type == PRIMITIVE_VOID) fatal("Cannot cast a value to a struct, union or void type");

    return type;
}

static struct symtable *scalar_declaration(char *varname, int type, struct symtable *ctype, int class, int signage, struct ASTNode **tree) {
    struct symtable *sym = NULL;
    struct ASTNode *varnode, *exprnode;
    *tree = NULL;

    switch(class) {
        case CLASS_EXTERN:
        case CLASS_STATIC:
        case CLASS_GLOBAL:
            sym = findglob(varname);
            if(isnewsym(sym, class, type, ctype))
                sym = addglob(varname, type, ctype, SYM_VAR, class, signage, 1, 0);
            break;
        case CLASS_LOCAL:
            sym = addlocl(varname, type, ctype, SYM_VAR, signage, 1);
            break;
        case CLASS_PARAM:
            sym = addparm(varname, type, ctype, SYM_VAR, signage);
            break;
        case CLASS_MEMBER:
            sym = addmemb(varname, type, ctype, SYM_VAR, signage, 1);
            break;
    }

    if(Token.token == TOK_ASSIGN) {
        if(class != CLASS_GLOBAL && class != CLASS_STATIC && class != CLASS_LOCAL) fatals("Variable cannot be initialised", varname);
        scan(&Token);

        if(class == CLASS_GLOBAL || class == CLASS_STATIC) {
            sym->initlist = (int *)malloc(sizeof(int));
            sym->initlist[0] = parse_literal(type);
        } //else scan(&Token);
        if(class == CLASS_LOCAL) {
            varnode = mkastleaf(AST_IDENT, sym->type, signage, sym->ctype, sym, 0);

            exprnode = binexpr(0);
            exprnode->rvalue = 1;

            exprnode = modify_type(exprnode, varnode->type, varnode->ctype, 0);
            if(exprnode == NULL) fatal("Expression returns an incompatible type for assignment");

            *tree = mkastnode(AST_ASSIGN, exprnode->type, signage, exprnode->ctype, exprnode, NULL, varnode, NULL, 0);
        }
    }

    if(class == CLASS_GLOBAL || class == CLASS_STATIC) genglobsym(sym);

    return sym;
}

static struct symtable *array_declaration(char *varname, int type, struct symtable *ctype, int class, int signage) {
    struct symtable *sym;
    int nelems = -1;
    int maxelems;
    int *initlist;
    int i = 0, j;

    scan(&Token);

    if(Token.token != TOK_RBRACKET) {
        nelems = parse_literal(PRIMITIVE_INT);
        if(nelems <= 0) fatald("Illegal array size", nelems);
    }

    match(TOK_RBRACKET, "]");

    switch(class) {
        case CLASS_STATIC:
        case CLASS_EXTERN:
        case CLASS_GLOBAL:
            sym = findglob(varname);
            if(isnewsym(sym, class, pointer_to(type), ctype))
                sym = addglob(varname, pointer_to(type), ctype, SYM_ARRAY, class, signage, 0, 0);
            break;
        case CLASS_LOCAL:
            sym = addlocl(varname, pointer_to(type), ctype, SYM_ARRAY, signage, 0);
            break;
        default: fatal("Parameter array declarations are unimplemented");
    }

    if(Token.token == AST_ASSIGN) {
        if(class != CLASS_GLOBAL && class != CLASS_STATIC) fatals("Variable can not be initialised", varname);
        scan(&Token);
        match(TOK_LBRACE, "{"); // begin initialisation
        
#define TABLE_INCREMENT 10

        if(nelems != -1) maxelems = nelems;
        else maxelems = TABLE_INCREMENT;
        initlist = (int *)malloc(maxelems * sizeof(int));
        
        while(1) {
            if(nelems != -1 && i == maxelems) fatal("Values exceed initialisation size");
            initlist[i++] = parse_literal(type); 

            if(nelems == -1 && i == maxelems) {
                maxelems += TABLE_INCREMENT;
                initlist = (int *)realloc(initlist, maxelems * sizeof(int));
            }

            if(Token.token == TOK_RBRACE) {
                scan(&Token);
                break;
            }

            comma();
        }
        
        for(j = i; j < sym->nelems; j++) initlist[j] = 0;
        if(i > nelems) nelems = i;
        sym->initlist = initlist;

    }
    if(class != CLASS_EXTERN && nelems <= 0) fatals("Only arrays with the extern modifier may have no elements", sym->name);


    sym->nelems = nelems;
    sym->size = sym->nelems * typesize(type, ctype);

    if(class == CLASS_GLOBAL || class == CLASS_STATIC) genglobsym(sym);
    return sym;
}

static int param_declaration_list(struct symtable *oldfuncsym, struct symtable *newfuncsym) {
    int type, paramcnt = 0;
    struct symtable *ctype;
    struct symtable *protoptr = NULL;
    struct ASTNode *unused;
    int signage = -1, exstatic = 1;

    if(oldfuncsym != NULL) protoptr = oldfuncsym->member;

    while(Token.token != TOK_RPAREN) {

        if(Token.token == TOK_VOID) {
            scan(&Peektoken);
            if(Peektoken.token == TOK_RPAREN) {
                paramcnt = 0;
                scan(&Token);
                break;
            }
        }

        type = declaration_list(&ctype, CLASS_PARAM, -1, TOK_COMMA, TOK_RPAREN, &unused);
        if(type == -1) fatal("Bad type in parameter list");

        if(protoptr != NULL) {
            if(type != protoptr->type) fatald("Parameter type mismatch for function prototype", paramcnt + 1);
            protoptr = protoptr->next;
        }
        paramcnt++;

        if(Token.token == TOK_RPAREN) break;
        comma();
    }

    if(oldfuncsym != NULL && paramcnt != oldfuncsym->nelems) fatals("Parameter count mismatch for function", oldfuncsym->name);

    return paramcnt;
}

struct symtable *function_declaration(char *funcname, int type,  struct symtable *ctype, int class, int signage) { 
    struct ASTNode *tree, *finalstatement;
    struct symtable *oldfuncsym, *newfuncsym = NULL;
    int endlabel, paramcnt; 
    int linenum = Line;

    if((oldfuncsym = findsymbol(funcname)) != NULL)
        if(oldfuncsym->stype != SYM_FUNC) oldfuncsym = NULL;

    if(oldfuncsym == NULL) {
        endlabel = genlabel();
        newfuncsym = addglob(funcname, type, NULL, SYM_FUNC, class, signage, 0, endlabel);
    } 

    lparen();
    paramcnt = param_declaration_list(oldfuncsym, newfuncsym);
    rparen();

    if(newfuncsym) {
        newfuncsym->nelems = paramcnt;
        newfuncsym->member = Parmhead;
        oldfuncsym = newfuncsym;
    }

    Parmhead = Parmtail = NULL;

    if(Token.token == TOK_SEMI) {
        return oldfuncsym; 
    }

    Functionid = oldfuncsym; 

    Looplevel = 0;
    Switchlevel = 0;
    lbrace();
    tree = compound_statement(0);
    rbrace();

    if(type != PRIMITIVE_VOID) {

        if(tree == NULL) fatal("Function body may not be empty for function of strict type");

        finalstatement = tree->op == AST_GLUE ? tree->right : tree;
        if(finalstatement == NULL || finalstatement->op != AST_RETURN) fatal("No return for function of strict type");
    }

    tree = mkastunary(AST_FUNCTION, type, signage, ctype, tree, oldfuncsym, endlabel);
    tree->linenum = linenum;

    tree = optimisetree(tree); // optimise the AST tree 

    genAST(tree, NOLABEL, NOLABEL, NOLABEL, 0);

    freeloclsyms();
    return oldfuncsym;
}

static struct symtable *composite_declaration(int type) {
    struct symtable *ctype = NULL;
    struct symtable *m;
    struct ASTNode *unused; // idk man
    int offset;
    int t; 

    scan(&Token);
    
    if(Token.token == TOK_IDENT) {
        if(type == PRIMITIVE_STRUCT)
           ctype = findstruct(Text);
        else ctype = findunion(Text);
        scan(&Token);
    }

    if(Token.token != TOK_LBRACE) {
        if(ctype == NULL) fatals("Unknown struct/union", Text);
        return ctype;
    }

    if(ctype) fatals("Previously defined struct/union", Text);

    if(type == PRIMITIVE_STRUCT) ctype = addstruct(Text);
    else ctype = addunion(Text);
    scan(&Token);
    while(1) {
        t = declaration_list(&m, CLASS_MEMBER, -1, TOK_SEMI, TOK_RBRACE, &unused);
        if(t == -1) fatal("Bad type in member list");
        if(Token.token == TOK_SEMI) scan(&Token);
        if(Token.token == TOK_RBRACE) break;
    }

    rbrace();
    if(Membhead == NULL) fatals("No members in struct", ctype->name);
    ctype->member = Membhead;
    Membhead = Membtail = NULL;

    m = ctype->member;
    m->st_posn = 0;
    offset = typesize(m->type, m->ctype);

    for(m = m->next; m != NULL; m = m->next) {
        if(type == PRIMITIVE_STRUCT) m->st_posn = genalign(m->type, offset, 1);
        else m->st_posn = 0;
        offset += typesize(m->type, m->ctype);
    }

    ctype->size = offset;
    return ctype;
}

static void enum_declaration(void) {
    struct symtable *etype = NULL;
    char *name;
    int intval = 0;

    scan(&Token);

    if(Token.token == TOK_IDENT) {
        etype = findenumtype(Text);
        name = strdup(Text);
        scan(&Token);
    }

    if(Token.token != TOK_LBRACE) {
        if(etype == NULL) fatals("Undeclared enum:", name);
        return;
    }

    scan(&Token);

    if(etype != NULL) fatals("Redeclaration of enum:", etype->name);
    else etype = addenum(name, CLASS_ENUMTYPE, 0);

    while(1) {
        ident();
        name = strdup(Text);

        etype = findenumval(name);
        if(etype != NULL) fatals("Redeclaration of enum value:", Text);

        if(Token.token == TOK_ASSIGN) {
            scan(&Token);
            if(Token.token != TOK_INTLIT) fatal("Expected int literal after '='");
            intval = Token.intvalue;
            scan(&Token);
        }

        etype = addenum(name, CLASS_ENUMVAL, intval++);

        if(Token.token == TOK_RBRACE) break;
        comma();
    }
    scan(&Token);
}

int typedef_declaration(struct symtable **ctype) {
    int type, signage = -1, class = 0;

    scan(&Token);

    type = parse_type(ctype, &class, &signage);
    if(class != 0) fatal("Extern cannot be used in combination with a typedef declaration");

    if(findtypedef(Text) != NULL) fatals("Redefinition of typedef", Text);

    type = parse_stars(type);

    addtypedef(Text, type, *ctype, signage);
    scan(&Token);
    return type;
}

int typeoftypedef(char *name, struct symtable **ctype) {
    struct symtable *t;

    t = findtypedef(name);
    if(t == NULL) fatals("Unknown type", name);
    scan(&Token);
    *ctype = t->ctype;
    return t->type;
}

int signageoftypedef(char *name) {
    struct symtable *t;

    t = findtypedef(name);
    if(t == NULL) fatals("Unknown type", name);
    return t->signage;
}

int parse_literal(int type) {
    struct ASTNode *tree;

    tree = optimisetree(binexpr(0));

    if(tree->op == AST_CAST) {
        tree->left->type = tree->type;
        tree = tree->left;
    }

    if(tree->op != AST_INTLIT && tree->op != AST_STRLIT) fatal("Attempted to initialise a global with a non-compile-time constant");

    if(type == pointer_to(PRIMITIVE_CHAR)) {
        if(tree->op == AST_STRLIT) return tree->a_intvalue;
        if(tree->op == AST_INTLIT && tree->a_intvalue == 0) return 0;
    }

    if(inttype(type) && typesize(type, NULL) >= typesize(tree->type, NULL)) return tree->a_intvalue;

    fatal("Type mismatch for global initialisation expression");
    return 0; // will not be reached
}

struct symtable *symbol_declaration(int type, struct symtable *ctype, int class, int signage, struct ASTNode **tree) {
    struct symtable *sym = NULL;
    char *varname = strdup(Text); 

    ident();

    if(Token.token == TOK_LPAREN) return function_declaration(varname, type, ctype, class, signage);

    switch(class) {
        case CLASS_STATIC:
        case CLASS_EXTERN:
        case CLASS_GLOBAL: 
        case CLASS_LOCAL:
        case CLASS_PARAM:
            if(findlocl(varname) != NULL) fatals("Redefinition of local variable", varname);
        case CLASS_MEMBER: if(findmember(varname) != NULL) fatals("Redefinition of struct/union member", varname);
    }
    
    if(Token.token == TOK_LBRACKET) {
        sym = array_declaration(varname, type, ctype, class, signage);
        *tree = NULL;
    } else {
        sym = scalar_declaration(varname, type, ctype, class, signage, tree); 
    }

    return sym;
}

int declaration_list(struct symtable **ctype, int class, int signage, int et1, int et2, struct ASTNode **gluetree) {
    int inittype, type;
    struct symtable *sym;
    struct ASTNode *tree;
    *gluetree = NULL;

    if((inittype = parse_type(ctype, &class, &signage)) == -1) return inittype;

    while(1) {
        type = parse_stars(inittype);

        sym = symbol_declaration(type, *ctype, class, signage, &tree);

        if(sym->stype == SYM_FUNC) {
            if(class != CLASS_GLOBAL && class != CLASS_STATIC) fatal("Function declaration is not at global level");
            return type;
        }
       
        if(*gluetree == NULL) *gluetree = tree;
        else *gluetree = mkastnode(AST_GLUE, PRIMITIVE_NONE, signage, NULL, *gluetree, NULL, tree, NULL, 0);

        if(Token.token == et1 || Token.token == et2) return type;
 
        comma();
    }
}

static void asm_block(void) {
    match(TOK_ASM, "asmblock");
    printf("%s\n", Text);
}

void global_declarations(void) {
    struct symtable *ctype = NULL;
    struct ASTNode *unused;
    while(Token.token != TOK_EOF) {
        if(Token.token == TOK_ASM) {
            asm_block();
            continue;
        }

        declaration_list(&ctype, CLASS_GLOBAL, -1, TOK_SEMI, TOK_EOF, &unused);
        if(Token.token == TOK_SEMI) scan(&Token);
    }
}
