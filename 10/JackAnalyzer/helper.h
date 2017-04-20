#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>

enum tokenType {
    KEYWORD, SYMBOL,
    IDENTIFIER, INT_CONST,
    STRING_CONST
};

enum keyword {
    CLASS, METHOD, FUNCTION,
    CONSTRUCTOR, _INT,
    _BOOLEAN, _CHAR, _VOID,
    VAR, STATIC, FIELD, LET,
    DO, _IF, _ELSE, _WHILE,
    _RETURN, _TRUE, _FALSE,
    _NULL, _THIS
};

const char* keyword[] = {
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

const char* op[] = {
    "+", "-", "*", "/", "&amp;", "|", "&lt;", "&gt;", "=",
};

const char* unaryOp[] = {
    "-", "~"
};

const char *keywordConstant[] = {
    "true", "false", "null", "this"
};

/* Utilities */
int is_directory(const char *filename);
char *setFileName(const char *arg);

/* JackTokenizer Module */

int has_more_token(FILE *fp);
void putBack();
void advance(FILE *fp);
int keyWord();
void tokenTest(FILE *fp, FILE *fw);

/* Compilation Module */

// terminal
void compileKeyword(FILE *fp, FILE *fw, int options, char *info);
void compileSymbol(FILE *fp, FILE *fw, char *info);
void compileIntegerConstant(FILE *fp, FILE *fw);
void compileStringConstant(FILE *fp, FILE *fw);
void compileIdentifier(FILE *fp, FILE *fw, char *info);

// nonterminal
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
void compileSubroutineCall(FILE *fp, FILE *fw);
