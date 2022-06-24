#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "codegen.h"
#include "declaration.h" // parse_cast()
#include "token.h"
#include "typing.h"
#include "data.h"
#include "ast.h"
#include "misc.h"
#include "symbols.h"

struct ASTNode *expressionlist(int endtoken) {
    struct ASTNode *tree = NULL;
    struct ASTNode *child = NULL;
    int exprcount = 0;

    while(Token.token != endtoken) {
        child = binexpr(0);
        exprcount++; 

        tree = mkastnode(AST_GLUE, PRIMITIVE_NONE, child->signage, NULL, tree, NULL, child, NULL, exprcount);

        if(Token.token == endtoken) break;

        match(TOK_COMMA, ",");
    }

    return tree;
}

static struct ASTNode *arrayaccess(struct ASTNode *left) {
    struct ASTNode *right; 
    
    if(!ptrtype(left->type)) fatal("Attempted to access a non-array/non-pointer in array fashion");

    scan(&Token);

    right = binexpr(0);

    match(TOK_RBRACKET, "]");

    if(!inttype(right->type)) fatal("Array index must be an integer");

    left->rvalue = 1;

    right = modify_type(right, left->type, left->ctype, AST_ADD);

    left = mkastnode(AST_ADD, left->type, SIGN_UNSIGNED, left->ctype, left, NULL, right, NULL, 0);
    left = mkastunary(AST_DEREF, value_at(left->type), SIGN_UNSIGNED, left->ctype, left, NULL, 0);

    return left;
}

static struct ASTNode *memberaccess(struct ASTNode *left, int withpointer) {
    struct ASTNode *right;
    struct symtable *typeptr;
    struct symtable *m;

    if(withpointer && left->type != pointer_to(PRIMITIVE_STRUCT) && left->type != pointer_to(PRIMITIVE_UNION)) fatal("Expression is not a pointer to a struct or union");

    if(!withpointer) {
        if(left->type == PRIMITIVE_STRUCT || left->type == PRIMITIVE_UNION) left->op = AST_ADDR;
        else fatal("Expression is not a struct or union");
    }

    typeptr = left->ctype;

    scan(&Token);
    ident();

    for(m = typeptr->member; m != NULL; m = m->next)
        if(!strcmp(m->name, Text)) break;
    if(m == NULL) fatals("Struct/union does not have member: ", Text);

    left->rvalue = 1;

    right = mkastleaf(AST_INTLIT, PRIMITIVE_INT, SIGN_UNSIGNED, NULL, NULL, m->st_posn);

    left = mkastnode(AST_ADD, pointer_to(m->type), SIGN_UNSIGNED, m->ctype, left, NULL, right, NULL, 0);
    left = mkastunary(AST_DEREF, m->type, SIGN_UNSIGNED, m->ctype, left, NULL, 0);
    return left;
}

static struct ASTNode *primary(int ptp);

static struct ASTNode *postfix(int ptp) {
    struct ASTNode *n;
    
    n = primary(ptp);

    while(1) {
        switch(Token.token) {
            case TOK_LBRACKET:
                n = arrayaccess(n);
                break;

            case TOK_DOT:
                n = memberaccess(n, 0);
                break;
            
            case TOK_ARROW:
                n = memberaccess(n , 1);
                break;

            case TOK_INC:
                if(n->rvalue == 1) fatal("Cannot increment on an rvalue");
                scan(&Token);
                
                if(n->op == AST_POSTINC || n->op == AST_POSTDEC) fatal("Cannot use an increment/decrement operator after an increment operator");

                n->op = AST_POSTINC;
                break;
            case TOK_DEC:
                if(n->rvalue == 1) fatal("Cannot decrement on an rvalue");
                scan(&Token);

                if(n->op == AST_POSTINC || n->op == AST_POSTDEC) fatal("Cannot use an increment/decrement after a decrement operator");

                n->op = AST_POSTDEC;
                break;
            default:
                return n;
        }
    }

    return NULL;
}

static struct ASTNode *parenexpr(int ptp) {
    struct ASTNode *n;
    int type = 0;
    struct symtable *ctype = NULL;
    int signage = -1, exstatic = 1;

    // printf("parenexpr: %s\n", Token.tokstr);
    scan(&Token);

    while(exstatic) {
        switch(Token.token) {
            case TOK_SIGNED:
                signage = SIGN_SIGNED;
                scan(&Token);
                break;
            case TOK_UNSIGNED:
                signage = SIGN_UNSIGNED;
                scan(&Token);
                break;
            default: exstatic = 0;
        }
    }

    switch(Token.token) {
        case TOK_IDENT:
            if(findtypedef(Text) == NULL) { 
                n = binexpr(0);
                break;
            }
            signage = signageoftypedef(Text);
        case TOK_VOID:
        case TOK_CHAR:
        case TOK_SHORT:
        case TOK_INT:
        case TOK_LONG:
        case TOK_STRUCT:
        case TOK_UNION:
        case TOK_ENUM:
            type = parse_cast(&ctype, signage);

            rparen();

        default:
            // printf("calling binexpr: %s\n", Token.tokstr);
            n = binexpr(0);
    }

    if(type == 0) {
        // printf("matching end of expression %s.\n", Token.tokstr);
        rparen();
    } else n = mkastunary(AST_CAST, type, signage, ctype, n, NULL, 0);
    return n;
}

static struct ASTNode *primary(int ptp) { 
    struct ASTNode *n;
    struct symtable *enumptr;
    struct symtable *varptr;
    int id, Lend;
    int type = 0;
    int size, class, signage = -1;
    struct symtable *ctype;

    switch(Token.token) {
        case TOK_STATIC:
        case TOK_EXTERN:
            fatal("Static and extern local declarations are unimplemented");
        case TOK_SIZEOF:
            scan(&Token);
            if(Token.token != TOK_LPAREN) fatal("Expected left parenthesis after sizeof operator");
            scan(&Token);

            type = parse_stars(parse_type(&ctype, &class, &signage));

            size = typesize(type, ctype);
            rparen();

            return mkastleaf(AST_INTLIT, PRIMITIVE_INT, SIGN_UNSIGNED, NULL, NULL, size);
        case TOK_INTLIT:
            if(Token.intvalue >= 0 && Token.intvalue < 256) n = mkastleaf(AST_INTLIT, PRIMITIVE_CHAR, SIGN_UNSIGNED,  NULL, NULL, Token.intvalue);
            else if(Token.intvalue >= 0 && Token.intvalue < 65536) n = mkastleaf(AST_INTLIT, PRIMITIVE_SHORT, SIGN_UNSIGNED, NULL, NULL, Token.intvalue);
            else n = mkastleaf(AST_INTLIT, PRIMITIVE_INT, SIGN_UNSIGNED, NULL, NULL, Token.intvalue);
            break;

        case TOK_STRLIT:
            Lend = genlabel();
            id = genglobstr(Text, 0, &Lend);

            while(1) {
                scan(&Peektoken);
                if(Peektoken.token != TOK_STRLIT) break;
                genglobstr(Text, 1, &Lend);
                scan(&Token);
            }

            genglobstrend(Lend);
            n = mkastleaf(AST_STRLIT, pointer_to(PRIMITIVE_CHAR), SIGN_UNSIGNED, NULL, NULL, id);
            break;
        
        case TOK_IDENT:
            if((enumptr = findenumval(Text)) != NULL) {
                n = mkastleaf(AST_INTLIT, PRIMITIVE_INT, SIGN_SIGNED, NULL, NULL, enumptr->st_posn);
                break;
            }

            if((varptr = findsymbol(Text)) == NULL) fatals("Unknown variable or function", Text);

            switch(varptr->stype) {
                case SYM_VAR:
                    n = mkastleaf(AST_IDENT, varptr->type, varptr->signage, varptr->ctype, varptr, 0);
                    break;
                case SYM_ARRAY:
                    n = mkastleaf(AST_ADDR, varptr->type, varptr->signage, varptr->ctype, varptr, 0);
                    n->rvalue = 1;
                    break;
                case SYM_FUNC:
                    scan(&Token);

                    if(Token.token != TOK_LPAREN) fatals("Function name used without parentheses", Text);
                    return funccall();
                default:
                    fatals("Identifier not a scalar variable or array variable", Text);
            }
            break;
        case TOK_LPAREN:
            return parenexpr(ptp);

        default:
            fatals("Expected a primary expression, got token", Token.tokstr);
    }
    scan(&Token);
    return n;
}

struct ASTNode *funccall(void) {
    struct ASTNode *tree;
    struct symtable *funcptr; 

    if((funcptr = findsymbol(Text)) == NULL || funcptr->stype != SYM_FUNC) fatals("Undeclared function", Text);

    lparen();

    tree = expressionlist(TOK_RPAREN);

    tree = mkastunary(AST_FUNCCALL, funcptr->type, funcptr->signage, funcptr->ctype, tree, funcptr, 0);

    rparen();
    return tree;
}

static int binastop(int tok) { 
    if(tok > TOK_EOF && tok <= TOK_MOD) return tok;
    fatals("Syntax error, token", Tstring[tok]);
    return 0; // will never be reached
}

static int rightassoc(int tokentype) {
    if(tokentype >= TOK_ASSIGN && tokentype <= TOK_ASSLASH) return 1;
    return 0;
}

static int OpPrec[] = {
    0, 10, 10,          // EOF, ASSIGN, ASPLUS
    10, 10,             // ASMINUS, ASSSTAR
    10, 10,             // ASSLASH, ASMOD
    15,                 // QUESTION (TERNARY)
    20, 30,             // LOGOR, LOGAND,
    40, 50, 60,         // OR, XOR, AMPER
    70, 70,             // EQ, NE
    80, 80, 80, 80,     // LT, GT, LE, GE
    90, 90,             // LSHIFT, RSHIFT
    100, 100,           // PLUS, MINUS
    110, 110, 110       // STAR, SLASH, MOD
};

static int opprecedence(int tokentype) {
    int prec;
    if(tokentype > TOK_MOD) fatald("Token with no precedence in opprecedence:", tokentype);
    prec = OpPrec[tokentype];
    if(prec == 0) {
        fatald("Syntax error, token", tokentype); 
    }
    return prec;
}

struct ASTNode *prefix(int ptp);

struct ASTNode *binexpr(int ptp) {
    struct ASTNode *left, *right;
    struct ASTNode *ltemp, *rtemp;
    int ASTop;
    int tokentype;

    // printf("binexpr(ptp): %s\n", Token.tokstr);

    left = prefix(ptp);

    tokentype = Token.token;
    if(tokentype == TOK_SEMI || tokentype == TOK_RPAREN || tokentype == TOK_RBRACKET || tokentype == TOK_COMMA || tokentype == TOK_COLON || tokentype == TOK_RBRACE) {
        // printf("quitting\n");
        left->rvalue = 1;
        return left;
    }

    while((opprecedence(tokentype) > ptp) || (rightassoc(tokentype) && opprecedence(tokentype) == ptp)) {
        // printf("binexpr start: %s\n", Token.tokstr);
        scan(&Token);

        right = binexpr(OpPrec[tokentype]);

        ASTop = binastop(tokentype);
        switch(ASTop) {
            case AST_TERNARY:
                match(TOK_COLON, ":");
                ltemp = binexpr(0);

                return mkastnode(AST_TERNARY, right->type, SIGN_UNSIGNED, right->ctype, left, right, ltemp, NULL, 0);

            case AST_ASSIGN:
                right->rvalue = 1;
        
                right = modify_type(right, left->type, left->ctype, 0);
                if(right == NULL) fatal("Incompatible expression in assignment");
                ltemp = left;
                left = right;
                right = ltemp;
                break;
            default:
                left->rvalue = 1;
                right->rvalue = 1;

                ltemp = modify_type(left, right->type, right->ctype, ASTop);
                rtemp = modify_type(right, left->type, left->ctype, ASTop);
                if(ltemp == NULL && rtemp == NULL) fatal("Incompatible types in binary expression");
                if(ltemp != NULL) left = ltemp;
                if(rtemp != NULL) right = rtemp;
        }

        left = mkastnode(binastop(tokentype), left->type, left->signage, left->ctype, left, NULL, right, NULL, 0);

        switch(binastop(tokentype)) {
            case AST_LOGOR:
            case AST_LOGAND:
            case AST_EQ:
            case AST_NE:
            case AST_LT:
            case AST_GT:
            case AST_LE:
            case AST_GE:
                left->type = PRIMITIVE_INT;
        }

        // printf("binexpr end: %s\n", Token.tokstr);

        tokentype = Token.token;
        if(tokentype == TOK_SEMI || tokentype == TOK_RPAREN || tokentype == TOK_RBRACKET || tokentype == TOK_COMMA || tokentype == TOK_COLON || tokentype == TOK_RBRACE) {
            left->rvalue = 1;
            return left;
        }
    }

    left->rvalue = 1;
    return left;
}

struct ASTNode *prefix(int ptp) {
    struct ASTNode *tree;
    switch(Token.token) {
        case TOK_AMPER:
            scan(&Token);
            tree = prefix(ptp);

            if(tree->op != AST_IDENT) fatal("& operator must be followed by an identifier");

            if(tree->sym->stype == SYM_ARRAY) fatal("& operator cannot be performed on an array");

            tree->op = AST_ADDR;
            tree->type = pointer_to(tree->type);
            break;
        case TOK_STAR:
            scan(&Token);
            tree = prefix(ptp);
            tree->rvalue = 1;

            if(!ptrtype(tree->type)) fatal("* dereference operator must be proceeded by a pointer type");
            tree = mkastunary(AST_DEREF, value_at(tree->type), SIGN_UNSIGNED, tree->ctype, tree, NULL, 0);
            break;
        case TOK_MINUS:
            scan(&Token);
            tree = prefix(ptp);

            tree->rvalue = 1;
            if(tree->type == PRIMITIVE_CHAR) tree->type = PRIMITIVE_INT;
            // tree = modify_type(tree, PRIMITIVE_INT, 0);
            tree = mkastunary(AST_NEGATE, tree->type, SIGN_SIGNED, tree->ctype, tree, NULL, 0);
            break;
        case TOK_INVERT:
            scan(&Token);
            tree = prefix(ptp);

            tree->rvalue = 1; 
            tree = mkastunary(AST_INVERT, tree->type, SIGN_SIGNED, tree->ctype, tree, NULL, 0);
            break;
        case TOK_LOGNOT:
            scan(&Token);
            tree = prefix(ptp);

            tree->rvalue = 1;
            tree = mkastunary(AST_LOGNOT, tree->type, SIGN_UNSIGNED, tree->ctype, tree, NULL, 0);
            break;
        case TOK_INC:
            scan(&Token);
            tree = prefix(ptp);

            if(tree->op != AST_IDENT) fatal("Prefix increment operator must precede an identifer");

            tree = mkastunary(AST_PREINC, tree->type, SIGN_SIGNED, tree->ctype, tree, NULL, 0);
            break;
        case TOK_DEC:
            scan(&Token);
            tree = prefix(ptp);

            if(tree->op != AST_IDENT) fatal("Prefix decrement operator must precede an identifer");

            tree = mkastunary(AST_PREDEC, tree->type, SIGN_SIGNED, tree->ctype, tree, NULL, 0);
            break;
        default:
            tree = postfix(ptp);
    }
    return tree;
}
