#ifndef SYMBOLS_H
#define SYMBOLS_H

enum {
    CLASS_GLOBAL = 1,
    CLASS_LOCAL,
    CLASS_PARAM,
    CLASS_EXTERN,
    CLASS_STATIC,
    CLASS_STRUCT,
    CLASS_UNION,
    CLASS_MEMBER,
    CLASS_ENUMTYPE,
    CLASS_ENUMVAL,
    CLASS_TYPEDEF
};

enum {
    SYM_VAR, SYM_FUNC, SYM_ARRAY
};

struct symtable {
    char *name;                 // name (identifier)
    int type;                   // primitive type
    struct symtable *ctype; 
    int stype;                  // symbol type
    int class;                  // symbol class
    int signage;                // symbol signage (integer types)
    int nelems;                 // number of parameters
    int size;                   // negative offset from stack base pointer
#define st_endlabel st_posn     // end label
    int st_posn;                // negative offset (local variables)
    int *initlist;              // Initial values
    struct symtable *next;      // next symbol
    struct symtable *member;    // first parameter of function
};

void appendsym(struct symtable **head, struct symtable **tail, struct symtable *node);
struct symtable *newsym(char *name, int type, struct symtable *ctype, int stype, int class, int signage, int size, int posn);
struct symtable *findglob(char *s);
struct symtable *findlocl(char *s);
struct symtable *findsymbol(char *s);
struct symtable *findmember(char *s);
struct symtable *findunion(char *s);
struct symtable *findstruct(char *s);
struct symtable *findenumtype(char *s);
struct symtable *findenumval(char *s);
struct symtable *findtypedef(char *s);
struct symtable *addglob(char *name, int type, struct symtable *ctype, int stype, int class, int signage, int nelems, int posn);
struct symtable *addlocl(char *name, int type, struct symtable *ctype, int stype, int signage, int nelems);
struct symtable *addparm(char *name, int type, struct symtable *ctype, int stype, int signage);
struct symtable *addmemb(char *name, int type, struct symtable *ctype, int stype, int sinage, int nelems);
struct symtable *addunion(char *name);
struct symtable *addstruct(char *name);
struct symtable *addenum(char *name, int class, int value);
struct symtable *addtypedef(char *name, int type, struct symtable *ctype, int signage);
void freeloclsyms(void);
void freestaticsyms(void);
void copyfuncparams(int slot);
void clear_symtable(void);
void dumptable(struct symtable *head, char *name, int indent);
void dumpsymtables(void);

#endif
