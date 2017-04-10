#ifndef __CONSTANT_H
#define __CONSTANT_H

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

enum file_type {
    T_FILE,
    T_FOLDER
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
    "function", "return", "call"
};

const char *pop_2_args = "@SP\n"
                         "AM=M-1\n"
                         "D=M\n"
                         "@SP\n"
                         "AM=M-1\n"
                         "%s"
                         "@SP\n"
                         "M=M+1\n";

const char *set_constant = "@%d\n"
                           "D=A\n";

const char *fetch_sp_top = "@SP\n"
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

#endif
