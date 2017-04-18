#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXTOKEN    100
#define MAXFILE     256
#define KEYWORD_NUM 21
#define SYMBOL_NUM  19

int has_got_token;

enum tokenType {
    KEYWORD, SYMBOL,
    IDENTIFIER, INT_CONST,
    STRING_CONST
};

enum keyword {
    CLASS, METHOD, FUNCTION,
    CONSTRUCTOR, INT,
    BOOLEAN, CHAR, VOID,
    VAR, STATIC, FIELD, LET,
    DO, IF, ELSE, WHILE,
    RETURN, TRUE, FALSE,
    NULLX, THIS
};

const char *keyword[] = {
    "class", "constructor", "function",
    "method", "field", "static", "var",
    "int", "char", "boolean", "void", "true",
    "false", "null", "this", "let", "do",
    "if", "else", "while", "return"
};
const char symbol[] = {
    '{', '}', '(', ')', '[', ']', '.',
    ',', ';', '+', '-', '*', '/', '&',
    '|', '<', '>', '=', '~'
};
char token[MAXTOKEN];
int tokenType;

char *setFileName(const char *arg);

// JackTokenizer Module
int is_regular_file(const char *filename);
int has_more_token(FILE *fp);
void advance(FILE *fp);
int keyWord();
void tokenTest(FILE *fp, FILE *fw);

// Compilation Module
void compileClass(FILE *fp, FILE *fw);
void compileClassVarDec(FILE *fp, FILE *fw);
void compileSubroutine(FILE *fp, FILE *fw);
void compileParameterList(FILE *fp, FILE *fw);
void compileVarDec(FILE *fp, FILE *fw);
void compileStatements(FILE *fp, FILE *fw);
void compileDo(FILE *fp, FILE *fw);
void compileLet(FILE *fp, FILE *fw);
void compileWhile(FILE *fp, FILE *fw);
void compileReturn(FILE *fp, FILE *fw);
void compileIf(FILE *fp, FILE *fw);
void compileExpression(FILE *fp, FILE *fw);
void compileTerm(FILE *fp, FILE *fw);
void compileExpressionList(FILE *fp, FILE *fw);

void compileClass(FILE *fp, FILE *fw)
{
    fprintf(fw, "<class>\n");
    if(has_more_token(fp)) {
        advance(fp);
        if (strcmp(token, "class") == 0) {
            fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
        tokenType = -1;
    }
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
    }
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }

    // classVarDec*
    while (has_more_token(fp)) {
        advance(fp);
        if (keyWord() == STATIC || keyWord() == FIELD) {
            compileClassVarDec(fp, fw);
        } else {
            break;
        }
    }
    // subroutineDec*
    do {
        if (keyWord() == CONSTRUCTOR || keyWord() == FUNCTION || keyWord() == METHOD) {
            compileSubroutine(fp, fw);
        } else {
            break;
        }
        if (has_more_token(fp)) {
            advance(fp);
        }
    } while(strcmp("}", token) != 0);


    if (tokenType == SYMBOL) {
        fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    fprintf(fw, "</class>\n");
}

void compileClassVarDec(FILE *fp, FILE *fw)
{
    fprintf(fw, "<classVarDec>\n");

    // token has been extracted to check whether class should call VarDec or subRoutine
    if (strcmp("static", token) == 0 || strcmp("field", token) == 0) {
        fprintf(fw, "<keyword> %s </keyword>", token);
    }

    if (has_more_token(fp)) {       // type
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == INT || keyWord() == CHAR || keyWord() == BOOLEAN) {
                fprintf(fw, "<keyword> %s </keyword>", token);
            }
        }
    }
    if (has_more_token(fp)) {       // identifier
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>", token);
        }
    }

    // ((',' varName)*
    if (has_more_token(fp)) {
        advance(fp);
        while(strcmp(";", token) != 0) {
            if (tokenType == SYMBOL)
                fprintf(fw, "<symbol> %s </symbol>\n", token);      // ,
            if (has_more_token(fp)) {                               // identifier
                advance(fp);
                if (tokenType == IDENTIFIER)
                    fprintf(fw, "<identifier> %s </identifier>", token);
            }
            if (has_more_token(fp)) {
                advance(fp);
            }
        }
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);           // ;
    }
    fprintf(fw, "</classVarDec>\n");
}

void compileSubroutine(FILE *fp, FILE *fw)
{
    fprintf(fw, "<subroutineDec>\n");

    if (keyWord() == CONSTRUCTOR || keyWord() == FUNCTION || keyWord() == METHOD) {
        fprintf(fw, "<keyword> %s </keyword>", token);
    }

    if (has_more_token(fp)) {       // identifier | void | type
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        else if (keyWord() == INT || keyWord() == CHAR || keyWord() == VOID || keyWord() == BOOLEAN) {
            fprintf(fw, "<keyword> %s </keyword>", token);
        }
    }

    if (has_more_token(fp)) {       // subroutineName
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
    }

    if (has_more_token(fp)) {       // '('
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }
    compileParameterList(fp, fw);

    if (tokenType == SYMBOL) {      // ')'
        fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    // subroutineBody
    fprintf(fw, "<subroutineBody>\n");
    if (has_more_token(fp)) {       // '{'
        advance(fp);
        //if ((strcmp("{", token) == 0))
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }
    // varDec*
    if (has_more_token(fp)) {
        advance(fp);
    }
    while (strcmp("var", token) == 0) {
        compileVarDec(fp, fw);
        if (has_more_token(fp))
            advance(fp);
    }

    compileStatements(fp, fw);
    if (tokenType == SYMBOL)         // '}'
        fprintf(fw, "<symbol> %s </symbol>\n", token);

    fprintf(fw, "</subroutineBody>\n");
    fprintf(fw, "</subroutineDec>\n");
}

void compileParameterList(FILE *fp, FILE *fw)
{
    fprintf(fw, "<parameterList>\n");

    if (has_more_token(fp)) {           // type for parameter
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == INT || keyWord() == CHAR || keyWord() == BOOLEAN) {
                fprintf(fw, "<keyword> %s </keyword>", token);
            }
        } else if (strcmp(")", token) == 0) {   // no parameter
            fprintf(fw, "</parameterList>\n");
            return;
        }
    }
    if (has_more_token(fp)) {           // name for parameter
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>", token);
        }
    }

    // (',' (type varName)*)?
    while(has_more_token(fp)) {
        advance(fp);
        if ((strcmp(")", token) != 0)) {
            if (tokenType == SYMBOL)
                fprintf(fw, "<symbol> %s </symbol>\n", token);
            if (has_more_token(fp)) {           // type for parameter
                advance(fp);
                if (tokenType == KEYWORD) {
                    if (keyWord() == INT || keyWord() == CHAR || keyWord() == BOOLEAN) {
                        fprintf(fw, "<keyword> %s </keyword>", token);
                    }
                }
            }
            if (has_more_token(fp)) {           // name for parameter
                advance(fp);
                if (tokenType == IDENTIFIER) {
                    fprintf(fw, "<identifier> %s </identifier>", token);
                }
            }
        } else {
            break;
        }
    }

    fprintf(fw, "</parameterList>\n");
}

// 'var' type varName (',' varName)* ';'
void compileVarDec(FILE *fp, FILE *fw)
{
    fprintf(fw, "<varDec>\n");

    if (strcmp("var", token) == 0) {    // var
        fprintf(fw, "<keyword> %s </keyword>", token);
    }

    if (has_more_token(fp)) {           // type
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>", token);
        }
    }

    if (has_more_token(fp)) {           // varName
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>", token);
        }
    }

    // it can be ',' or ';'
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            while (strcmp(";", token) != 0) {
                // (',' varName)*
                fprintf(fw, "<symbol> %s </symbol>\n", token);      // ','
                if (has_more_token(fp)) {
                    advance(fp);
                    if (tokenType == IDENTIFIER) {
                        fprintf(fw, "<identifier> %s </identifier>", token);
                    }
                }
                if (has_more_token(fp))
                    advance(fp);
            }
            // ;
            fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }

    fprintf(fw, "</varDec>\n");
}

void compileStatements(FILE *fp, FILE *fw)
{
    fprintf(fw, "<statements>\n");
    // since we check token == var or not to determine the ending of varDec
    // therefore, we don't have to use 'has_more_token() to set the cursor to the new token
    // in the first time.
    do {       // token = 'let' | 'do' | 'if' | 'while' | 'return'
        advance(fp);
        if (strcmp("}", token) == 0) {
            break;
        }
        switch(keyWord()) {
        case LET:
            compileLet(fp, fw);
            break;
        case DO:
            compileDo(fp, fw);
            break;
        case IF:
            compileIf(fp, fw);
            break;
        case WHILE:
            compileWhile(fp, fw);
            break;
        case RETURN:
            compileReturn(fp, fw);
            break;
        }
    } while (has_more_token(fp));
    fprintf(fw, "</statements>\n");
}

void compileLet(FILE *fp, FILE *fw)
{
    fprintf(fw, "<letStatement>\n");

    if (tokenType == KEYWORD) {     // let
        fprintf(fw, "<keyword> %s </keyword>", token);
    }
    if (has_more_token(fp)) {       // varName
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
    }

    // TODO ('[' expression ']')?

    if (has_more_token(fp)) {       // =
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    // TODO expression

    fprintf(fw, "<expression>\n");
    fprintf(fw, "<term>\n");
    if (has_more_token(fp)) {       // varName
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
    }
    fprintf(fw, "</term>\n");
    fprintf(fw, "</expression>\n");

    if (has_more_token(fp)) {       // ;
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    fprintf(fw, "</letStatement>\n");
}

void compileDo(FILE *fp, FILE *fw)
{
    fprintf(fw, "<doStatement>\n");

    if (tokenType == KEYWORD) {     // do
        fprintf(fw, "<keyword> %s </keyword>", token);
    }

    if (has_more_token(fp)) {       // subroutineName
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
    }

    if (has_more_token(fp)) {       // (
        advance(fp);
        if (tokenType == SYMBOL) {
            if (strcmp("(", token) == 0)
                fprintf(fw, "<symbol> %s </symbol>\n", token);  // '('
            else if (strcmp(".", token) == 0) {
                fprintf(fw, "<symbol> %s </symbol>\n", token);  // '.'
                if (has_more_token(fp)) {
                    advance(fp);
                    if (tokenType == IDENTIFIER)
                        fprintf(fw, "<identifier> %s </identifier>\n", token);
                    if (has_more_token(fp)) {
                        advance(fp);
                        fprintf(fw, "<symbol> %s </symbol>\n", token);  // '('
                    }
                }
            }
        }
    }
    // expression
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType != SYMBOL)
            fprintf(fw, "<expressionList> %s </expressionList>\n", token);

        if (tokenType == SYMBOL)        // )
            fprintf(fw, "<symbol> %s </symbol>\n", token);

        if (has_more_token(fp)) {       // ;
            advance(fp);
            if (tokenType == SYMBOL)
                fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
        fprintf(fw, "</doStatement>\n");
        return;
    }

    if (has_more_token(fp)) {       // )
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    if (has_more_token(fp)) {       // ;
        advance(fp);
        //printf("ggff %s\n", token);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>\n", token);
    }

    fprintf(fw, "</doStatement>\n");
}

void compileReturn(FILE *fp, FILE *fw)
{
    fprintf(fw, "<returnStatement>\n");
    if (tokenType == KEYWORD) {     // return
        fprintf(fw, "<keyword> %s </keyword>", token);
    }

    if (has_more_token(fp)) {       // check have varName
        advance(fp);
    }
    if (strcmp(";", token) != 0) {
        fprintf(fw, "<expression>\n");
        fprintf(fw, "<term>\n");
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
        fprintf(fw, "</term>\n");
        fprintf(fw, "</expression>\n");
        if (has_more_token(fp)) {       // varName
            advance(fp);
        }
    }

    if (tokenType == SYMBOL)        // ;
        fprintf(fw, "<symbol> %s </symbol>\n", token);

    fprintf(fw, "</returnStatement>\n");
}

void compileIf(FILE *fp, FILE *fw)
{
    fprintf(fw, "<ifStatement>\n");
    if (tokenType == KEYWORD) {     // if
        fprintf(fw, "<keyword> %s </keyword>", token);
    }
    if (has_more_token(fp)) {       // '('
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>", token);
    }
    // TODO: expression
    fprintf(fw, "<expression>\n");
    fprintf(fw, "<term>\n");
    if (has_more_token(fp)) {
        advance(fp);
        fprintf(fw, "<identifier> %s </identifier>", token);
    }
    fprintf(fw, "</term>\n");
    fprintf(fw, "</expression>\n");

    if (has_more_token(fp)) {       // ')'
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>", token);
    }
    if (has_more_token(fp)) {       // '{'
        advance(fp);
        if (tokenType == SYMBOL)
            fprintf(fw, "<symbol> %s </symbol>", token);
    }

    compileStatements(fp, fw);

    // no need to get new token, compileStatement has done it to
    // check the ending
    if (tokenType == SYMBOL)
        fprintf(fw, "<symbol> %s </symbol>", token);

    fprintf(fw, "</ifStatement>\n");
}

void compileWhile(FILE *fp, FILE *fw)
{
    fprintf(fw, "<whileStatement>\n");
    fprintf(fw, "</whileStatement>\n");
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>\n");
    else {
        // init
        has_got_token = 0;
        if (is_regular_file(argv[1])) {
            FILE *fp = fopen(argv[1], "r");
            char *ptr = setFileName(argv[1]);
            FILE *fw = fopen(ptr, "w");
            compileClass(fp, fw);
            //tokenTest(fp, fw);
        } else {        // dir

        }
    }

    return 0;
}

char* setFileName(const char *arg)
{
    char *filename = malloc(sizeof(char) * MAXFILE);
    strcpy(filename, arg);
    char *ptr = filename;
    while(*ptr++ != '.') ;
    strcpy(ptr, "xml");
    return filename;
}

int is_regular_file(const char *filename)
{
    return 1;
}

void skip_comments(FILE *fp)
{
    int c;
    if ((c = fgetc(fp)) == '/')
        while((c = fgetc(fp)) != '\n')
            ;
}

int has_more_token(FILE *fp)
{
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

// returns the keyword which is the current token. Should be called only when tokenType is KEYWORD
int keyWord()
{
    if (strcmp(token, "class") == 0) {
        return CLASS;
    } else if (strcmp(token, "int") == 0) {
        return INT;
    } else if (strcmp(token, "boolean") == 0) {
        return BOOLEAN;
    } else if (strcmp(token, "char") == 0) {
        return CHAR;
    } else if (strcmp(token, "void") == 0) {
        return VOID;
    } else if (strcmp(token, "static") == 0) {
        return STATIC;
    } else if (strcmp(token, "field") == 0) {
        return FIELD;
    } else if (strcmp(token, "constructor") == 0) {
        return CONSTRUCTOR;
    } else if (strcmp(token, "function") == 0) {
        return FUNCTION;
    } else if (strcmp(token, "method") == 0) {
        return METHOD;
    } else if (strcmp(token, "let") == 0) {
        return LET;
    } else if (strcmp(token, "do") == 0) {
        return DO;
    } else if (strcmp(token, "if") == 0) {
        return IF;
    } else if (strcmp(token, "while") == 0) {
        return WHILE;
    } else if (strcmp(token, "return") == 0) {
        return RETURN;
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
