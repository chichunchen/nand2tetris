#include "helper.h"
#include "constant.h"

/* terminals */

int objSize = 0;

char className[100];
char LL1_TEMP[100];

int if_counter = -1;
int while_counter = -1;

void inline compileKeyword(FILE *fp, FILE *fw, int options, char *info)
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

void inline compileSymbol(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            //fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }
}

void inline compileIntegerConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        //fprintf(fw, "<integerConstant> %s </integerConstant>\n", token);
    }
}

void inline compileStringConstant(FILE *fp, FILE *fw)
{
    if (has_more_token(fp)) {
        advance(fp);
        //fprintf(fw, "<stringConstant> %s </stringConstant>\n", token);
    }
}

void inline compileIdentifier(FILE *fp, FILE *fw, char *info)
{
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == IDENTIFIER) {
            //fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
    }
}

/* non-terminals */

void compileClass(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<class>\n");
    if(has_more_token(fp)) {
        advance(fp);
        if (strcmp(token, "class") == 0) {
            //fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
        tokenType = -1;
    }

    compileIdentifier(fp, fw, "className");
    strcpy(className, token);
    compileSymbol(fp, fw, "{");

    // classVarDec*
    while (has_more_token(fp)) {
        advance(fp);
        if (keyWord() == _STATIC || keyWord() == _FIELD) {
            compileClassVarDec(fp, fw);
        } else {
            break;
        }
    }

    // subroutineDec*
    do {
        if (keyWord() == _CONSTRUCTOR || keyWord() == _FUNCTION || keyWord() == _METHOD) {
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
    printClassTable();

    //fprintf(fw, "</class>\n");
}

void compileClassVarDec(FILE *fp, FILE *fw)
{
    char type[BUFSIZ];
    int kind = -1;
    //fprintf(fw, "<classVarDec>\n");

    // token has been extracted to check whether class should call VarDec or subRoutine
    //if (strcmp("static", token) == 0 || strcmp("field", token) == 0) {
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
    //}
    if (keyWord() == _STATIC) {
        kind = STATIC;
    } else if (keyWord() == _FIELD) {
        kind = FIELD;
    }

    if (has_more_token(fp)) {       // type
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                //fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        } else if (tokenType == IDENTIFIER) {
            //fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
        strcpy(type, token);
    }
    compileIdentifier(fp, fw, "varName");
    Define(CLASS, token, type, kind, varCount(CLASS, kind));
    objSize++;

    // ((',' varName)*
    if (has_more_token(fp)) {
        advance(fp);
        while(strcmp(";", token) != 0) {
            putBack();
            compileSymbol(fp, fw, ",");
            compileIdentifier(fp, fw, "varName");
            Define(CLASS, token, type, kind, varCount(CLASS, kind));
            objSize++;

            if (has_more_token(fp)) {
                advance(fp);
            }
        }
        putBack();
        compileSymbol(fp, fw, ";");
    }
    //fprintf(fw, "</classVarDec>\n");
}

void compileSubroutine(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<subroutineDec>\n");
    char funcType[BUFSIZ];
    char subroutinName[BUFSIZ];

    putBack();
    compileKeyword(fp, fw,
                    keyWord() == _CONSTRUCTOR || keyWord() == _FUNCTION || keyWord() == _METHOD,
                    "constructor|function|method");
    strcpy(funcType, token);

    if (has_more_token(fp)) {       // identifier | void | type
        advance(fp);
        if (tokenType == IDENTIFIER) {
            //fprintf(fw, "<identifier> %s </identifier>\n", token);
        }
        else if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _VOID || keyWord() == _BOOLEAN) {
            //fprintf(fw, "<keyword> %s </keyword>\n", token);
        }
    }

    compileIdentifier(fp, fw, "subroutineName");
    strcpy(subroutinName, token);
    compileSymbol(fp, fw, "(");

    compileParameterList(fp, fw);

    putBack();
    compileSymbol(fp, fw, "}");

    // subroutineBody
    //fprintf(fw, "<subroutineBody>\n");
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

    fprintf(fw, "function %s.%s %d\n", className, subroutinName, varCount(SUBROUTINE, VAR));
    if (strcmp("constructor", funcType) == 0) {
        writePush(fw, __CONST, objSize);
        fprintf(fw, "call Memory.alloc %d\n", 1);
        writePop(fw, __POINTER, 0);
    } else if (strcmp("method", funcType) == 0) {
        writePush(fw, __ARG, 0);
        writePop(fw, __POINTER, 0);
    } else if (strcmp("function", funcType) == 0) {

    }

    compileStatements(fp, fw);
    putBack();
    compileSymbol(fp, fw, "}");

    printSubTable();
    cleanSubroutineTab();
    if_counter = -1;
    while_counter = -1;
    //fprintf(fw, "</subroutineBody>\n");
    //fprintf(fw, "</subroutineDec>\n");
}

void compileParameterList(FILE *fp, FILE *fw)
{
    char type[BUFSIZ];
    //fprintf(fw, "<parameterList>\n");

    if (has_more_token(fp)) {           // type for parameter
        advance(fp);
        if (tokenType == KEYWORD || tokenType == IDENTIFIER) {
            if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                //fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
            strcpy(type, token);
        } else if (strcmp(")", token) == 0) {   // no parameter
            //fprintf(fw, "</parameterList>\n");
            return;
        }
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

    //fprintf(fw, "</parameterList>\n");
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
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            while (strcmp(";", token) != 0) {
                // (',' varName)*
                //fprintf(fw, "<symbol> %s </symbol>\n", token);      // ','
                compileIdentifier(fp, fw, "varName");
                Define(SUBROUTINE, token, type, VAR, varCount(SUBROUTINE, VAR));
                if (has_more_token(fp))
                    advance(fp);
            }
            // ;
            //fprintf(fw, "<symbol> %s </symbol>\n", token);
        }
    }

    //fprintf(fw, "</varDec>\n");
}

void compileStatements(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<statements>\n");
    // since we check token == var or not to determine the ending of varDec
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
    int is_array = 0;
    //fprintf(fw, "<letStatement>\n");

    if (tokenType == KEYWORD) {     // let
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
    }

    compileIdentifier(fp, fw, "varName");
    strcpy(varName, token);

    if (has_more_token(fp)) {
        advance(fp);
    }

    // ('[' expression ']')?
    if (strcmp("[", token) == 0) {
        putBack();

        compileSymbol(fp, fw, "[");
        // expression 1
        compileExpression(fp, fw);

        if (kindOf(varName) == VAR) {
            writePush(fw, __LOCAL, indexOf(varName));
        } else if (kindOf(varName) == FIELD) {
            writePush(fw, __THIS, indexOf(varName));
        } else if (kindOf(varName) == STATIC) {
            writePush(fw, __STATIC, indexOf(varName));
        }

        writeArithmetic(fw, "+");
        is_array = 1;

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
        is_array = 0;
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

    //fprintf(fw, "</letStatement>\n");
}

void compileDo(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<doStatement>\n");

    putBack();
    compileKeyword(fp, fw, 1, "do");

    compileSubroutineCall(fp, fw);
    compileSymbol(fp, fw, ";");

    // dummy data for return
    writePop(fw, __TEMP, 0);

    //fprintf(fw, "</doStatement>\n");
}

void compileReturn(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<returnStatement>\n");
    if (tokenType == KEYWORD) {         // return
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
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
    } else {
        writePush(fw, __CONST, 0);
    }

    fprintf(fw, "return\n");
    putBack();
    compileSymbol(fp, fw, ";");

    //fprintf(fw, "</returnStatement>\n");
}

void compileExpression(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<expression>\n");

    compileTerm(fp, fw);

    // (op term)
    if (has_more_token(fp)) {                   // if not op, should putback
        advance(fp);
    }
    int i;
    int is_op = 0;
    char oper[100];
    for (i = 0; i < OP_NUM; i++) {
        if (strcmp(op[i], token) == 0) {        // check op?
            is_op = 1;
            if (tokenType == SYMBOL) {          // op
                strcpy(oper, token);
            }
                //fprintf(fw, "<symbol> %s </symbol>\n", token);
            compileTerm(fp, fw);
            writeArithmetic(fw, oper);
        }
    }
    if (!is_op)
        putBack();

    //fprintf(fw, "</expression>\n");
}

void compileTerm(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<term>\n");
    if (has_more_token(fp)) {
        advance(fp);
    }

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
    } else if (tokenType == INT_CONST) {
        putBack();
        compileIntegerConstant(fp, fw);
        writePush(fw, __CONST, atoi(token));
    } else if (tokenType == STRING_CONST) {
        putBack();
        compileStringConstant(fp, fw);
        writePush(fw, __CONST, strlen(token));
        fprintf(fw, "call String.new 1\n");
        int i;
        for (i = 0; i < strlen(token); i++) {
            writePush(fw, __CONST, token[i]);
            fprintf(fw, "call String.appendChar 2\n");
        }
    } else if (tokenType == KEYWORD) {
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
        }
    } else if (strcmp("(", token) == 0) {
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
            fprintf(fw, "not\n");
    }

    strcpy(LL1_TEMP, token);

    if (has_more_token(fp))                                 // check ahead for LL(1)
        advance(fp);

    if (strcmp(".", token) == 0) {
        is_LL1 = 1;
        putBack();
        compileSubroutineCall(fp, fw);
    } else if (strcmp("[", token) == 0) {
        //writeArithmetic(fw, "+");
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

    //fprintf(fw, "</term>\n");
}

void compileIf(FILE *fp, FILE *fw)
{
    if_counter++;
    //fprintf(fw, "<ifStatement>\n");
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

    //fprintf(fw, "goto IF_END%d\n", if_counter);

    // no need to get new token, compileStatement has done it to
    // check the ending
    putBack();
    compileSymbol(fp, fw, "}");
    // check else
    if (has_more_token(fp)) {
        advance(fp);
    }

    if (strcmp("else", token) == 0) {
        fprintf(fw,  "goto IF_END%d\n", if_counter);
        fprintf(fw, "label IF_FALSE%d\n", if_counter);
        putBack();
        compileKeyword(fp, fw, 1, "else");
        compileSymbol(fp, fw, "{");
        compileStatements(fp, fw);
        fprintf(fw, "label IF_END%d\n", if_counter);
        putBack();
        compileSymbol(fp, fw, "}");
    } else {
        fprintf(fw, "label IF_FALSE%d\n", if_counter);
        putBack();
    }
    //if_counter--;
    //fprintf(fw, "</ifStatement>\n");
}

void compileWhile(FILE *fp, FILE *fw)
{
    while_counter++;
    int temp = while_counter;
    fprintf(fw, "label WHILE_EXP%d\n", while_counter);

    //fprintf(fw, "<whileStatement>\n");

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

    //fprintf(fw, "</whileStatement>\n");
}

// before calling this method, you should check the token
// if the token is ")", then you should just print <expressionList></expressionList>
int compileExpressionList(FILE *fp, FILE *fw)
{
    int count = 1;
    //fprintf(fw, "<expressionList>\n");
    compileExpression(fp, fw);

    if (has_more_token(fp)) {
        advance(fp);
    }

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

    //fprintf(fw, "</expressionList>\n");
    return count;
}

void compileSubroutineCall(FILE *fp, FILE *fw)
{
    char funcName[100];
    char subroutine[100];
    int count = 0;
    enum _funcType {
        SIMPLE,
        COMPLICATED
    } funcType;
    funcType = SIMPLE;

    if (has_more_token(fp)) {       // subroutineName | className
        advance(fp);
    }

    if (is_LL1) {
        putBack();
        strcpy(funcName, LL1_TEMP);
        is_LL1 = 0;
    }
    else if (tokenType == IDENTIFIER) {
        //fprintf(fw, "<identifier> %s </identifier>\n", token);
        if (typeOf(token)) {
            //printf("method: name: %s, type: %s, kind: %d, index: %d\n", token, typeOf(token), kindOf(token), indexOf(token));
            strcpy(funcName, typeOf(token));
            if (kindOf(token) == VAR) {
                writePush(fw, __LOCAL, indexOf(token));
            } else if (kindOf(token) == FIELD) {
                writePush(fw, __THIS, indexOf(token));
            }
            count++;
        } else {
            //printf("function: name: %s\n", token);
            strcpy(funcName, token);
        }
    }


    if (has_more_token(fp)) {                   // could be '(' or '.'
        advance(fp);
    }

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

    if (has_more_token(fp)) {
        advance(fp);
    }
    if (strcmp(")", token) == 0) {
        //fprintf(fw, "<expressionList>\n</expressionList>\n");
        putBack();
    } else {
        putBack();
        count = compileExpressionList(fp, fw);
    }

    switch(funcType) {
    case SIMPLE:
        // must be method
        writePush(fw, __POINTER, 0);
        fprintf(fw, "call %s.%s %d\n", className, funcName, count+1);
        break;
    case COMPLICATED:
        fprintf(fw, "call %s.%s %d\n", funcName, subroutine, count);
        break;
    }

    compileSymbol(fp, fw, ")");
}
