#include <stddef.h>
#include <string.h>

#include "ast.h"
#include "codegen.h"
#include "data.h"
#include "declaration.h"
#include "token.h"
#include "typing.h"
#include "misc.h"

struct ASTNode *if_statement(void);
struct ASTNode *while_statement(void);
struct ASTNode *for_statement(void);
struct ASTNode *return_statement(void);
struct ASTNode *break_statement(void);
struct ASTNode *continue_statement(void);
struct ASTNode *switch_statement(void);

int isnewsym(struct symtable *sym, int class, int type, struct symtable *ctype) {
    if(sym == NULL) return 1;

    if((sym->class == CLASS_GLOBAL && class == CLASS_EXTERN) || (sym->class == CLASS_EXTERN && class == CLASS_GLOBAL)) {
        if(type != sym->type) fatals("Type mismatch between global and extern declarations", sym->name);

        if(type >= PRIMITIVE_STRUCT && ctype != sym->ctype) fatals("Type mismatch between global and extern declarations", sym->name);

        sym->class = CLASS_GLOBAL;
        return 0;
    }

    fatals("Duplicate global variable declaration", sym->name);
    return -1; // will never be reached
}

static struct ASTNode *single_statement(void) { 
    struct ASTNode *stmt;
    struct symtable *ctype;
    int linenum = Line;

    switch(Token.token) {
        case TOK_SEMI:
            semi();
            break;
        case TOK_LBRACE:
            lbrace();
            stmt = compound_statement(0);
            stmt->linenum = Line;
            rbrace();
            return stmt;
        case TOK_IDENT:
            if(findtypedef(Text) == NULL) {
                stmt = binexpr(0);
                stmt->linenum = linenum;
                semi();
                return stmt;
            }
        case TOK_CHAR:
        case TOK_SHORT:
        case TOK_INT:
        case TOK_LONG:
        case TOK_STRUCT:
        case TOK_UNION:
        case TOK_ENUM:
        case TOK_TYPEDEF:
            declaration_list(&ctype, CLASS_LOCAL, -1, TOK_SEMI, TOK_EOF, &stmt);
            semi();
            return stmt; 
        case TOK_IF:
            stmt = if_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_WHILE:
            stmt = while_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_FOR:
            stmt = for_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_RETURN:
            stmt = return_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_BREAK:
            stmt = break_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_CONTINUE:
            stmt = continue_statement();
            stmt->linenum = linenum;
            return stmt;
        case TOK_SWITCH:
            stmt = switch_statement();
            stmt->linenum = linenum;
            return stmt;
        default: 
            stmt = binexpr(0);
            stmt->linenum = linenum;
            semi();
            return stmt;
    }
    
    return NULL; // will never be reached
    
}

struct ASTNode *if_statement(void) {
    struct ASTNode *condAST, *trueAST, *falseAST = NULL;

    match(TOK_IF, "if");
    lparen();

    condAST = binexpr(0);

    if(condAST->op < AST_EQ || condAST->op > AST_GE)
        condAST = mkastunary(AST_TOBOOL, condAST->type, SIGN_UNSIGNED, condAST->ctype, condAST, NULL, 0);
    rparen();

    trueAST = single_statement();

    if(Token.token == TOK_ELSE) {
        scan(&Token);
        falseAST = single_statement();
    }

    return mkastnode(AST_IF, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, condAST, trueAST, falseAST, NULL, 0);
}

struct ASTNode *while_statement(void) {
    struct ASTNode *condAST, *bodyAST;

    match(TOK_WHILE, "while");
    lparen();

    condAST = binexpr(0);
    if(condAST->op < AST_EQ || condAST->op > AST_GE)
        condAST = mkastunary(AST_TOBOOL, condAST->type, SIGN_UNSIGNED, condAST->ctype, condAST, NULL, 0);
    rparen();

    Looplevel++;
    bodyAST = single_statement();
    Looplevel--;

    return mkastnode(AST_WHILE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, condAST, NULL, bodyAST, NULL, 0);
}

struct ASTNode *return_statement(void) {
    struct ASTNode *tree = NULL;

    match(TOK_RETURN, "return");
//    lparen();

    if(Token.token == TOK_SEMI) {
        if(Functionid->type != PRIMITIVE_VOID) fatal("Must return from a non-void function");
    } else {
        if(Functionid->type == PRIMITIVE_VOID) fatal("Cannot return a value from a non-void function");
        tree = binexpr(0);
        tree = modify_type(tree, Functionid->type, Functionid->ctype, 0);
        if(tree == NULL) fatal("Type incompatible with function return type");
    }

    tree = mkastunary(AST_RETURN, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, tree, NULL, 0);
//    rparen();
    semi();
    return tree;
}

struct ASTNode *break_statement(void) {
    if(Looplevel == 0 && Switchlevel == 0) fatal("No loop/switch to break out of");
    scan(&Token);
    semi();
    return mkastleaf(AST_BREAK, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, NULL, 0);
}

struct ASTNode *continue_statement(void) {
    if(Looplevel == 0) fatal("No loop to continue to");
    scan(&Token);
    semi();
    return mkastleaf(AST_CONTINUE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, NULL, 0);
}

struct ASTNode *switch_statement(void) {
    struct ASTNode *left, *body, *n, *c, *casetree = NULL, *casetail;
    int inloop = 1, casecount = 0;
    int seendefault = 0;
    int ASTop, casevalue;

    scan(&Token);
    lparen();

    left = binexpr(0);
    rparen();
    lbrace();

    if(!inttype(left->type)) fatal("Switch expression must be of integer type");

    n = mkastunary(AST_SWITCH, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, left, NULL, 0);

    Switchlevel++;
    while(inloop) {
        switch(Token.token) {
            case TOK_RBRACE:
                if(casecount == 0) fatal("Empty body for switch case");
                inloop = 0;
                break;
            case TOK_CASE:
            case TOK_DEFAULT:
                if(seendefault) fatal("Case or redeclaration of default after existing default");
                if(Token.token == TOK_DEFAULT) {
                    ASTop = AST_DEFAULT;
                    seendefault = 1;
                    scan(&Token);
                } else {
                    ASTop = AST_CASE;
                    scan(&Token);
                    left = binexpr(0);

                    if(left->op != AST_INTLIT) fatal("Expecting integer literal for case value");
                    casevalue = left->a_intvalue;

                    for(c = casetree; c != NULL; c = c->right)
                        if(casevalue == c->a_intvalue) fatal("Duplicate case value");
                }
                match(TOK_COLON, ":"); 
                casecount++;

                if(Token.token == TOK_CASE || Token.token == TOK_DEFAULT) body = NULL;
                else body = compound_statement(1);

                if(casetree == NULL) {
                    casetree = casetail = mkastunary(ASTop, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, body, NULL, casevalue);
                } else {
                    casetail->right = mkastunary(ASTop, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, body, NULL, casevalue);
                    casetail = casetail->right;
                }
                break;
            default: fatals("Unexpected token in switch statement", Token.tokstr);
        }
    }

    Switchlevel--;

    n->a_intvalue = casecount;
    n->right = casetree;
    rbrace();

    return n;
}

struct ASTNode *for_statement(void) {
    struct ASTNode *condAST, *bodyAST;
    struct ASTNode *preopAST, *postopAST;
    struct ASTNode *tree;

    match(TOK_FOR, "for");
    lparen();

    preopAST = expressionlist(TOK_SEMI);
    semi();

    condAST = binexpr(0);

    if(condAST->op < AST_EQ || condAST->op > AST_GE)
        condAST = mkastunary(AST_TOBOOL, condAST->type, SIGN_UNSIGNED, condAST->ctype, condAST, NULL, 0);
    semi();

    postopAST = expressionlist(TOK_RPAREN);
    rparen();

    Looplevel++;
    bodyAST = single_statement();
    Looplevel--;

    tree = mkastnode(AST_GLUE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, bodyAST, NULL, postopAST, NULL, 0);

    tree = mkastnode(AST_WHILE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, condAST, NULL, tree, NULL, 0);

    return mkastnode(AST_GLUE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, preopAST, NULL, tree, NULL, 0);
}




struct ASTNode *compound_statement(int inswitch) {
    struct ASTNode *left = NULL;
    struct ASTNode *tree; 

    while(1) {
        if(Token.token == TOK_RBRACE) return left;
        if(inswitch && (Token.token == TOK_CASE || Token.token == TOK_DEFAULT)) return left;

        tree = single_statement();
 

        if(tree != NULL) {
            if(left == NULL) left = tree;
            else left = mkastnode(AST_GLUE, PRIMITIVE_NONE, SIGN_UNSIGNED, NULL, left, NULL, tree, NULL, 0);
        } 

        
    }

    return NULL;
}
