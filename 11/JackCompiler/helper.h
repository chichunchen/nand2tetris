#ifndef _HELPER_H
#define _HELPER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include "dbg.h"

#define MAXTOKEN    100
#define MAXFILE     256
#define KEYWORD_NUM 21
#define SYMBOL_NUM  19
#define OP_NUM      9

#define HASHSIZE    1000

/* Gloabal Variables */

extern int putback;
extern int is_LL1;
extern char token[MAXTOKEN];
extern int tokenType;

/* Utilities */

int is_directory(const char *filename);
char *setFileName(const char *arg);

/* JackTokenizer Module */

#define putBack() ((putback=1))
int has_more_token(FILE *fp);
void advance(FILE *fp);
int eat(FILE *fp);
int keyWord();
void tokenTest(FILE *fp, FILE *fw);

/* Compilation Module */

// compiling terminal
void compileKeyword(FILE *fp, FILE *fw, int options, char *info);
void compileSymbol(FILE *fp, FILE *fw, char *info);
void compileIntegerConstant(FILE *fp, FILE *fw);
void compileStringConstant(FILE *fp, FILE *fw);
void compileIdentifier(FILE *fp, FILE *fw, char *info);

// compiling nonterminal
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
int  compileExpressionList(FILE *fp, FILE *fw);
void compileSubroutineCall(FILE *fp, FILE *fw);

/* Symbol Table */

struct nlist *Define(int choice, char *name, char* type, int kind, int index);
int varCount(int choice, int kind);
int kindOf(char *name);
char *typeOf(char *name);
int indexOf(char *name);
void cleanSubroutineTab();
void cleanClassTab();
void symbolTest();
void printClassTable();
void printSubTable();

/* VM writer */
void writePush(FILE *fw, int vmtype, int index);
void writePop(FILE *fw, int vmtype, int index);
void writeArithmetic(FILE *fw, char *command);
void writeLabel(FILE *fw, char *label);
void writeGoto(FILE *fw, char *label);
void writeIf(FILE *fw, char *label);
void writeCall(FILE *fw, char *name, int nLocals);
void writeFunction(FILE *fw, char *name, int nLocals);
void writeReturn(FILE *fw);

#endif
