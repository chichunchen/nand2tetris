#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include "constant.h"

static int lt_counter = 0;
static int eq_counter = 0;
static int gt_counter = 0;
static int ret_call_counter = 0;
static int ret_addr_counter = 0;

char buffer[BUFFERLEN];
char filename[100];

// parser
int is_regular_file(const char *path);
int is_vmfile(const char *name);
char * fetch_filename(const char *path, const char *name);
char * parse_line(char *str, int len);
int commandType(char *line);
char *arg1(char *command, int type);
int arg2(char *command, int type);

// code generator
void code_generator(const char *vmfilename, FILE *fw);
char *setFileName(const char *filename, int type);
char *getFileName(char *name);
void writeArithmetic(char *str, FILE *fw);
void writePushPop(int type, char *str, int index, FILE *fw);
void writeInit(FILE *fw);
void writeLabel(char *label, FILE *fw);
void writeGoto(char *label, FILE *fw);
void writeIf(char *label, FILE *fw);
void writeCall(char *functionName, int numArgs, FILE *fw);
void writeReturn(FILE *fw);
void writeFunction(char *functionName, int numLocals, FILE *fw);

// set filename to path/xxx.asm
char *setFileName(const char *arg, int type)
{
    if (type == T_FILE) {
        strcpy(filename, arg);
        char *writeptr = filename;
        while(*writeptr++ != '.') ;
        strcpy(writeptr, "asm");
        //printf("sss %s\n", filename);
        return filename;
    } else {
        // extract the folder's name
        int last = 0;
        int index;
        for (index = 0; index < strlen(arg); index++) {
            if (arg[index] == '/' && isalnum(arg[index+1]) == 1)
                last = index;
        }
        strcpy(buffer, &arg[last+1]);
        if (buffer[strlen(buffer)-1] == '/') {
            buffer[strlen(buffer)-1] = '\0';
            strcat(buffer, ".asm");
            strcpy(filename, arg);
            strcat(filename, buffer);
        } else {
            strcat(buffer, ".asm");
            strcpy(filename, arg);
            strcat(filename, "/");
            strcat(filename, buffer);
        }
        //printf("sss %s\n", filename);
        return filename;
    }
}

// get xxx from path/xxx.asm
char *getFileName(char *name)
{
    char *temp = malloc(sizeof(char) * BUFFERLEN);
    int last = 0;
    int i;
    for(i = 0; i < strlen(name); i++) {
        if (name[i] == '/')
            last = i;
    }
    if (last == 0)
        return name;
    char *ptr = &name[last]+1;
    strcpy(temp, ptr);
    ptr = temp;
    while(*ptr != '.')
        ptr++;
    *ptr = '\0';
    return temp;
}

void writeInit(FILE *fw)
{
    const char *init_sp = "@256\n"
                          "D=A\n"
                          "@SP\n"
                          "M=D\n";
    fprintf(fw, "%s", init_sp);
    writeCall("Sys.init", 0, fw);
}

void writeArithmetic(char *str, FILE *fw)
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
    }
    else if (strcmp(str, "eq") == 0) {
        fprintf(fw, arith_content[2], "eq", eq_counter, "JEQ",
                "eq", eq_counter, "eq", eq_counter, "eq", eq_counter);
    }
    else if (strcmp(str, "gt") == 0) {
        fprintf(fw, arith_content[2], "gt", gt_counter, "JGT",
                "gt", gt_counter, "gt", gt_counter, "gt", gt_counter);
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
void writePushPop(int type, char *str, int index, FILE *fw)
{
    const char * map_1[] = {
        "local", "argument", "this", "that"
    };
    if (type == C_PUSH) {
        if (strcmp(str, "constant") == 0) {
            strcpy(buffer, set_constant);
            strcat(buffer, fetch_sp_top);
            fprintf(fw, buffer, index);
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
                char *name = getFileName(filename);
                fprintf(fw, "@%s.%d\n", name, index);
                fprintf(fw, "%s", down_push);
                free(name);
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
            char *name = getFileName(filename);
            sprintf(buffer, "%s.%d", name, index);
            fprintf(fw, static_pop, buffer, down_pop);
            free(name);
        }
    }
}

void writeLabel(char *label, FILE *fw)
{
    fprintf(fw, "(%s)\n", label);
}

void writeGoto(char *label, FILE *fw)
{
    fprintf(fw, "@%s\n", label);
    fprintf(fw, "0;JMP\n");
}

// Pop the stack's topmost value; if the value is not zero,
// execution continues from the location marked by the label; otherwise,
// execution continues from the next command in the program.
void writeIf(char *label, FILE *fw)
{
    const char *if_statement = "@SP\n"
                               "A=M-1\n"
                               "D=M\n"
                               "@SP\n"
                               "M=M-1\n"
                               "@%s\n"
                               "D;JNE\n";
    fprintf(fw, if_statement, label);
}

void writeCall(char *functionName, int numArgs, FILE *fw)
{
    // push return address
    const char *ret_addr_call = "RET_ADDRESS_CALL";
    const char *push_ret_addr = "@%s%d\n"
                                "D=A\n"
                                "@SP\n"
                                "A=M\n"
                                "M=D\n"
                                "@SP\n"
                                "M=M+1\n";
    fprintf(fw, push_ret_addr, ret_addr_call, ret_call_counter);

    // push LCL
    fprintf(fw, "@LCL\n%s", down_push);
    // push ARG
    fprintf(fw, "@ARG\n%s", down_push);
    // push THIS
    fprintf(fw, "@THIS\n%s", down_push);
    // push THAT
    fprintf(fw, "@THAT\n%s", down_push);
    // ARG = SP-n-5
    fprintf(fw, "@SP\nD=M\n");
    int j;
    for (j = 0; j < numArgs+5; j++) {
        fprintf(fw, "D=D-1\n");
    }
    fprintf(fw, "@ARG\nM=D\n");
    // LCL = SP
    const char *lcl_sp = "@SP\n"
                         "D=M\n"
                         "@LCL\n"
                         "M=D\n";
    fprintf(fw, "%s", lcl_sp);

    // goto f
    writeGoto(functionName, fw);

    // declare label
    //printf("lll %s\n", functionName);
    char ret_label[100];
    sprintf(ret_label, "%s%d", ret_addr_call, ret_call_counter);
    writeLabel(ret_label, fw);
    ret_call_counter++;
}

void writeReturn(FILE *fw)
{
    // push ret_address
    char ret_addr[100], frame[100];
    sprintf(frame, "FRAME_%d", ret_addr_counter);
    sprintf(ret_addr, "RET_ADDR_%d", ret_addr_counter);

    // frame = lcl
    const char *setframe = "@LCL\n"
                           "D=M\n"
                           "@%s\n" // frame
                           "M=D\n";
    fprintf(fw, setframe, frame);

    // ret = *(frame-5)
    const char *set_ret = "@%s\n"       // frame_#
                          "D=M\n"
                          "D=D-1\n"     // Frame-5
                          "D=D-1\n"
                          "D=D-1\n"
                          "D=D-1\n"
                          "D=D-1\n"
                          "A=D\n"       // point to &RAM[FRAME-5]
                          "D=M\n"
                          "@%s\n"
                          "M=D\n";
    fprintf(fw, set_ret, frame, ret_addr);

    // *ARG = pop()
    const char *set_ret_for_caller = "@SP\n"
                                     "A=M-1\n"
                                     "D=M\n"
                                     "@ARG\n"
                                     "A=M\n"
                                     "M=D\n"
                                     "@SP\n"
                                     "M=M-1\n";
    fprintf(fw, "%s", set_ret_for_caller);

    // SP = ARG+1
    const char *set_sp = "@ARG\n"
                         "D=M\n"
                         "D=D+1\n"
                         "@SP\n"
                         "M=D\n";
    fprintf(fw, "%s", set_sp);

    // THAT = *(FRAME-1)
    const char *set_that = "@%s\n"
                           "D=M\n"
                           "D=D-1\n"     //Frame-1
                           "A=D\n"       // point to &RAM[FRAME-1]
                           "D=M\n"
                           "@%s\n"
                           "M=D\n";
    fprintf(fw, set_that, frame, "THAT");

    // THIS = *(FRAME-2)
    const char *set_this = "@%s\n"
                           "D=M\n"
                           "D=D-1\n"     //Frame-2
                           "D=D-1\n"
                           "A=D\n"       // point to &RAM[FRAME-2]
                           "D=M\n"
                           "@%s\n"
                           "M=D\n";
    fprintf(fw, set_this, frame, "THIS");

    // ARG = *(FRAME-3)
    const char *set_arg =  "@%s\n"
                           "D=M\n"
                           "D=D-1\n"     //Frame-3
                           "D=D-1\n"
                           "D=D-1\n"
                           "A=D\n"       // point to &RAM[FRAME-3]
                           "D=M\n"
                           "@%s\n"
                           "M=D\n";
    fprintf(fw, set_arg, frame, "ARG");

    // LCL = *(FRAME-4)
    const char *set_lcl =  "@%s\n"
                           "D=M\n"
                           "D=D-1\n"     //Frame-4
                           "D=D-1\n"
                           "D=D-1\n"
                           "D=D-1\n"
                           "A=D\n"       // point to &RAM[FRAME-4]
                           "D=M\n"
                           "@%s\n"
                           "M=D\n";
    fprintf(fw, set_lcl, frame, "LCL");

    const char *goto_ret_addr = "@%s\n"
                                "A=M\n"  // jump to the content of ret_addr
                                "0;JMP\n";

    fprintf(fw, goto_ret_addr, ret_addr);
    ret_addr_counter++;
}

void writeFunction(char *functionName, int numLocals, FILE *fw)
{
    writeLabel(functionName, fw);
    int i;
    for(i = 0; i < numLocals; i++) {
        writePushPop(C_PUSH, "constant", 0, fw);
    }
}

void code_generator(const char *vmfilename, FILE *fw)
{
    FILE *fp = fopen(vmfilename, "r");

    char *lineptr = buffer;
    size_t linecap = BUFFERLEN;
    int i;
    while((i = getline(&lineptr, &linecap, fp)) > 0) {
        char *line = parse_line(lineptr, i);
        char *temp;
        if (strlen(line) > 1) {
            int type = commandType(line);
            switch(type) {
                case C_ARITHMETIC:
                    temp = arg1(line, C_ARITHMETIC);
                    writeArithmetic(temp, fw);
                    if (strcmp(temp, "lt") == 0)
                        lt_counter++;
                    if (strcmp(temp, "eq") == 0)
                        eq_counter++;
                    if (strcmp(temp, "gt") == 0)
                        gt_counter++;
                    free(temp);
                    break;
                case C_PUSH:
                    temp = arg1(line, C_PUSH);
                    writePushPop(C_PUSH, temp, arg2(line, C_PUSH), fw);
                    free(temp);
                    break;
                case C_POP:
                    temp = arg1(line, C_POP);
                    writePushPop(C_POP, arg1(line, C_POP), arg2(line, C_POP), fw);
                    free(temp);
                    break;
                case C_LABEL:
                    temp = arg1(line, C_LABEL);
                    writeLabel(temp, fw);
                    free(temp);
                    break;
                case C_GOTO:
                    temp = arg1(line, C_GOTO);
                    writeGoto(temp, fw);
                    free(temp);
                    break;
                case C_IF:
                    temp = arg1(line, C_IF);
                    writeIf(temp, fw);
                    free(temp);
                    break;
                case C_FUNCTION:
                    temp = arg1(line, C_FUNCTION);
                    writeFunction(temp, arg2(line, C_FUNCTION), fw);
                    free(temp);
                    break;
                case C_RETURN:
                    writeReturn(fw);
                    break;
                case C_CALL:
                    temp = arg1(line, C_CALL);
                    writeCall(temp, arg2(line, C_CALL), fw);
                    free(temp);
                    break;
            }
            free(line);
        }
    }
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>");
    else {
        if (is_regular_file(argv[1])) {
            FILE *fw = fopen(setFileName(argv[1], T_FILE), "w");
            code_generator(argv[1], fw);
            fclose(fw);
        } else {
            DIR *dirp;
            struct dirent *dp;
            dirp = opendir(argv[1]);
            // only one .asm for multiple files in the directory
            FILE *fw = fopen(setFileName(argv[1], T_FOLDER), "w");
            writeInit(fw);

            if (dirp == NULL) {
                fprintf(stderr, "opendir failed on '%s'", argv[1]);
                return EXIT_FAILURE;
            }

            for (;;) {
                errno = 0;
                dp = readdir(dirp);
                if (dp == NULL)
                    break;
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
                    continue;

                if (is_vmfile(dp->d_name)) {
                    // set filename for static segment
                    char *name = fetch_filename(argv[1], dp->d_name);
                    strcpy(filename, name);
                    code_generator(name, fw);
                    free(name);
                }
            }
            if (errno != 0) {
                fprintf(stderr, "readdir");
                return EXIT_FAILURE;
            }
            if (closedir(dirp) == -1) {
                fprintf(stderr, "closedir");
                return EXIT_FAILURE;
            }
            fclose(fw);
        }
    }

    return 0;
}

// 1 if it's a regular file
// 0 if not (maybe directory or device or sth else
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

// check whether the file is a vm file
int is_vmfile(const char *name)
{
    while(*name++ != '.' && *name) ;
    return strncmp(name, "vm", 3) == 0 ? 1 : 0;
}

char * fetch_filename(const char * path, const char * name)
{
    char *temp = malloc(sizeof(char) * BUFFERLEN);
    strcpy(temp, path);
    if (temp[strlen(temp)-1] != '/') {
        strcat(temp, "/");
    }
    strcat(temp, name);
    return temp;
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
    char *temp = malloc(sizeof(char)*BUFFERLEN);
    // ignore comments
    if (type == C_ARITHMETIC || type == C_RETURN) {
        int i;
        int len = strlen(command);
        for (i = 0; command[i] != ' ' && i < len; i++) {
            temp[i] = command[i];
        }
        temp[i] = '\0';
        return temp;
    }
    char *ptr = command;
    while(*ptr++ != ' ')
        ;
    int i;
    for (i = 0; i < strlen(command) && *ptr != ' '; i++, ptr++) {
        temp[i] = *ptr;
    }
    temp[i] = '\0';
    return temp;
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
