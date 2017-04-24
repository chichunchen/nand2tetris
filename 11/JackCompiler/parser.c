#include "helper.h"
#include "constant.h"

/* Utilities for parser */

char* setFileName(const char *arg)
{
    char *filename = malloc(sizeof(char) * MAXFILE);
    strcpy(filename, arg);
    char *ptr = filename;
    while(*ptr++ != '.') ;
    strcpy(ptr, "vm");
    return filename;
}

int is_directory(const char *szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/* Parser code */

int has_more_token(FILE *fp)
{
    if (putback) {
        return 1;
    }

    int c;

    while((c = fgetc(fp)) == ' ' || c == '\t' || c == '\n' || c == '/') {
        if (c == '/') {
            if ((c = fgetc(fp)) == '/') {       // check second '/'
                while((c = fgetc(fp)) != '\n')
                    ;
            } else if (c == '*') {              // skip until meet '*', '/'
                char last = ' ';
                while(((c = fgetc(fp)) != '/') || (last != '*')) {
                    //printf("last: %c, curr: %c\n", last, c);
                    last = c;
                }
            }
            else {                              // if only one '/', then we should put it back
                ungetc('/', fp);
                return 1;
            }
        }
    }
    ungetc(c, fp);                              // while we check the c in the while loop, one
                                                // must be give back to the stream
    if(isalnum(c) || ispunct(c)) {
        return 1;
    } else {
        return 0;
    }
}

// Gets the next token from the input and makes it the current token. This
// method should only be called if has_more_token() is true. Initially there is no current token.
void advance(FILE *fp)
{
    if (putback) {
        putback = 0;
        return;
    }
    int c = fgetc(fp);
    char *p = token;

    if (isalpha(c)) {
        for (*p++ = c; isalnum(c = fgetc(fp)); )
            *p++ = c;
        *p = '\0';
        ungetc(c, fp);

        int i;
        tokenType = IDENTIFIER;
        for (i = 0; i < KEYWORD_NUM; i++) {
            if (strcmp(keyword[i], token) == 0) {
                tokenType = KEYWORD;
                break;
            }
        }
        //printf("%s, %d\n", token, tokenType);
    } else if (c == '\"') {         // string constant
        tokenType = STRING_CONST;
        c = fgetc(fp);
        for (*p++ = c; (c = fgetc(fp)) != '\"'; )
            *p++ = c;
        *p = '\0';
    } else if (ispunct(c)) {        // symbol
        int i;
        for (i = 0; i < SYMBOL_NUM; i++) {
            if (c == symbol[i]) {
                tokenType = SYMBOL;
                *p++ = c;
                *p = '\0';
                break;
            }
        }
        if (strcmp(">", token) == 0) {
            strcpy(token, "&gt;");
        } else if (strcmp("<", token) == 0) {
            strcpy(token, "&lt;");
        } if (strcmp("&", token) == 0) {
            strcpy(token, "&amp;");
        }
        //printf("%s, %d\n", token, tokenType);
    } else if (isdigit(c)) {        // integer constant
        tokenType = INT_CONST;
        for (*p++ = c; isdigit(c = fgetc(fp)); )
            *p++ = c;
        *p = '\0';
        ungetc(c, fp);
    }
    //printf("%d\n", tokenType);
}

// eat one token and then renew the token and the tokenType
int eat(FILE *fp)
{
    if (has_more_token(fp)) {
        advance(fp);
        return TRUE;
    }
    else
        return FALSE;
}

// returns the keyword which is the current token. Should be called only when tokenType is KEYWORD
inline int keyWord()
{
    if (strcmp(token, "class") == 0) {
        return _CLASS;
    } else if (strcmp(token, "int") == 0) {
        return _INT;
    } else if (strcmp(token, "boolean") == 0) {
        return _BOOLEAN;
    } else if (strcmp(token, "char") == 0) {
        return _CHAR;
    } else if (strcmp(token, "void") == 0) {
        return _VOID;
    } else if (strcmp(token, "static") == 0) {
        return _STATIC;
    } else if (strcmp(token, "field") == 0) {
        return _FIELD;
    } else if (strcmp(token, "constructor") == 0) {
        return _CONSTRUCTOR;
    } else if (strcmp(token, "function") == 0) {
        return _FUNCTION;
    } else if (strcmp(token, "method") == 0) {
        return _METHOD;
    } else if (strcmp(token, "let") == 0) {
        return _LET;
    } else if (strcmp(token, "do") == 0) {
        return _DO;
    } else if (strcmp(token, "if") == 0) {
        return _IF;
    } else if (strcmp(token, "while") == 0) {
        return _WHILE;
    } else if (strcmp(token, "return") == 0) {
        return _RETURN;
    }
    return -1;
}


void tokenTest(FILE *fp, FILE *fw)
{
    fprintf(fw, "<tokens>\n");
    while(has_more_token(fp)) {
        advance(fp);
        if (tokenType == INT_CONST) {
            fprintf(fw, "<integerConstant> %s </integerConstant>\n", token);
        } else if (tokenType == STRING_CONST) {
            fprintf(fw, "<stringConstant> %s </stringConstant>\n", token);
        } else if (tokenType == SYMBOL) {
            if (strcmp(">", token) == 0) {
                strcpy(token, "&gt;");
            } else if (strcmp("<", token) == 0) {
                strcpy(token, "&lt;");
            } if (strcmp("&", token) == 0) {
                strcpy(token, "&amp;");
            }
            fprintf(fw, "<symbol> %s </symbol>\n", token);
        } else if (tokenType == KEYWORD) {
            fprintf(fw, "<keyword> %s </keyword>\n", token);
        } else if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
        tokenType = -1;
    }
    fprintf(fw, "</tokens>\n");
}
