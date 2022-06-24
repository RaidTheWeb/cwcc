#ifndef extern_
    #define extern_ extern
#endif

#include <stdint.h>
#include <stdio.h>

#include "token.h"
#include "symbols.h"

// compile flags
extern_ int O_dumpAST;
extern_ int O_dumpsym;
extern_ int O_keepasm;
extern_ int O_assemble;
extern_ int O_verbose;
// #ifdef CWCC_AUDR32
extern_ uint32_t O_offset;
extern_ uint32_t O_size;
// #endif
#ifdef CWCC_X86_64
extern_ int O_dolink;
#endif

extern_ int Line;
extern_ int Linestart;
extern_ int Putback;
extern_ int Looplevel;
extern_ int Switchlevel;
extern_ FILE *Infile;
extern_ char *Infilename;
extern_ FILE *Outfile;
extern_ char *Outfilename;
extern_ struct token Token;
extern_ struct token Peektoken;
extern_ struct symtable *Functionid;
#define TEXTLEN 2048 // 2kb string max :flushed: (this will also be the max size of any symbol)
extern_ char Text[TEXTLEN + 1];
#define NSYMBOLS 2048
extern char *Tstring[]; // human readable token names

extern_ struct symtable *Globhead, *Globtail;       // Global variables/functions
extern_ struct symtable *Loclhead, *Locltail;       // Local variables
extern_ struct symtable *Parmhead, *Parmtail;       // Local parameters
extern_ struct symtable *Membhead, *Membtail;       // Temporary list of struct/union members
extern_ struct symtable *Structhead, *Structtail;   // List of structs
extern_ struct symtable *Unionhead, *Uniontail;     // List of unions
extern_ struct symtable *Enumhead, *Enumtail;       // List of enums
extern_ struct symtable *Typehead, *Typetail;       // List of typedefs

extern_ int Globs;
extern_ int Locls;
