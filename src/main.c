#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "codegen.h"
#include "declaration.h"
#include "symbols.h"
#include "typing.h"
#include "token.h"
#define extern_
#include "data.h"
#undef extern_
#include <errno.h>

#include <unistd.h>

#define AOUT "a.out"
#define CPPCMD "cpp -nostdinc -isystem "
#ifndef INCDIR
#define INCDIR "" // funny
#endif
#ifndef ASCMD
#define ASCMD "" // funny
#endif

char *altersuffix(char *str, char suffix) {
    char *posn;
    char *newstr;

    if((newstr = strdup(str)) == NULL) return NULL;

    if((posn = strrchr(newstr, '.')) == NULL) return NULL;

    posn++;
    if(*posn == '\0') return NULL;

    *posn = suffix;
    posn++;
    *posn = '\0';
    return newstr;
}

static char *docompile(char *filename) {
    char cmd[TEXTLEN];

    Outfilename = altersuffix(filename, 's');
    if(Outfilename == NULL) {
        fprintf(stderr, "Error: %s has no suffix (file extension)\n", filename);
        exit(1);
    }

    snprintf(cmd, TEXTLEN, "%s %s %s", CPPCMD, INCDIR, filename);

    if((Infile = popen(cmd, "r")) == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    Infilename = filename;

    if((Outfile = fopen(Outfilename, "w")) == NULL) {
        fprintf(stderr, "Unable to create %s: %s\n", Outfilename, strerror(errno));
        exit(1);
    }

    Line = 1;
    Linestart = 1;
    Putback = '\n';
    clear_symtable(); // clear all symbols
    if(O_verbose) printf("Compiling %s\n", filename);
    scan(&Token);
    Peektoken.token = 0;
    genpreamble(filename);
    global_declarations();
    genpostamble();
    fclose(Outfile);

    if(O_dumpsym) {
        printf("Symbols for %s\n", filename);
        dumpsymtables();
        fprintf(stdout, "\n");
    }

    freestaticsyms();
    return Outfilename;
}

char *doassemble(char *filename, char *endfilename, char **files, size_t filenum, char **assemblyfiles, int assemblyfilesnum) {
    char cmd[TEXTLEN];
    int err;

    char *outfilename = altersuffix(filename, 'o');
    if(outfilename == NULL) {
        fprintf(stderr, "Error: %s has no suffix (file extension)\n", filename);
        exit(1);
    }

#ifdef CWCC_X86_64
    snprintf(cmd, TEXTLEN, "%s %s %s", ASCMD, outfilename, filename);
#else
    snprintf(cmd, TEXTLEN, "%s %s -b %08x -s %u", ASCMD, endfilename, O_offset, O_size); // assembler is the final step for audr32 binaries
    for(size_t i = 0; i < filenum; i++) {
        snprintf(cmd, TEXTLEN, "%s %s ", strdup(cmd), files[i]);
    }
    if(assemblyfilesnum) {
        for(size_t i = 0; i < assemblyfilesnum; i++) {
            snprintf(cmd, TEXTLEN, "%s %s ", strdup(cmd), assemblyfiles[i]);
        }
    }
#endif
    if(O_verbose) printf("%s\n", cmd);
    err = system(cmd);
    if(err != 0) {
        fprintf(stderr, "Assembly of %s failed\n", filename);
        exit(1);
    }

    return outfilename;
}

void do_link(char *outfilename, char *objlist[]) {
#ifdef CWCC_X86_64
    int cnt, size = TEXTLEN;
    char cmd[TEXTLEN], *cptr;
    int err;

    cptr = cmd;
    cnt = snprintf(cptr, size, "%s %s ", LDCMD, outfilename);
    cptr += cnt;
    size -= cnt;

    while(*objlist != NULL) {
        cnt = snprintf(cptr, size, "%s ", *objlist);
        cptr += cnt;
        size -= cnt;
        objlist++;
    }

    if(O_verbose) printf("%s\n", cmd);
    err = system(cmd);
    if(err != 0) {
        fprintf(stderr, "Linking failed\n");
        exit(1);
    }

#endif
}

static void usage(char *prog) {
#ifdef CWCC_X86_64
    fprintf(stderr, "Usage: %s [-vcSTM] [-o outfile] file [file ...]\n", prog);
#else
    fprintf(stderr, "Usage: %s [-vbsSTM] [[-a assemblyfile] ...] [-o outfile] file [file ...]\n", prog);
#endif
    fprintf(stderr, "       -v toggle verbose output\n");
    fprintf(stderr, "       -b base offset\n");
    fprintf(stderr, "       -s size, force base size for output binary\n");
    fprintf(stderr, "       -a assemblyfile, add an assembly file for compilation\n");
#ifdef CWCC_X86_64
    fprintf(stderr, "       -c generate object files only\n");
#endif
    fprintf(stderr, "       -S generate assembly files only\n");
    fprintf(stderr, "       -T dump the AST trees\n");
    fprintf(stderr, "       -M dump the symbol table\n");
    fprintf(stderr, "       -o outfile, select output name\n");
    exit(1);
}

#define MAXOBJ 100

int main(int argc, char **argv) {
    char *outfilename = AOUT;
    char *asmfile, *objfile;
    char *objlist[MAXOBJ];
    int i, j, objcnt = 0;
    char *assemblyfiles[32];
    int assemblyfileptr = 0;

    O_dumpAST = 0;
    O_dumpsym = 0;
    O_keepasm = 0;
#ifdef CWCC_X86_64
    O_assemble = 0;
#else
    O_offset = 0x40330000; // default to ROM
    O_size = 0; // default to dynamic
    O_assemble = 1;
#endif
    O_verbose = 0;
#ifdef CWCC_X86_64
    O_dolink = 1;
#endif

    for(i = 1; i < argc; i++) {
        if(*argv[i] != '-') break;

        for(j = 1; (*argv[i] == '-') && argv[i][j]; j++) {
            switch(argv[i][j]) {
                case 'o':
                    outfilename = argv[++i];
                    break;
                case 'a':
                    if(assemblyfileptr == 32) { 
                        exit(1);
                    }
                    assemblyfiles[assemblyfileptr++] = strdup(argv[++i]);
                    break;
                case 'b':
                    O_offset = strtol(argv[++i], NULL, 16); 
                    break;
                case 's':
                    O_size = strtol(argv[++i], NULL, 10);
                case 'T':
                    O_dumpAST = 1;
                    break;
                case 'M':
                    O_dumpsym = 1;
                    break;
#ifdef CWCC_X86_64
                case 'c':
                    O_assemble = 1;
                    O_keepasm = 0;
                    O_dolink = 0;
                    break;
#endif
                case 'S':
                    O_keepasm = 1;
                    O_assemble = 0;
#ifdef CWCC_X86_64
                    O_dolink = 0;
#endif
                    break;
                case 'v':
                    O_verbose = 1;
                    break;
                default:
                    usage(argv[0]);
            }
        }
    }

    if(i >= argc) usage(argv[0]);

    char *files[TEXTLEN];
    size_t fileptr;
    size_t filenum = 0;

    int initial = i;
    while(i < argc) {
        char *tempfilename = altersuffix(argv[i], 's');
        if(tempfilename == NULL) {
            fprintf(stderr, "Error: %s has no suffix (file extension)\n", argv[i]);
            exit(1);
        }
        files[fileptr++] = strdup(tempfilename);
        filenum++;
        i++;
    }

    i = initial;

#ifdef CWCC_X86_64

    while(i < argc) {
        asmfile = docompile(argv[i]);

        if(O_dolink || O_assemble) {
            objfile = doassemble(asmfile, outfilename, files, filenum);
            if(objcnt == (MAXOBJ - 2)) {
                fprintf(stderr, "Too many object files to handle\n");
                exit(1);
            }
            objlist[objcnt++] = objfile;
            objlist[objcnt] = NULL;
        }

        if(!O_keepasm) unlink(asmfile);
        i++;
    }
#else
    while(i < argc) {
        asmfile = docompile(argv[i]);
        i++;
    }
    if(O_assemble) {
        doassemble(asmfile, outfilename, files, filenum, assemblyfiles, assemblyfileptr);
    }
    if(!O_keepasm) {
        for(size_t k = 0; k < filenum; k++) {
            unlink(files[k]);
        }
    }
#endif

#ifdef CWCC_X86_64
    if(O_dolink) {
        do_link(outfilename, objlist);
        if(!O_assemble) {
            for(i = 0; objlist[i] != NULL; i++) unlink(objlist[i]);
        }
    }
#endif

    return 0;
}
//
//
// static void usage(char *prog) {
//     fprintf(stderr, "Usage: %s infile\n", prog);
//     exit(1);
// }
//
//
//
// int main(int argc, char **argv) {
//     char cmd[TEXTLEN];
//
//     struct ASTNode tree;
//     Line = 1;
//     Linestart = 1;
//     Putback = '\n';
//     Peektoken.token = 0;
//     clear_symtable(); // clear all symbols
//
//     if(argc != 2) usage(argv[0]);
//
//     snprintf(cmd, TEXTLEN, "%s %s %s", CPPCMD, INCDIR, argv[1]);
//
//     if((Infile = popen(cmd, "r")) == NULL) {
//         fprintf(stderr, "Unable to open: %s: %s\n", argv[1], strerror(errno));
//         return 1;
//     }
//     Infilename = argv[1];
//     Outfilename = "out.asm";
//
//     if((Outfile = fopen(Outfilename, "w")) == NULL) {
//         fprintf(stderr, "Unable to create out.asm: %s\n", strerror(errno));
//         return 1;
//     }
//
//     scan(&Token);
//     genpreamble(argv[1]);
//     global_declarations(); 
//     genpostamble();
//     fclose(Outfile);
//     // dumpsymtables();
//     freestaticsyms();
//     return 0;
// }
