#include "helper.h"
#include "constant.h"

/* terminals */

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

    //fprintf(fw, "</class>\n");
}

void compileClassVarDec(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<classVarDec>\n");

    // token has been extracted to check whether class should call VarDec or subRoutine
    if (strcmp("static", token) == 0 || strcmp("field", token) == 0) {
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
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
    }
    compileIdentifier(fp, fw, "varName");

    // ((',' varName)*
    if (has_more_token(fp)) {
        advance(fp);
        while(strcmp(";", token) != 0) {
            putBack();
            compileSymbol(fp, fw, ",");
            compileIdentifier(fp, fw, "varName");

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

    putBack();
    compileKeyword(fp, fw,
                    keyWord() == _CONSTRUCTOR || keyWord() == _FUNCTION || keyWord() == _METHOD,
                    "constructor|function|method");

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

    compileStatements(fp, fw);
    putBack();
    compileSymbol(fp, fw, "}");

    //fprintf(fw, "</subroutineBody>\n");
    //fprintf(fw, "</subroutineDec>\n");
}

void compileParameterList(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<parameterList>\n");

    if (has_more_token(fp)) {           // type for parameter
        advance(fp);
        if (tokenType == KEYWORD) {
            if (keyWord() == _INT || keyWord() == _CHAR || keyWord() == _BOOLEAN) {
                //fprintf(fw, "<keyword> %s </keyword>\n", token);
            }
        } else if (strcmp(")", token) == 0) {   // no parameter
            //fprintf(fw, "</parameterList>\n");
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
                    putBack();
                    compileKeyword(fp, fw, 1, "primitive type");
                } else if (tokenType == IDENTIFIER) {
                    putBack();
                    compileIdentifier(fp, fw, "custom type");
                }
            }
            compileIdentifier(fp, fw, "varName");
        } else {
            break;
        }
    }

    //fprintf(fw, "</parameterList>\n");
}

// 'var' type varName (',' varName)* ';'
void compileVarDec(FILE *fp, FILE *fw)
{
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
    }

    compileIdentifier(fp, fw, "varName");

    // it can be ',' or ';'
    if (has_more_token(fp)) {
        advance(fp);
        if (tokenType == SYMBOL) {
            while (strcmp(";", token) != 0) {
                // (',' varName)*
                //fprintf(fw, "<symbol> %s </symbol>\n", token);      // ','
                compileIdentifier(fp, fw, "varName");
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
    //fprintf(fw, "<letStatement>\n");

    if (tokenType == KEYWORD) {     // let
        //fprintf(fw, "<keyword> %s </keyword>\n", token);
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

    //fprintf(fw, "</letStatement>\n");
}

void compileDo(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<doStatement>\n");

    putBack();
    compileKeyword(fp, fw, 1, "do");

    compileSubroutineCall(fp, fw);
    compileSymbol(fp, fw, ";");

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
    }

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
    for (i = 0; i < OP_NUM; i++) {
        if (strcmp(op[i], token) == 0) {        // check op?
            is_op = 1;
            if (tokenType == SYMBOL)            // op
                //fprintf(fw, "<symbol> %s </symbol>\n", token);
            compileTerm(fp, fw);
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

    //fprintf(fw, "</term>\n");
}

void compileIf(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<ifStatement>\n");
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

    //fprintf(fw, "</ifStatement>\n");
}

void compileWhile(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<whileStatement>\n");

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

    //fprintf(fw, "</whileStatement>\n");
}

// before calling this method, you should check the token
// if the token is ")", then you should just print <expressionList></expressionList>
void compileExpressionList(FILE *fp, FILE *fw)
{
    //fprintf(fw, "<expressionList>\n");

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

    //fprintf(fw, "</expressionList>\n");
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
        //fprintf(fw, "<identifier> %s </identifier>\n", token);
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
        //fprintf(fw, "<expressionList>\n</expressionList>\n");
        putBack();
    } else {
        putBack();
        compileExpressionList(fp, fw);
    }

    compileSymbol(fp, fw, ")");
}
