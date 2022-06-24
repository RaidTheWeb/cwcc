#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "token.h"
#include "misc.h"

char *Tstring[] = {
    "EOF", "=",  "+=", "-=", "*=", "/=", "%=",
    "?", "||", "&&", "|", "^", "&",
    "==", "!=", ",", ">", "<=", ">=", "<<", ">>",
    "+", "-", "*", "/", "%",
    "++", "--", "~", "!",
    "void", "char", "int", "long",
    "if", "else", "while", "for", "return",
    "struct", "union", "enum", "typedef",
    "extern", "break", "continue", "case",
    "default", "switch", "sizeof", "static",
    "asmblock",
    "intlit", "strlit", ";", "identifier",
    "{", "}", "(", ")", "[", "]", ",", ".",
    "->", ":"
};

static int chrpos(char *s, int c) {
    char *p;

    p = strchr(s, c);
    return p ? p - s : -1;
}

static int next(void) {
    int c, l;

    if(Putback) {
        c = Putback;
        Putback = 0;
        return c;
    }

    c = fgetc(Infile);

    while(Linestart && c == '#') {
        Linestart = 0;
        scan(&Token);
        if(Token.token != TOK_INTLIT) fatals("Expected preprocessor line number, got:", Text);
        l = Token.intvalue;

        scan(&Token);
        if(Token.token != TOK_STRLIT) fatals("Expected preprocessor filename, got:", Text);

        if(Text[0] != '<') {
            if(strcmp(Text, Infilename)) Infilename = strdup(Text); // Only update filename if it's not the same
            Line = l;
        }

        while((c = fgetc(Infile)) != '\n');
        c = fgetc(Infile);
        Linestart = 1;
    }

    Linestart = 0;
    if(c == '\n') {
        Line++;
        Linestart = 1;
    }
    return c;
}

static void putback(int c) {
    Putback = c;
}

static int skip(void) {
    int c;

    c = next();
    while(c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f') c = next();
    return c;
}

static struct token *Rejtoken = NULL;

void rejecttoken(struct token *t) {
    if(Rejtoken != NULL) fatal("Can't reject token twice");
    Rejtoken = t;
}

static int scanint(int c) {
    int k, val = 0, radix = 10;

    if (c == '0') {
        if ((c = next()) == 'x') {
            radix = 16;
            c = next();
        } else radix = 8;
    }

    while ((k = chrpos("0123456789abcdef", tolower(c))) >= 0) {
        if (k >= radix) fatalc("invalid digit in integer literal", c);
        val = val * radix + k;
        c = next();
    }

    putback(c);
    return (val);
}

static int scanident(int c, char *buf, int lim) {
    int i = 0;

    while(isalpha(c) || isdigit(c) || c == '_') {
        if(i == lim - 1) {
            printf("Identifier too long on line %d\n", Line);
            exit(1);
        } else if(i < lim - 1) {
            buf[i++] = c;
        }

        c = next();
    }

    putback(c);
    buf[i] = '\0';
    return i;
}

static int hexchar(void) {
    int c, h, n = 0, f = 0;


    while(isxdigit(c = next())) {
        h = chrpos("0123456789abcdef", tolower(c));
        n = n * 16 + h;
        f = 1;
    }

    putback(c);

    if(!f) fatal("Missing digits after '\\x'");
    if(n > 255) fatal("Value out of range after '\\x'");
    return n;
}

static int scanch(void) {
    int i, c, c2;

    c = next();
    if(c == '\\') {
        switch(c = next()) {
            case 'a': return '\a';
            case 'b': return '\b';
            case 'f': return '\f';
            case 'n': return '\n';
            case 'r': return '\r';
            case 't': return '\t';
            case 'v': return '\v';
            case '\\': return '\\';
            case '"': return '"';
            case '\'': return '\'';
            case '0':
                if(!isdigit(c = next())) {
                    putback(c);
                    return '\0';
                }
                putback(c);
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                for(i = c2 = 0; isdigit(c) && c < '8'; c = next()) {
                    if(++i > 3) break;
                    c2 = c2 * 8 + (c = '0');
                }
                putback(c);
                return c2;
            case 'x': return hexchar();
            default: fatalc("unknown escape sequence", c);
        }
    }

    return c;
}

static int scanstr(char *buf) {
    int i, c;

    for(i = 0; i < TEXTLEN - 1; i++) {
        if((c = scanch()) == '"') {
            buf[i] = 0;
            return i;
        }
        buf[i] = c;
    }

    fprintf(stderr, "String literal of length %d exceeds max length of %d\n", i + 1, TEXTLEN);
    exit(1);
    return 0;
}

static int scanasm(char *buf) {
    int i;
    char c;

    for(i = 0; i < TEXTLEN - 1; i++) { 
        if((c = next()) == ']') {
            buf[i] = 0;
            printf("%s", buf);
            return i;
        }
        buf[i] = c;
    }
    fprintf(stderr, "Assembly block of length %d exceeds max length of %d\n", i + 1, TEXTLEN);
    exit(1);
    return 0;
}

static int keyword(char *s) {
    switch(*s) {
        case 'b':
            if(!strcmp(s, "break")) return TOK_BREAK;
            break;
        case 'c':
            if(!strcmp(s, "case")) return TOK_CASE;
            if(!strcmp(s, "char")) return TOK_CHAR;
            if(!strcmp(s, "continue")) return TOK_CONTINUE;
            break;
        case 'd':
            if(!strcmp(s, "default")) return TOK_DEFAULT;
            break;
        case 'e':
            if(!strcmp(s, "else")) return TOK_ELSE;
            if(!strcmp(s, "enum")) return TOK_ENUM;
            if(!strcmp(s, "extern")) return TOK_EXTERN;
            break;
        case 'f':
            if(!strcmp(s, "for")) return TOK_FOR;
            break;
        case 'i':
            if(!strcmp(s, "if")) return TOK_IF;
            if(!strcmp(s, "int")) return TOK_INT;
            break;
        case 'l':
            if(!strcmp(s, "long")) return TOK_LONG;
        case 'r':
            if(!strcmp(s, "return")) return TOK_RETURN;
            break;
        case 's':
            if(!strcmp(s, "short")) return TOK_SHORT;
            if(!strcmp(s, "signed")) return TOK_SIGNED;
            if(!strcmp(s, "sizeof")) return TOK_SIZEOF;
            if(!strcmp(s, "static")) return TOK_STATIC;
            if(!strcmp(s, "struct")) return TOK_STRUCT;
            if(!strcmp(s, "switch")) return TOK_SWITCH;
            break;
        case 't':
            if(!strcmp(s, "typedef")) return TOK_TYPEDEF;
            break;
        case 'u':
            if(!strcmp(s, "union")) return TOK_UNION;
            if(!strcmp(s, "unsigned")) return TOK_UNSIGNED;
            break;
        case 'v':
            if(!strcmp(s, "void")) return TOK_VOID;
            break;
        case 'w':
            if(!strcmp(s, "while")) return TOK_WHILE;
            break;
    }

    return 0;
}

int scan(struct token *t) {
    int c, tokentype;

    if(Peektoken.token != 0) {
        t->token = Peektoken.token;
        t->tokstr = Peektoken.tokstr;
        t->intvalue = Peektoken.intvalue;
        Peektoken.token = 0;
        return 1;
    }

    c = skip();


    switch(c) {
        case EOF:
            t->token = TOK_EOF;
            return 0;
        case '+':
            if((c = next()) == '+') {
                t->token = TOK_INC;
            } else if(c == '=') {
                t->token = TOK_ASPLUS;
            } else {
                putback(c);
                t->token = TOK_PLUS;
            }
            break;
        case '-':
            if((c = next()) == '-') {
                t->token = TOK_DEC;            
            } else if(c == '>') {
                t->token = TOK_ARROW;
            } else if(c == '=') {
                t->token = TOK_ASMINUS;
            } else if(isdigit(c)) {
                t->intvalue = -scanint(c);
                t->token = TOK_INTLIT;
            } else {
                putback(c);
                t->token = TOK_MINUS;
            }
            break;
        case '*':
            if((c = next()) == '=') {
                t->token = TOK_ASSTAR;
            } else {
                putback(c);
                t->token = TOK_STAR;
            }
            break;
        case '/':
            if((c = next()) == '=') {
                t->token = TOK_ASSLASH;
            } else {
                putback(c);
                t->token = TOK_SLASH;
            }
            break;
        case '%': 
            if((c = next()) == '=') {
                t->token = TOK_ASMOD; 
            } else {
                putback(c);
                t->token = TOK_MOD;
            }
            break;
        case ';':
            t->token = TOK_SEMI;
            break;
        case ':':
            if((c = next()) == '[') {
                scanasm(Text);
                t->token = TOK_ASM;
            } else {
                putback(c);
                t->token = TOK_COLON;
            }
            break;
        case '{':
            t->token = TOK_LBRACE;
            break;
        case '}':
            t->token = TOK_RBRACE;
            break;
        case '(':
            t->token = TOK_LPAREN;
            break;
        case ')':
            t->token = TOK_RPAREN;
            break;
        case '=':
            if((c = next()) == '=') {
                t->token = TOK_EQ;
            } else {
                putback(c);
                t->token = TOK_ASSIGN;
            }
            break;
        case '!':
            if((c = next()) == '=') {
                t->token = TOK_NE;
            } else {
                putback(c);
                t->token = TOK_LOGNOT; 
            }
            break;
        case '<':
            if((c = next()) == '=') {
                t->token = TOK_LE;
            } else if(c == '<') {
                t->token = TOK_LSHIFT;
            } else {
                putback(c);
                t->token = TOK_LT;
            }
            break;
        case '>':
            if((c = next()) == '=') {
                t->token = TOK_GE;
            } else if(c == '>') {
                t->token = TOK_RSHIFT;
            } else {
                putback(c);
                t->token = TOK_GT;
            }
            break;
        case '&':
            if((c = next()) == '&') {
                t->token = TOK_LOGAND;
            } else {
                putback(c);
                t->token = TOK_AMPER;
            }
            break;
        case ',':
            t->token = TOK_COMMA;
            break;
        case '.':
            t->token = TOK_DOT;
            break;
        case '[':
            t->token = TOK_LBRACKET;
            break;
        case ']':
            t->token = TOK_RBRACKET;
            break;
        case '\'':
            t->intvalue = scanch();
            t->token = TOK_INTLIT;
            if(next() != '\'') fatal("Expected '\\'' delimiting character literal");
            break;
        case '"':
            scanstr(Text);
            t->token = TOK_STRLIT;
            break;
        case '|':
            if((c = next()) == '|') {
                t->token = TOK_LOGOR;
            } else {
                putback(c);
                t->token = TOK_OR;
            }
            break;
        case '^':
            t->token = TOK_XOR;
            break;
        case '~':
            t->token = TOK_INVERT;
            break;
        case '?':
            t->token = TOK_QUESTION;
            break;
        default:
            if(isdigit(c)) { 
                t->intvalue = scanint(c);
                t->token = TOK_INTLIT;
                break;
            } else if(isalpha(c) || c == '_') {
                scanident(c, Text, TEXTLEN);

                if((tokentype = keyword(Text)) != 0) {
                    t->token = tokentype;
                    break;
                }

                t->token = TOK_IDENT;
                break; 
            }

            fatalc("Unrecognised character", c); 
    }

    t->tokstr = Tstring[t->token];

    // printf("tok: %s\n", t->tokstr);
    return 1;
}
