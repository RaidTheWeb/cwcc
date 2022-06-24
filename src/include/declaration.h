#ifndef DECLARATION_H
#define DECLARATION_H

#include "ast.h"
#include "symbols.h"

int isnewsym(struct symtable *sym, int class, int type, struct symtable *ctype); // located in statements.c

int signageoftypedef(char *name);
int parse_cast(struct symtable **ctype, int signage);
int parse_stars(int type);
int parse_literal(int type);
int declaration_list(struct symtable **ctype, int class, int signage, int et1, int et2, struct ASTNode **gluetree);
void global_declarations(void);

#endif
