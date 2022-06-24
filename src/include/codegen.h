#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

struct ASTNode *if_statement(void);
struct ASTNode *compound_statement(int inswitch);

void generatecode(struct ASTNode *n);

void genfreeregs(int keepreg);
void genpreamble(char *filename);
void genpostamble(void);
void genglobsym(struct symtable *node);
int genprimsize(int type);
int genlabel(void);
int genglobstr(char *str, int append, int *Lend);
void genglobstrend(int l);
int gengetlocaloffset(int type);
int genalign(int type, int offset, int direction);
int genAST(struct ASTNode *n, int iflabel, int looptoplabel, int loopendlabel, int parentASTop);

// AUDR32
void freeallregisters(int keepreg);
void freeregister(int reg);
void cgpreamble(char *filename);
void cgpostamble(void);
void funcpreamble(struct symtable *sym);
void funcpostamble(struct symtable *sym);
int cgloadint(int value, int type);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgand(int r1, int r2);
int cgor(int r1, int r2);
int cgxor(int r1, int r2);
int cgshl(int r1, int r2);
int cgshr(int r1, int r2);
int cgnegate(int r);
int cginvert(int r);
int cglognot(int r);
int cgcompareset(int ASTop, int r1, int r2, int type);
int cgcomparejmp(int ASTop, int r1, int r2, int label, int type);
int cgloadvar(struct symtable *sym, int op);
int cgstorederef(int r1, int r2, int type);
int cgwiden(int r, int oldtype, int newtype);
int cgderef(int r, int type);
int allocregister(void);
void cgjmp(int l);
void cgmove(int r1, int r2);
void cgswitch(int reg, int casecount, int toplabel, int *caselabel, int *caseval, int defaultlabel);
void spillallregs(void);
void cgcopyarg(int r, int argposn);
int cgboolean(int r, int op, int label);
void cgloadboolean(int r, int val);
void cgfuncpreamble(struct symtable *sym);
void cgfuncpostamble(struct symtable *sym);
int cgdivmod(int r1, int r2, int op, int signage);
int cgshlconst(int r, int val);
int cgcall(struct symtable *sym, int numargs);
int cgstoreglob(int r, struct symtable *sym);
int cgstorelocal(int r, struct symtable *sym);
void cgglobsym(struct symtable *sym);
int cgprimsize(int type);
void cglabel(int l);
void cgglobstr(int l, char *str, int append, int *Lend);
void cgglobstrend(int l);
int cgloadglobstr(int label);
int cggetlocaloffset(int type);
void cgresetlocals(void);
void cgreturn(int reg, struct symtable *sym);
int cgaddress(struct symtable *sym);
int cgalign(int type, int offset, int direction);
void cgenumline(int line);
void cginlineasm(char *data);

#endif
