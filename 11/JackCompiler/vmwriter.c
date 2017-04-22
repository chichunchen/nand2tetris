#include "helper.h"
#include "constant.h"

void writeArithmetic(FILE *fw, char *command)
{
    if (strcmp(command, "+") == 0) {
        fprintf(fw, "add\n");
    } else if (strcmp(command, "-") == 0) {
        fprintf(fw, "sub\n");
    } else if (strcmp(command, "*") == 0) {
        writeCall(fw, "Math.multiply", 2);
    }
}

void writeCall(FILE *fw, char *name, int nLocals)
{
    fprintf(fw, "call %s %d\n", name, nLocals);
}
