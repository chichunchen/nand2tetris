#include <helper.h>

#define MAXTOKEN    100
#define MAXFILE     256
#define KEYWORD_NUM 21
#define SYMBOL_NUM  19

int putback;
int is_LL1;
char token[MAXTOKEN];
int tokenType;

void compileKeyword(FILE *fp, FILE *fw, int options, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == KEYWORD) {
            if (options) {
                fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        }
    }
}

void compileSymbol(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }
}

void compileIntegerConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        fprintf(fw, "<integerConstant> %s </integerConstant>\n", token);
    }
}

void compileStringConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        fprintf(fw, "<stringConstant> %s </stringConstant>\n", token);
    }
}

void compileIdentifier(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
    }
}

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

    compileIdentifier(fp, fw, "className");
    compileSymbol(fp, fw, "{");

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

    putBack();
    compileSymbol(fp, fw, "}");

    fprintf(fw, "</class>\n");
}

void compileClassVarDec(FILE *fp, FILE *fw)
{
    fprintf(fw, "<classVarDec>\n");

    // token has been extracted to check whether class should call VarDec or subRoutine
    if (strcmp("static", token) == 0 || strcmp("field", token) == 0) {
        fprintf(fw, "<keyword> %s </keyword>\n", token);
    }

    if (has_more_token(fp)) {       // type
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        } else if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
    }
    compileIdentifier(fp, fw, "varName");

    // ((',' varName)*
    if (has_more_token(fp)) {
        advance(fp);
        while(strcmp(";", token) != 0) {
            if (tokenType == SYMBOL)
                fprintf(fw, "<symbol> %s </symbol>\n", token);      // ,

            compileIdentifier(fp, fw, "varName");

            if (has_more_token(fp)) {
                advance(fp);
            }
        }
        putBack();
        compileSymbol(fp, fw, ";");
    }
    fprintf(fw, "</classVarDec>\n");
}

void compileSubroutine(FILE *fp, FILE *fw)
{
    fprintf(fw, "<subroutineDec>\n");

    putBack();
    compileKeyword(fp, fw,
                    keyWord() == CONSTRUCTOR || keyWord() == FUNCTION || keyWord() == METHOD,
                    "constructor|function|method");

    if (has_more_token(fp)) {       // identifier | void | type
        advance(fp);
        if (tokenType == IDENTIFIER)
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        else if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _VOID || keyWord() == _BOOLEAN) {
            fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
    }

    compileIdentifier(fp, fw, "subroutineName");
    compileSymbol(fp, fw, "(");

    compileParameterList(fp, fw);

    putBack();
    compileSymbol(fp, fw, "}");

    // subroutineBody
    fprintf(fw, "<subroutineBody>\n");
    compileSymbol(fp, fw, "{");

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
    putBack();
    compileSymbol(fp, fw, "}");

    fprintf(fw, "</subroutineBody>\n");
    fprintf(fw, "</subroutineDec>\n");
}

void compileParameterList(FILE *fp, FILE *fw)
{
    fprintf(fw, "<parameterList>\n");

    if (has_more_token(fp)) {           // type for parameter
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        } else if (strcmp(")", token) == 0) {   // no parameter
            fprintf(fw, "</parameterList>\n");
            return;
        }
    }
    compileIdentifier(fp, fw, "varName");

    // (',' (type varName)*)?
    while(has_more_token(fp)) {
        advance(fp);
        if ((strcmp(")", token) != 0)) {
            putBack();
            compileSymbol(fp, fw, ",");
            if (has_more_token(fp)) {           // type for parameter
                advance(fp);
                if (tokenType == KEYWORD) {
                    if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                        fprintf(fw, "<keyword> %s </keyword>\n", token);
                    }
                }
            }
            //compileKeyword(fp, fw, keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN, "type");
            compileIdentifier(fp, fw, "varName");
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
        fprintf(fw, "<keyword> %s </keyword>\n", token);
    }

    if (has_more_token(fp)) {           // type
        advance(fp);
        if (tokenType == IDENTIFIER) {
            fprintf(fw, "<identifier> %s </identifier>\n", token);
        } else if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN || keyWord() == _VOID) {
            fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
    }

    compileIdentifier(fp, fw, "varName");

    // it can be ',' or ';'
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            while (strcmp(";", token) != 0) {
                // (',' varName)*
                fprintf(fw, "<symbol> %s </symbol>\n", token);      // ','
                compileIdentifier(fp, fw, "varName");
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
        case _IF:
            compileIf(fp, fw);
            break;
        case _WHILE:
            compileWhile(fp, fw);
            break;
        case _RETURN:
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
        fprintf(fw, "<keyword> %s </keyword>\n", token);
    }

    compileIdentifier(fp, fw, "varName");

    if (has_more_token(fp)) {
        advance(fp);
    }

    // ('[' expression ']')?
    if (strcmp("[", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "[");
        compileExpression(fp, fw);
        compileSymbol(fp, fw, "]");
        compileSymbol(fp, fw, "=");
    } else {
        putBack();
        compileSymbol(fp, fw, "=");
    }

    compileExpression(fp, fw);
    compileSymbol(fp, fw, ";");

    fprintf(fw, "</letStatement>\n");
}

void compileDo(FILE *fp, FILE *fw)
{
    fprintf(fw, "<doStatement>\n");

    putBack();
    compileKeyword(fp, fw, 1, "do");

    compileSubroutineCall(fp, fw);
    compileSymbol(fp, fw, ";");

    fprintf(fw, "</doStatement>\n");
}

void compileReturn(FILE *fp, FILE *fw)
{
    fprintf(fw, "<returnStatement>\n");
    if (tokenType == KEYWORD) {         // return
        fprintf(fw, "<keyword> %s </keyword>\n", token);
    }

    if (has_more_token(fp)) {           // check have varName
        advance(fp);
    }
    if (strcmp(";", token) != 0) {
        putBack();
        compileExpression(fp, fw);
        if (has_more_token(fp)) {       // varName
            advance(fp);
        }
    }

    putBack();
    compileSymbol(fp, fw, ";");

    fprintf(fw, "</returnStatement>\n");
}

void compileExpression(FILE *fp, FILE *fw)
{
    fprintf(fw, "<expression>\n");

    compileTerm(fp, fw);

    // (op term)
    if (has_more_token(fp)) {                   // if not op, should putback
        advance(fp);
    }
    int i;
    int is_op = 0;
    for (i = 0; i < 9; i++) {
        if (strcmp(op[i], token) == 0) {        // check op?
            is_op = 1;
            if (tokenType == SYMBOL)            // op
                fprintf(fw, "<symbol> %s </symbol>\n", token);
            compileTerm(fp, fw);
        }
    }
    if (!is_op)
        putBack();

    fprintf(fw, "</expression>\n");
}

void compileTerm(FILE *fp, FILE *fw)
{
    fprintf(fw, "<term>\n");
    if (has_more_token(fp)) {
        advance(fp);
    }

    if (tokenType == IDENTIFIER) {
        putBack();
        compileIdentifier(fp, fw, "varName");
    } else if (tokenType == INT_CONST) {
        putBack();
        compileIntegerConstant(fp, fw);
    } else if (tokenType == STRING_CONST) {
        putBack();
        compileStringConstant(fp, fw);
    } else if (tokenType == KEYWORD) {
        putBack();
        compileKeyword(fp, fw, 1, "keywordConstant");
    } else if (strcmp("(", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "(");
        compileExpression(fp, fw);
        compileSymbol(fp, fw, ")");
    } else if (strcmp(unaryOp[0], token) == 0 || strcmp(unaryOp[1], token) == 0) {
        putBack();
        compileSymbol(fp, fw, "unaryOp");
        compileTerm(fp, fw);
    }

    if (has_more_token(fp))                                 // check ahead for LL(1)
        advance(fp);

    if (strcmp(".", token) == 0) {
        is_LL1 = 1;
        putBack();
        compileSubroutineCall(fp, fw);
    } else if (strcmp("[", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "[");
        compileExpression(fp, fw);
        compileSymbol(fp, fw, "]");
    } else {
        putBack();
    }

    fprintf(fw, "</term>\n");
}

void compileIf(FILE *fp, FILE *fw)
{
    fprintf(fw, "<ifStatement>\n");
    putBack();
    compileKeyword(fp, fw, 1, "if");
    compileSymbol(fp, fw, "(");
    compileExpression(fp, fw);
    compileSymbol(fp, fw, ")");
    compileSymbol(fp, fw, "{");
    compileStatements(fp, fw);
    // no need to get new token, compileStatement has done it to
    // check the ending
    putBack();
    compileSymbol(fp, fw, "}");

    // check else
    if (has_more_token(fp)) {
        advance(fp);
    }
    if (strcmp("else", token) == 0) {
        putBack();
        compileKeyword(fp, fw, 1, "else");
        compileSymbol(fp, fw, "{");
        compileStatements(fp, fw);
        putBack();
        compileSymbol(fp, fw, "}");
    } else {
        putBack();
    }

    fprintf(fw, "</ifStatement>\n");
}

void compileWhile(FILE *fp, FILE *fw)
{
    fprintf(fw, "<whileStatement>\n");

    putBack();
    compileKeyword(fp, fw, 1, "while");
    compileSymbol(fp, fw, "(");
    compileExpression(fp, fw);
    compileSymbol(fp, fw, ")");
    compileSymbol(fp, fw, "{");
    compileStatements(fp, fw);
    // no need to get new token, compileStatement has done it to
    // check the ending
    putBack();
    compileSymbol(fp, fw, "}");

    fprintf(fw, "</whileStatement>\n");
}

// before calling this method, you should check the token
// if the token is ")", then you should just print <expressionList></expressionList>
void compileExpressionList(FILE *fp, FILE *fw)
{
    fprintf(fw, "<expressionList>\n");

    compileExpression(fp, fw);

    if (has_more_token(fp)) {
        advance(fp);
    }

    // (',' expression)*
    while (strcmp(",", token) == 0) {
        putBack();
        compileSymbol(fp, fw, ",");
        compileExpression(fp, fw);
        if (has_more_token(fp))
            advance(fp);
    }
    putBack();

    fprintf(fw, "</expressionList>\n");
}

void compileSubroutineCall(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {       // subroutineName | className
        advance(fp);
    }

    if (is_LL1) {
        putBack();
        is_LL1 = 0;
    }
    else if (tokenType == IDENTIFIER) {
        fprintf(fw, "<identifier> %s </identifier>\n", token);
    }

    if (has_more_token(fp)) {                   // could be '(' or '.'
        advance(fp);
    }

    if (strcmp("(", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "(");
    } else if (strcmp(".", token) == 0) {
        putBack();
        compileSymbol(fp, fw, ".");
        compileIdentifier(fp, fw, "subroutineName");
        compileSymbol(fp, fw, "(");
    }

    if (has_more_token(fp)) {
        advance(fp);
    }
    if (strcmp(")", token) == 0) {
        fprintf(fw, "<expressionList>\n</expressionList>\n");
        putBack();
    } else {
        putBack();
        compileExpressionList(fp, fw);
    }

    compileSymbol(fp, fw, ")");
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>\n");
    else {
        putback = 0;
        FILE *fp;
        FILE *fw;
        if (!is_directory(argv[1])) {
            fp = fopen(argv[1], "r");
            char *ptr = setFileName(argv[1]);
            fw = fopen(ptr, "w");
            compileClass(fp, fw);
            fclose(fp);
            fclose(fw);
            //tokenTest(fp, fw);
        } else {        // dir
            WIN32_FIND_DATA fdFile;
            HANDLE hFind = NULL;

            char sPath[2048];

            //Specify a file mask. *.* = We want everything!
            sprintf(sPath, "%s\\*.*", argv[1]);
            //printf("%s\n", sPath);
            if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
                printf("Path not found: [%s]\n", argv[1]);
                return;
            }

            do {
                //Find first file will always return "."
                //    and ".." as the first two directories.
                if (strcmp(fdFile.cFileName, ".") != 0
                    && strcmp(fdFile.cFileName, "..") != 0) {
                    //Build up our file path using the passed in
                    //  [sDir] and the file/foldername we just found:
                    sprintf(sPath, "%s\\%s", argv[1], fdFile.cFileName);

                    //Is the entity a File or Folder?
                    if (fdFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY) {
                        printf("Directory: %s\n", sPath);
                        //ListDirectoryContents(sPath); //Recursion, I love it!
                    }
                    else {
                        char *p = sPath;
                        while(*p++ != '.')
                            ;
                        if (strcmp("jack", p) == 0) {
                            fp = fopen(sPath, "r");
                            char *ptr = setFileName(sPath);
                            fw = fopen(ptr, "w");
                            compileClass(fp, fw);
                            fclose(fp);
                            fclose(fw);
                        }
                    }
                }
            } while(FindNextFile(hFind, &fdFile)); //Find the next file.

            FindClose(hFind); //Always, Always, clean things up!
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
    strcpy(ptr, "ml");
    return filename;
}

int is_directory(const char *szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

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

void putBack()
{
    putback = 1;
}

// returns the keyword which is the current token. Should be called only when tokenType is KEYWORD
int keyWord()
{
    if (strcmp(token, "class") == 0) {
        return CLASS;
    } else if (strcmp(token, "int") == 0) {
        return _INT;
    } else if (strcmp(token, "boolean") == 0) {
        return _BOOLEAN;
    } else if (strcmp(token, "char") == 0) {
        return _CHAR;
    } else if (strcmp(token, "void") == 0) {
        return _VOID;
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
