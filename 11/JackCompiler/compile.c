#include "helper.h"
#include "constant.h"

/* terminals */

char className[100];
char LL1_TEMP[100];

int if_counter = -1;
int while_counter = -1;

static inline void compileKeyword(FILE *fp, FILE *fw, int options, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == KEYWORD) {
            if (options) {
                //fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        }
    }
}

static inline void compileSymbol(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            //fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }
}

static inline void compileIntegerConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        //fprintf(fw, "<integerConstant> %s </integerConstant>\n", token);
    }
}

static inline void compileStringConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        //fprintf(fw, "<stringConstant> %s </stringConstant>\n", token);
    }
}

static inline void compileIdentifier(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == IDENTIFIER) {
            //fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
    }
}

static inline void checkType()
{
    if (tokenType == IDENTIFIER) {
        //fprintf(fw, "<identifier> %s </identifier>\n", token);
    }
    else if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _VOID || keyWord() == _BOOLEAN) {
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
    }
}

static inline void checkVarType()
{
    if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
    }
}

/* non-terminals */

void compileClass(FILE *fp, FILE *fw)
{
    compileKeyword(fp, fw, 1, "class");
    compileIdentifier(fp, fw, "className");
    strcpy(className, token);
    compileSymbol(fp, fw, "{");

    // classVarDec*
    while (eat(fp)) {
        if (keyWord() == _STATIC || keyWord() == _FIELD) {
            compileClassVarDec(fp, fw);
        } else {
            break;
        }
    }

    // subroutineDec*
    for ( ; strcmp("}", token) != 0; eat(fp)) {
        if (keyWord() == _CONSTRUCTOR || keyWord() == _FUNCTION || keyWord() == _METHOD) {
            compileSubroutine(fp, fw);
        } else {
            break;
        }
    }

    putBack();
    compileSymbol(fp, fw, "}");

    // clean environment
    printClassTable();
    cleanClassTab();
}

void compileClassVarDec(FILE *fp, FILE *fw)
{
    char type[BUFSIZ];
    int kind = -1;

    // token has been extracted to check whether class should call VarDec or subRoutine
    putBack();
    compileKeyword(fp, fw, keyWord() == STATIC || keyWord() == FIELD, "decorator: field or static");

    // keep type for defining the variable to symbol table
    if (keyWord() == _STATIC) {
        kind = STATIC;
    } else if (keyWord() == _FIELD) {
        kind = FIELD;
    }

    // get type
    eat(fp);
    strcpy(type, token);

    compileIdentifier(fp, fw, "varName");
    Define(CLASS, token, type, kind, varCount(CLASS, kind));

    // ((',' varName)*
    for(eat(fp); strcmp(";", token) != 0; eat(fp)) {
        putBack();
        compileSymbol(fp, fw, ",");
        compileIdentifier(fp, fw, "varName");
        Define(CLASS, token, type, kind, varCount(CLASS, kind));
    }
    putBack();
    compileSymbol(fp, fw, ";");
}

void compileSubroutine(FILE *fp, FILE *fw)
{
    char decorator[BUFSIZ];
    char subroutinName[BUFSIZ];

    putBack();
    compileKeyword(fp, fw,
                    keyWord() == _CONSTRUCTOR || keyWord() == _FUNCTION || keyWord() == _METHOD,
                    "constructor|function|method");
    strcpy(decorator, token);

    // arg0
    if (keyWord() == _METHOD) {
        Define(SUBROUTINE, "this", className, ARG, 0);
    }

    eat(fp);
    checkType();

    compileIdentifier(fp, fw, "subroutineName");
    strcpy(subroutinName, token);
    compileSymbol(fp, fw, "(");

    compileParameterList(fp, fw);

    putBack();
    compileSymbol(fp, fw, "}");

    // subroutineBody
    compileSymbol(fp, fw, "{");

    // varDec*, besides the parsing, also install all the variables to the symbol table
    for (eat(fp); strcmp("var", token) == 0; eat(fp))
        compileVarDec(fp, fw);

    char temp[BUFSIZ];
    strcpy(temp, className);
    strcat(temp, ".");
    strcat(temp, subroutinName);
    writeFunction(fw, temp, varCount(SUBROUTINE, VAR));

    // push pointer to constructor or method
    if (strcmp("constructor", decorator) == 0) {
        writePush(fw, __CONST, varCount(CLASS, FIELD));
        writeCall(fw, "Memory.alloc", 1);
        writePop(fw, __POINTER, 0);
    } else if (strcmp("method", decorator) == 0) {
        // push this
        writePush(fw, __ARG, 0);
        writePop(fw, __POINTER, 0);
    }

    compileStatements(fp, fw);
    putBack();
    compileSymbol(fp, fw, "}");

#ifndef NDEBUG
    printSubTable();
    cleanSubroutineTab();
#endif
    // refresh the counter when a subroutine is over
    if_counter = -1;
    while_counter = -1;
}

void compileParameterList(FILE *fp, FILE *fw)
{
    char type[BUFSIZ];

    // eat type
    eat(fp);

    if (strcmp(")", token) == 0) {   // no parameter
        return;
    } else if (tokenType == KEYWORD || tokenType == IDENTIFIER) {
        checkVarType();
        strcpy(type, token);
    }

    compileIdentifier(fp, fw, "varName");
    Define(SUBROUTINE, token, type, ARG, varCount(SUBROUTINE, ARG));

    // (',' (type varName)*)?
    while(has_more_token(fp)) {
        advance(fp);
        if ((strcmp(")", token) != 0)) {
            putBack();
            compileSymbol(fp, fw, ",");

            if (has_more_token(fp)) {           // type for parameter
                advance(fp);
                if (tokenType == KEYWORD) {
                    putBack();
                    compileKeyword(fp, fw, 1, "primitive type");
                } else if (tokenType == IDENTIFIER) {
                    putBack();
                    compileIdentifier(fp, fw, "custom type");
                }
                strcpy(type, token);
            }
            compileIdentifier(fp, fw, "varName");
            Define(SUBROUTINE, token, type, ARG, varCount(SUBROUTINE, ARG));
        } else {
            break;
        }
    }
}

// 'var' type varName (',' varName)* ';'
void compileVarDec(FILE *fp, FILE *fw)
{
    char type[BUFSIZ];
    //fprintf(fw, "<varDec>\n");

    putBack();
    compileKeyword(fp, fw, 1, "var");

    if (has_more_token(fp)) {           // type
        advance(fp);
        if (tokenType == IDENTIFIER) {
            //fprintf(fw, "<identifier> %s </identifier>\n", token);
        } else if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN || keyWord() == _VOID) {
            //fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
        strcpy(type, token);
    }
    compileIdentifier(fp, fw, "varName");
    Define(SUBROUTINE, token, type, VAR, varCount(SUBROUTINE, VAR));

    // it can be ',' or ';'
    for (eat(fp); strcmp(";", token) != 0; eat(fp)) {
        if (tokenType == SYMBOL) {
            compileIdentifier(fp, fw, "varName");
            Define(SUBROUTINE, token, type, VAR, varCount(SUBROUTINE, VAR));
        }
    }
}

void compileStatements(FILE *fp, FILE *fw)
{
    // since we check whether (token == var or not) to determine the ending of varDec
    // therefore, we don't have to use 'has_more_token() to set the cursor to the new token
    // in the first time.
    do {       // token = 'let' | 'do' | 'if' | 'while' | 'return'
        advance(fp);
        if (strcmp("}", token) == 0) {
            break;
        }
        switch(keyWord()) {
        case _LET:
            compileLet(fp, fw);
            break;
        case _DO:
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
    //fprintf(fw, "</statements>\n");
}

void compileLet(FILE *fp, FILE *fw)
{
    char varName[BUFSIZ];
    int is_array = FALSE;

    putBack();
    compileKeyword(fp, fw, 1, "let");

    compileIdentifier(fp, fw, "varName");
    strcpy(varName, token);

    // eat '='
    eat(fp);

    // ('[' expression ']')?
    if (strcmp("[", token) == 0) {
        putBack();

        compileSymbol(fp, fw, "[");

        // push the base address of the Array
        if (kindOf(varName) == VAR) {
            writePush(fw, __LOCAL, indexOf(varName));
        } else if (kindOf(varName) == FIELD) {
            writePush(fw, __THIS, indexOf(varName));
        } else if (kindOf(varName) == STATIC) {
            writePush(fw, __STATIC, indexOf(varName));
        } else if (kindOf(varName) == ARG) {
            writePush(fw, __ARG, indexOf(varName));
        }

        // the result of the expression will be pushed into stack
        // the result of the expression is the offset of the var pushed below
        compileExpression(fp, fw);

        // add the result of expression and the base address of the pushed var
        writeArithmetic(fw, "+");
        is_array = TRUE;

        compileSymbol(fp, fw, "]");
        compileSymbol(fp, fw, "=");
    } else {
        putBack();
        compileSymbol(fp, fw, "=");
    }

    compileExpression(fp, fw);

    compileSymbol(fp, fw, ";");

    if (is_array) {
        writePop(fw, __TEMP, 0);
        writePop(fw, __POINTER, 1);
        writePush(fw, __TEMP, 0);
        writePop(fw, __THAT, 0);
    } else if (kindOf(varName) == VAR) {
        writePop(fw, __LOCAL, indexOf(varName));
    } else if (kindOf(varName) == ARG) {
        writePop(fw, __ARG, indexOf(varName));
    } else if (kindOf(varName) == FIELD) {
        writePop(fw, __THIS, indexOf(varName));
    } else if (kindOf(varName) == STATIC) {
        writePop(fw, __STATIC, indexOf(varName));
    } else if (kindOf(varName) == FIELD) {
        writePop(fw, __POINTER, indexOf(varName));
    }
}

void compileDo(FILE *fp, FILE *fw)
{
    putBack();
    compileKeyword(fp, fw, 1, "do");

    compileSubroutineCall(fp, fw);
    compileSymbol(fp, fw, ";");

    // pop dummy data for return
    writePop(fw, __TEMP, 0);
}

void compileReturn(FILE *fp, FILE *fw)
{
    putBack();
    compileKeyword(fp, fw, 1, "return");

    // eat for checking token is varName or ';'
    eat(fp);
    if (strcmp(";", token) != 0) {
        putBack();
        compileExpression(fp, fw);
        eat(fp);
    } else {
        // push dummy data(0) to stack, since the spec require all the subroutine
        // to return a value (which means push one var onto stack
        writePush(fw, __CONST, 0);
    }

    writeReturn(fw);
    putBack();
    compileSymbol(fp, fw, ";");
}

void compileExpression(FILE *fp, FILE *fw)
{
    int i;
    int is_op = 0;
    char oper[BUFSIZ];

    compileTerm(fp, fw);

    // (op term): eat op
    eat(fp);

    for (i = 0; i < OP_NUM; i++) {
        if (strcmp(op[i], token) == 0) {        // check op?
            is_op = 1;
            if (tokenType == SYMBOL) {          // op
                strcpy(oper, token);
            }
            compileTerm(fp, fw);
            writeArithmetic(fw, oper);
        }
    }
    if (!is_op)
        putBack();
}

void compileTerm(FILE *fp, FILE *fw)
{
    eat(fp);

    // if term is a variable, output "push v"
    if (tokenType == IDENTIFIER) {
        putBack();
        compileIdentifier(fp, fw, "varName");
        if (kindOf(token) == FIELD) {
            writePush(fw, __THIS, indexOf(token));
        } else if (kindOf(token) == STATIC) {
            writePush(fw, __STATIC, indexOf(token));
        } else if (kindOf(token) == VAR) {
            writePush(fw, __LOCAL, indexOf(token));
        } else if (kindOf(token) == ARG) {
            writePush(fw, __ARG, indexOf(token));
        }
    }
    // if term is a number n, output "push n"
    else if (tokenType == INT_CONST) {
        putBack();
        compileIntegerConstant(fp, fw);
        writePush(fw, __CONST, atoi(token));
    }
    // if term is a string, alloc space for string and then push each char iteratively
    else if (tokenType == STRING_CONST) {
        putBack();
        compileStringConstant(fp, fw);
        writePush(fw, __CONST, strlen(token));
        fprintf(fw, "call String.new 1\n");
        unsigned i;
        for (i = 0; i < strlen(token); i++) {
            writePush(fw, __CONST, token[i]);
            fprintf(fw, "call String.appendChar 2\n");
        }
    }
    // push the identical value for each keyword: true, false, this, null
    else if (tokenType == KEYWORD) {
        putBack();
        compileKeyword(fp, fw, 1, "keywordConstant");
        //printf("key %s\n", token);
        if (strcmp("true", token) == 0) {
            writePush(fw, __CONST, 0);
            fprintf(fw, "not\n");
        } else if (strcmp("false", token) == 0) {
            writePush(fw, __CONST, 0);
        } else if (strcmp("this", token) == 0) {
            writePush(fw, __POINTER, 0);
        } else if (strcmp("null", token) == 0) {
            writePush(fw, __CONST, 0);
        }
    }
    else if (strcmp("(", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "(");
        compileExpression(fp, fw);
        compileSymbol(fp, fw, ")");
    } else if (strcmp(unaryOp[0], token) == 0 || strcmp(unaryOp[1], token) == 0) {
        putBack();
        compileSymbol(fp, fw, "unaryOp");
        char temp[100];
        strcpy(temp, token);
        compileTerm(fp, fw);
        if (strcmp("-", temp) == 0)
            fprintf(fw, "neg\n");
        else if (strcmp("~", temp) == 0)
            writeArithmetic(fw, temp);
    }

    strcpy(LL1_TEMP, token);

    eat(fp);

    if (strcmp(".", token) == 0) {
        is_LL1 = 1;
        putBack();
        compileSubroutineCall(fp, fw);
    } else if (strcmp("[", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "[");
        compileExpression(fp, fw);

        writeArithmetic(fw, "+");
        writePop(fw, __POINTER, 1);
        writePush(fw, __THAT, 0);

        compileSymbol(fp, fw, "]");
    } else {
        putBack();
    }
}

void compileIf(FILE *fp, FILE *fw)
{
    if_counter++;
    int temp_counter = if_counter;
    putBack();
    compileKeyword(fp, fw, 1, "if");
    compileSymbol(fp, fw, "(");
    compileExpression(fp, fw);

    fprintf(fw, "if-goto IF_TRUE%d\n", if_counter);
    fprintf(fw, "goto IF_FALSE%d\n", if_counter);
    fprintf(fw, "label IF_TRUE%d\n", if_counter);

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
        fprintf(fw,  "goto IF_END%d\n", temp_counter);
        fprintf(fw, "label IF_FALSE%d\n", temp_counter);
        putBack();
        compileKeyword(fp, fw, 1, "else");
        compileSymbol(fp, fw, "{");
        compileStatements(fp, fw);
        fprintf(fw, "label IF_END%d\n", temp_counter);
        putBack();
        compileSymbol(fp, fw, "}");
    } else {
        fprintf(fw, "label IF_FALSE%d\n", temp_counter);
        putBack();
    }
}

void compileWhile(FILE *fp, FILE *fw)
{
    while_counter++;
    int temp = while_counter;
    fprintf(fw, "label WHILE_EXP%d\n", while_counter);

    putBack();
    compileKeyword(fp, fw, 1, "while");
    compileSymbol(fp, fw, "(");
    compileExpression(fp, fw);
    fprintf(fw, "not\n");
    fprintf(fw, "if-goto WHILE_END%d\n", while_counter);
    compileSymbol(fp, fw, ")");
    compileSymbol(fp, fw, "{");
    compileStatements(fp, fw);
    fprintf(fw, "goto WHILE_EXP%d\n", temp);
    fprintf(fw, "label WHILE_END%d\n", temp);
    // no need to get new token, compileStatement has done it to
    // check the ending
    putBack();
    compileSymbol(fp, fw, "}");
}

// before calling this method, you should check the token
// if the token is ")", then you should just print <expressionList></expressionList>
int compileExpressionList(FILE *fp, FILE *fw)
{
    int count = 1;
    compileExpression(fp, fw);

    eat(fp);

    // (',' expression)*
    while (strcmp(",", token) == 0) {
        putBack();
        count++;
        compileSymbol(fp, fw, ",");
        compileExpression(fp, fw);
        if (has_more_token(fp))
            advance(fp);
    }
    putBack();

    return count;
}

void compileSubroutineCall(FILE *fp, FILE *fw)
{
    char object[100];
    char subject[100];
    char subroutine[100];
    int count = 0;

    enum _funcType {
        SIMPLE,
        COMPLICATED
    } funcType;
    funcType = SIMPLE;

    // subroutineName | className
    eat(fp);

    // LL1_temp stores the identifier before ".", and the it can be both
    // the name of the Class, ex: Screen, or the name of the object, ex:
    // ball.
    if (is_LL1) {
        putBack();
        strcpy(subject, LL1_TEMP);
        is_LL1 = 0;
    }
    // the token just like LL1_temp can be either the name of the Class, ex: Screen,
    // or the name of the object, ex: ball.
    else if (tokenType == IDENTIFIER) {
        // the token is a variable or a field, such as bat or ball
        // the subject stores the type of the variable, therefore, it might be Bat or Ball
        if (typeOf(token)) {
            strcpy(subject, typeOf(token));
            strcpy(object, token);
            if (kindOf(token) == VAR) {
                writePush(fw, __LOCAL, indexOf(token));
            } else if (kindOf(token) == FIELD) {
                writePush(fw, __THIS, indexOf(token));
            }
        }
        // the subject is the name of the class, such as Screen
        else {
            //printf("function: name: %s\n", token);
            strcpy(subject, token);
        }
    }

    // could be '(' or '.'
    eat(fp);

    if (strcmp("(", token) == 0) {
        putBack();
        compileSymbol(fp, fw, "(");
    } else if (strcmp(".", token) == 0) {
        putBack();
        funcType = COMPLICATED;
        compileSymbol(fp, fw, ".");
        compileIdentifier(fp, fw, "subroutineName");
        strcpy(subroutine, token);
        compileSymbol(fp, fw, "(");
    }

    // push pointer before the expressionList pushing the variables*
    if (funcType == SIMPLE) {
        writePush(fw, __POINTER, 0);
    }

    eat(fp);
    if (strcmp(")", token) == 0) {
        putBack();
    } else {
        putBack();
        count = compileExpressionList(fp, fw);
    };

    switch(funcType) {
    case SIMPLE:
        fprintf(fw, "call %s.%s %d\n", className, subject, count+1);
        break;
    case COMPLICATED:
        if (kindOf(object) != NONE) {         // method
            fprintf(fw, "call %s.%s %d\n", subject, subroutine, count+1);
        } else if (kindOf(subject) != NONE) {
            fprintf(fw, "call %s.%s %d\n", typeOf(subject), subroutine, count+1);
        } else {
            fprintf(fw, "call %s.%s %d\n", subject, subroutine, count);
        }
        break;
    }

    compileSymbol(fp, fw, ")");
}
