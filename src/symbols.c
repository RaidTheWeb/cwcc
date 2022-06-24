#include <string.h>
#include <stdlib.h>

#include "codegen.h"
#include "data.h"
#include "misc.h"
#include "typing.h"

void appendsym(struct symtable **head, struct symtable **tail, struct symtable *node) {
    if(head == NULL || tail == NULL || node == NULL) fatal("Either head, tail or node is NULL in appendsym()");

    if(*tail) {
        (*tail)->next = node;
        *tail = node;
    } else *head = *tail = node;
    node->next = NULL;
}

struct symtable *newsym(char *name, int type, struct symtable *ctype, int stype, int class, int signage, int nelems, int posn) {
    struct symtable *node = (struct symtable *)malloc(sizeof(struct symtable));
    if(node == NULL) fatal("Failed to malloc symbol table node in newsym()");

    if(name == NULL) node->name = NULL;
    else node->name = strdup(name);
    node->type = type;
    node->ctype = ctype;
    node->stype = stype;
    node->class = class;
    node->signage = signage;
    node->nelems = nelems;

    if(ptrtype(type) || inttype(type)) node->size = nelems * typesize(type, ctype);

    node->st_posn = posn;
    node->next = NULL;
    node->member = NULL;
    node->initlist = NULL; 
    return node;
}

struct symtable *addglob(char *name, int type, struct symtable *ctype, int stype, int class, int signage, int nelems, int posn) {
    struct symtable *sym = newsym(name, type, ctype, stype, class, signage, nelems, posn);
    if(type == PRIMITIVE_STRUCT || type == PRIMITIVE_UNION)
        sym->size = ctype->size;
    appendsym(&Globhead, &Globtail, sym);
    return sym;
}

struct symtable *addlocl(char *name, int type, struct symtable *ctype, int stype, int signage, int nelems) {
    struct symtable *sym = newsym(name, type, ctype, stype, CLASS_LOCAL, signage, nelems, 0);
    if(type == PRIMITIVE_STRUCT || type == PRIMITIVE_UNION)
        sym->size = ctype->size;
    appendsym(&Loclhead, &Locltail, sym);
    return sym;
}

struct symtable *addparm(char *name, int type, struct symtable *ctype, int stype, int signage) {
    struct symtable *sym = newsym(name, type, ctype, stype, CLASS_PARAM, signage, 1, 0);
    appendsym(&Parmhead, &Parmtail, sym);
    return sym;
}

struct symtable *addmemb(char *name, int type, struct symtable *ctype, int stype, int signage, int nelems) {
    struct symtable *sym = newsym(name, type, ctype, stype, CLASS_MEMBER, signage, nelems, 0);
    if(type == PRIMITIVE_STRUCT || type == PRIMITIVE_UNION)
        sym->size = ctype->size;
    appendsym(&Membhead, &Membtail, sym);
    return sym;
}

struct symtable *addunion(char *name) {
    struct symtable *sym = newsym(name, PRIMITIVE_UNION, NULL, 0, CLASS_UNION, SIGN_UNSIGNED, 0, 0);
    appendsym(&Unionhead, &Uniontail, sym);
    return sym;
}

struct symtable *addstruct(char *name) {
    struct symtable *sym = newsym(name, PRIMITIVE_STRUCT, NULL, 0, CLASS_STRUCT, SIGN_UNSIGNED, 0, 0);
    appendsym(&Structhead, &Structtail, sym);
    return sym;
}

struct symtable *addenum(char *name, int class, int value) {
    struct symtable *sym = newsym(name, PRIMITIVE_INT, NULL, 0, class, SIGN_SIGNED, 0, value);
    appendsym(&Enumhead, &Enumtail, sym);
    return sym;
}

struct symtable *addtypedef(char *name, int type, struct symtable *ctype, int signage) {
    struct symtable *sym = newsym(name, type, ctype, 0, CLASS_TYPEDEF, signage, 0, 0);
    appendsym(&Typehead, &Typetail, sym);
    return sym;
}

static struct symtable *findsyminlist(char *s, struct symtable *list, int class) {
    for(; list != NULL; list = list->next)
        if((list->name != NULL) && !strcmp(s, list->name))
            if(class == 0 || class == list->class) return list; 

    return NULL;
}

struct symtable *findglob(char *s) {
    return findsyminlist(s, Globhead, 0);
}

struct symtable *findlocl(char *s) {
    struct symtable *node;

    if(Functionid) {
        node = findsyminlist(s, Functionid->member, 0);
        if(node) return node;
    }
    return findsyminlist(s, Loclhead, 0);
}

struct symtable *findsymbol(char *s) {
    struct symtable *node;

    if(Functionid) {
        node = findsyminlist(s, Functionid->member, 0);
        if(node) return node;
    }

    node = findsyminlist(s, Loclhead, 0);
    if(node) return node;
    return findsyminlist(s, Globhead, 0);
}

struct symtable *findmember(char *s) {
    return findsyminlist(s, Membhead, 0);
}

struct symtable *findunion(char *s) {
    return findsyminlist(s, Unionhead, 0);
}

struct symtable *findstruct(char *s) {
    return findsyminlist(s, Structhead, 0);
}

struct symtable *findenumtype(char *s) {
    return findsyminlist(s, Enumhead, CLASS_ENUMTYPE);
}

struct symtable *findenumval(char *s) {
    return findsyminlist(s, Enumhead, CLASS_ENUMVAL);
}

struct symtable *findtypedef(char *s) {
    return findsyminlist(s, Typehead, 0);
}

void freeloclsyms(void) {
    Loclhead = Locltail = NULL;
    Parmhead = Parmtail = NULL;
    Functionid = NULL;
}

void freestaticsyms(void) {
    struct symtable *g, *prev = NULL;

    for(g = Globhead; g != NULL; g = g->next) {
        if(g->class == CLASS_STATIC) {
            if(prev != NULL) prev->next = g->next;
            else Globhead->next = g->next;

            if(g == Globtail) {
                if(prev != NULL) Globtail = prev;
                else Globtail = Globhead;
            }
        }
    }

    prev = g;
}

void clear_symtable(void) {
    Globhead = Globtail = NULL;
    Loclhead = Locltail = NULL;
    Parmhead = Parmtail = NULL;
    Membhead = Membtail = NULL;
    Structhead = Structtail = NULL;
    Unionhead = Uniontail = NULL;
    Enumhead = Enumtail = NULL;
    Typehead = Typetail = NULL;
}

static void dumpsym(struct symtable *sym, int indent) {
    int i;


    for(i = 0; i < indent; i++) printf(" ");
    switch(sym->signage) {
        case SIGN_SIGNED:
            printf("signed ");
            break;
        case SIGN_UNSIGNED:
            printf("unsigned ");
            break;
        // default: printf("unknown signage ");
    }
    switch(sym->type & (~0xf)) {
        case PRIMITIVE_VOID:
            printf("void ");
            break;
        case PRIMITIVE_CHAR:
            printf("char ");
            break;
        case PRIMITIVE_SHORT:
            printf("short ");
            break;
        case PRIMITIVE_INT:
            printf("int ");
            break;
        case PRIMITIVE_LONG:
            printf("long ");
            break;
        case PRIMITIVE_STRUCT:
            if(sym->ctype != NULL) printf("struct %s ", sym->ctype->name);
            else printf("struct %s ", sym->name);
            break;
        case PRIMITIVE_UNION:
            if(sym->ctype != NULL) printf("union %s ", sym->ctype->name);
            else printf("union %s ", sym->name);
            break;
        default: printf("unknown type ");
    }

    for(i = 0; i < (sym->type & 0xf); i++) printf("*");
    printf("%s", sym->name);

    switch(sym->stype) {
        case SYM_VAR: break;
        case SYM_FUNC:
            printf("()");
            break;
        case SYM_ARRAY:
            printf("[]");
            break;
        default:
            printf(" unknown stype");
    }

    switch(sym->class) {
        case CLASS_GLOBAL:
            printf(": global");
            break;
        case CLASS_LOCAL:
            printf(": local");
            break;
        case CLASS_PARAM:
            printf(": parameter");
            break;
        case CLASS_EXTERN:
            printf(": extern");
            break;
        case CLASS_STATIC:
            printf(": static");
            break;
        case CLASS_STRUCT:
            printf(": struct");
            break;
        case CLASS_UNION:
            printf(": union");
            break;
        case CLASS_MEMBER:
            printf(": member");
            break;
        case CLASS_ENUMTYPE:
            printf(": enumtype");
            break;
        case CLASS_ENUMVAL:
            printf(": enumval");
            break;
        case CLASS_TYPEDEF:
            printf(": typedef");
            break;
        default: printf(": unknown class");
    }

    switch(sym->stype) {
        case SYM_VAR:
            if(sym->class == CLASS_ENUMVAL) printf(", value %d\n", sym->st_posn);
            else printf(", size %d\n", sym->size);
            break;
        case SYM_FUNC:
            printf(", %d parameters\n", sym->nelems);
            break;
        case SYM_ARRAY:
            printf(", %d elements, size %d\n", sym->nelems, sym->size);
            break;
    }

    switch(sym->stype & (~0xf)) {
        case PRIMITIVE_STRUCT:
        case PRIMITIVE_UNION:
            dumptable(sym->member, NULL, 4);
    }

    switch(sym->stype) {
        case SYM_FUNC: dumptable(sym->member, NULL, 4);
    }
}

void dumptable(struct symtable *head, char *name, int indent) {
    struct symtable *sym;

    if(head != NULL && name != NULL) printf("%s\n---------\n", name);
    for(sym = head; sym != NULL; sym = sym->next) dumpsym(sym, indent);
}

void dumpsymtables(void) {
    dumptable(Globhead, "Globals", 0);
    printf("\n");
    dumptable(Enumhead, "Enums", 0);
    printf("\n");
    dumptable(Typehead, "Typedefs", 0);
}
