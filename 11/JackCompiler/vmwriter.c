#include "helper.h"
#include "constant.h"

void writePush(FILE *fw, int vmtype, int index)
{
    switch(vmtype) {
    case __CONST:
        fprintf(fw, "push constant %d\n", index);
        break;
    case __ARG:
        fprintf(fw, "push argument %d\n", index);
        break;
    case __LOCAL:
        fprintf(fw, "push local %d\n", index);
        break;
    case __STATIC:
        fprintf(fw, "push static %d\n", index);
        break;
    case __THIS:
        fprintf(fw, "push this %d\n", index);
        break;
    case __THAT:
        fprintf(fw, "push that %d\n", index);
        break;
    case __POINTER:
        fprintf(fw, "push pointer %d\n", index);
        break;
    case __TEMP:
        fprintf(fw, "push temp %d\n", index);
        break;
    }
}

void writePop(FILE *fw, int vmtype, int index)
{
    switch(vmtype) {
    case __ARG:
        fprintf(fw, "pop argument %d\n", index);
        break;
    case __LOCAL:
        fprintf(fw, "pop local %d\n", index);
        break;
    case __STATIC:
        fprintf(fw, "pop static %d\n", index);
        break;
    case __THIS:
        fprintf(fw, "pop this %d\n", index);
        break;
    case __THAT:
        fprintf(fw, "pop that %d\n", index);
        break;
    case __POINTER:
        fprintf(fw, "pop pointer %d\n", index);
        break;
    case __TEMP:
        fprintf(fw, "pop temp %d\n", index);
        break;
    }
}

void writeArithmetic(FILE *fw, char *command)
{
    if (strcmp(command, "+") == 0) {
        fprintf(fw, "add\n");
    } else if (strcmp(command, "-") == 0) {
        fprintf(fw, "sub\n");
    } else if (strcmp(command, "*") == 0) {
        fprintf(fw, "call Math.multiply 2\n");
    } else if (strcmp(command, "/") == 0) {
        fprintf(fw, "call Math.divide 2\n");
    } else if (strcmp(command, "&gt;") == 0) {
        fprintf(fw, "gt\n");
    } else if (strcmp(command, "&lt;") == 0) {
        fprintf(fw, "lt\n");
    } else if (strcmp(command, "=") == 0) {
        fprintf(fw, "eq\n");
    } else if (strcmp(command, "&amp;") == 0) {
        fprintf(fw, "and\n");
    } else if (strcmp(command, "|") == 0) {
        fprintf(fw, "or\n");
    } else if (strcmp(command, "~") == 0) {
        fprintf(fw, "not\n");
    }
}

void writeLabel(FILE *fw, char *label)
{
    fprintf(fw, "label %s\n", label);
}

void writeGoto(FILE *fw, char *label)
{

}

void writeIf(FILE *fw, char *label)
{

}

void writeCall(FILE *fw, char *name, int nLocals)
{
    fprintf(fw, "call %s %d\n", name, nLocals);
}

void writeFunction(FILE *fw, char *name, int nLocals)
{
    fprintf(fw, "function %s %d\n", name, nLocals);
}

void writeReturn(FILE *fw)
{
    fprintf(fw, "return\n");
}
