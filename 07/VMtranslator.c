#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFERLEN       1024
#define ARITHMETICS_NUM 9
#define TYPES_NUM       8

enum command_type {
    C_ARITHMETIC,
    C_PUSH, C_POP,
    C_LABEL,
    C_GOTO, C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};
const char *default_register[] = {
    "SP", "LCL", "ARG", "THIS", "THAT"
};

const char *arithmetics[] = {
    "add", "sub",
    "lt", "eq", "gt",
    "neg", "and", "or", "not"
};
const char *types[] = {
    "push", "pop",
    "label", "goto", "if-goto",
    "function", "call", "return"
};

const char *pop_2_args = "@SP\n"
                         "AM=M-1\n"
                         "D=M\n"
                         "@SP\n"
                         "AM=M-1\n"
                         "%s"
                         "@SP\n"
                         "M=M+1\n";
static int lt_counter = 0;
static int eq_counter = 0;
static int gt_counter = 0;

const char *compare = "@SP\n"
                      "AM=M-1\n"
                      "D=M\n"
                      "@SP\n"
                      "AM=M-1\n"
                      "D=M-D\n"
                      "@true$%s$%d\n"     // %s=lt,eq,gt %d=number
                      "D;%s\n"           // JLT,JEQ, or JGT
                      "@SP\n"
                      "A=M\n"
                      "M=0\n"
                      "@end$%s$%d\n"      // number
                      "0;JMP\n"
                      "(true$%s$%d)\n"    // number
                      "@SP\n"
                      "A=M\n"
                      "M=-1\n"
                      "(end$%s$%d)\n"
                      "@SP\n"
                      "M=M+1\n";

FILE *fp, *fw;
char buffer[BUFFERLEN];
char filename[100];

// parser
char * parse_line(char *str, int len);
int commandType(char *line);
char *arg1(char *command, int type);
int arg2(char *command, int type);

// code generator
char *setFileName(const char *filename);
char *getFileName(char *name);
void writeArithmetic(char *str);
void writePushPop(int type, char *str, int index);

char *setFileName(const char *arg)
{
    strcpy(filename, arg);
    strcpy(buffer, arg);
    char *writeptr = buffer;
    while(*writeptr++ != '.') ;
    strcpy(writeptr, "asm");
    return buffer;
}

// get filename from path
char *getFileName(char *name)
{
    int last = 0;
    int i;
    for(i = 0; i < strlen(name); i++) {
        if (name[i] == '/')
            last = i;
    }
    if (last == 0)
        return name;
    char *ptr = &name[last]+1;
    strcpy(buffer, ptr);
    ptr = buffer;
    while(*ptr != '.')
        ptr++;
    *ptr = '\0';
    return buffer;
}

void writeArithmetic(char *str)
{
    const char *arith_content[] = {       // Since we fetch value from stack(which is LIFO), M is arg1, D is arg2
        "M=M+D\n",          // add
        "M=M-D\n",          // sub
        compare,          // lt
        compare,          // eq
        compare,          // gt
        "@SP\nA=M-1\nM=-M\n",          // neg
        "M=M&D\n",                     // and
        "M=M|D\n",                     // or
        "@SP\nA=M-1\nM=!M\n",          // not
    };
    if (strcmp(str, "add") == 0) {
        fprintf(fw, pop_2_args, arith_content[0]);
    }
    else if (strcmp(str, "sub") == 0) {
        fprintf(fw, pop_2_args, arith_content[1]);
    }
    else if (strcmp(str, "lt") == 0) {
        fprintf(fw, arith_content[2], "lt", lt_counter, "JLT",
                "lt", lt_counter, "lt", lt_counter, "lt", lt_counter);
        //static_counter++;
    }
    else if (strcmp(str, "eq") == 0) {
        fprintf(fw, arith_content[2], "eq", eq_counter, "JEQ",
                "eq", eq_counter, "eq", eq_counter, "eq", eq_counter);
        //static_counter++;
    }
    else if (strcmp(str, "gt") == 0) {
        fprintf(fw, arith_content[2], "gt", gt_counter, "JGT",
                "gt", gt_counter, "gt", gt_counter, "gt", gt_counter);
        //static_counter++;
    }
    else if (strcmp(str, "and") == 0) {
        fprintf(fw, pop_2_args, arith_content[6]);
    }
    else if (strcmp(str, "or") == 0) {
        fprintf(fw, pop_2_args, arith_content[7]);
    }
    else if (strcmp(str, "neg") == 0) {
        fprintf(fw, "%s", arith_content[5]);
    }
    else if (strcmp(str, "not") == 0) {
        fprintf(fw, "%s", arith_content[8]);
    }

}

// writes the assembly code that is the translation of the given
// arithmetic command
void writePushPop(int type, char *str, int index)
{
    const char *push_constant = "@%d\n"
        "D=A\n"
        "@SP\n"
        "A=M\n"
        "M=D\n"
        "@SP\n"
        "M=M+1\n";
    const char *up_push = "@%s\n"
                    "A=M\n";
    const char *down_push = "D=M\n"
                      "@SP\n"
                      "A=M\n"
                      "M=D\n"
                      "@SP\n"
                      "M=M+1\n";
    const char *push_temp_pointer = "@R%d\n";
    const char *pop_temp_pointer =  "@SP\n"
                              "A=M-1\n"
                              "D=M\n"
                              "@R%d\n"
                              "M=D\n"
                              "@SP\n"
                              "M=M-1\n";
    const char *up_pop =   "@SP\n"
                     "A=M-1\n"
                     "D=M\n"
                     "@%s\n"          // the string should be local, argument, this, that
                     "A=M\n";         // after A=M might be several A=A+1, considering the value of arg2
    const char *repeat = "A=A+1\n";
    const char *down_pop = "M=D\n"
                     "@SP\n"
                     "M=M-1\n";
    const char * map_1[] = {
        "local", "argument", "this", "that"
    };
    if (type == C_PUSH) {
        if (strcmp(str, "constant") == 0) {
            fprintf(fw, push_constant, index);
        } else {
            int i;
            for (i = 0; i < 4; i++) {
                if (strcmp(str, map_1[i]) == 0) {
                    strcpy(buffer, up_push);
                    int j;
                    for (j = 0; j < index; j++) {
                        strcat(buffer, repeat);
                    }
                    strcat(buffer, down_push);
                    fprintf(fw, buffer, default_register[i+1]); // skip SP
                }
            }
            if (strcmp(str, "temp") == 0) {
                strcpy(buffer, push_temp_pointer);
                strcat(buffer, down_push);
                fprintf(fw, buffer, 5+index);
            } else if (strcmp(str, "pointer") == 0) {
                strcpy(buffer, push_temp_pointer);
                strcat(buffer, down_push);
                fprintf(fw, buffer, 3+index);
            } else if (strcmp(str, "static") == 0) {
                //printf("%s\n", getFileName(filename));
                fprintf(fw, "@%s.%d\n", getFileName(filename), index);
                fprintf(fw, "%s", down_push);
            }
        }
    }
    else if (type == C_POP) {
        int i;
        for (i = 0; i < 4; i++) {
            if (strcmp(str, map_1[i]) == 0) {
                strcpy(buffer, up_pop);
                int j;
                for (j = 0; j < index; j++) {
                    strcat(buffer, repeat);
                }
                strcat(buffer, down_pop);
                fprintf(fw, buffer, default_register[i+1]); // skip SP
            }
        }
        if (strcmp(str, "temp") == 0) {
            strcpy(buffer, pop_temp_pointer);
            fprintf(fw, buffer, 5+index);
        } else if (strcmp(str, "pointer") == 0) {
            strcpy(buffer, pop_temp_pointer);
            fprintf(fw, buffer, 3+index);
        } else if (strcmp(str, "static") == 0) {
            char *static_pop = "@SP\n"
                               "A=M-1\n"
                               "D=M\n"
                               "@%s\n"        // @Xxx.index
                               "%s";
            sprintf(buffer, "%s.%d", getFileName(filename), index);
            fprintf(fw, static_pop, buffer, down_pop);
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>");
    else {
        fp = fopen(argv[1], "r");
        fw = fopen(setFileName(argv[1]), "w");

        char *lineptr = buffer;
        size_t linecap = BUFFERLEN;
        int i;
        while((i = getline(&lineptr, &linecap, fp)) > 0) {
            char *line = parse_line(lineptr, i);
            if (strlen(line) > 1) {
                int type = commandType(line);
                switch(type) {
                    case C_ARITHMETIC:
                        writeArithmetic(arg1(line, C_ARITHMETIC));
                        if (strcmp(arg1(line, C_ARITHMETIC), "lt") == 0)
                            lt_counter++;
                        if (strcmp(arg1(line, C_ARITHMETIC), "eq") == 0)
                            eq_counter++;
                        if (strcmp(arg1(line, C_ARITHMETIC), "gt") == 0)
                            gt_counter++;
//                         printf("arg1: %s\n", arg1(line, C_ARITHMETIC));
                        break;
                    case C_PUSH:
                        writePushPop(C_PUSH, arg1(line, C_PUSH), arg2(line, C_PUSH));
//                         printf("arg1: %s\n", arg1(line, C_PUSH));
//                         printf("arg2: %d\n", arg2(line, C_PUSH));
                        break;
                    case C_POP:
                        writePushPop(C_POP, arg1(line, C_PUSH), arg2(line, C_PUSH));
//                         printf("arg1: %s\n", arg1(line, C_POP));
//                         printf("arg2: %d\n", arg2(line, C_POP));
                        break;
                }
            }
        }
    }

    fclose(fp);
    fclose(fw);
    return 0;
}

// ignore all spaces, comments, and '\n'
char * parse_line(char *str, int len)
{
    char *command = malloc(sizeof(char) * len);
    while(*str == ' ')  // skip whitespace in front of the any text
        str++;
    if (*str == '/')
        return "";
    int j = 0;
    for (; j <len && isprint(*str); j++, str++) {
        command[j] = *str;
    }
    command[j] = '\0';

    return command;
}

// returns the type of the current VM command.
// C_ARITHMETIC is returned for all the arithmetic commands.
int commandType(char *line)
{
    int i;
    for (i = 0; i < ARITHMETICS_NUM; i++) {
        const char *temp = arithmetics[i];
        if (strncmp(line, temp, strlen(temp)) == 0)
            return C_ARITHMETIC;
    }
    for (i = 0; i < TYPES_NUM; i++) {
        const char *temp = types[i];
        if (strncmp(line, temp, strlen(temp)) == 0)
            return i+1;
    }
    // shouldn't go here
    return -1;
}

// returns the first argument of the current command
// ex: push constant 1
// then return constant
// In the case of C_ARITHMETIC, the command itself is returned.
char *arg1(char *command, int type)
{
    if (type == C_ARITHMETIC) {
        return command;
    }
    char *ptr = command;
    while(*ptr++ != ' ')
        ;
    int i;
    for (i = 0; i < strlen(command) && *ptr != ' '; i++, ptr++) {
        buffer[i] = *ptr;
    }
    buffer[i] = '\0';
    return buffer;
}

// returns the second argument of the current command
// should be called only if the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL
int arg2(char *command, int type)
{
    if (type == C_PUSH || type == C_POP || type == C_FUNCTION || type == C_CALL) {
        char *ptr = command;
        while(*ptr++ != ' ')
            ;
        while(*ptr++ != ' ')
            ;
        return atoi(ptr);
    }
    else {
        fprintf(stderr, "arg2 type error\n");
        return -1;
    }
}
