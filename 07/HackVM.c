#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFERLEN 1024
#define TOK_LEN   16

enum command_type {
    C_ARITHMETIC,
    C_PUSH, C_POP,
    C_LABEL,
    C_GOTO, C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

FILE *fp, *fw;
char buffer[BUFFERLEN];

// parser
char * parse_line(char *str, int len);
int commandType(char *line);
char *arg1(char *command, int len);
char *arg2(char *command, int len);

int main(int argc, const char *argv[])
{
    if (argc < 2)
        printf("usage: ./a.out <name of file or directory>");
    else {
        fp = fopen(argv[1], "r");
        char *lineptr = buffer;
        size_t linecap = BUFFERLEN;
        int i;
        while((i = getline(&lineptr, &linecap, fp)) > 0) {
            char *line = parse_line(lineptr, i);
            if (strlen(line) > 1) {
                int type = commandType(line);
                switch(type) {
                    case C_ARITHMETIC:
                        printf("arg1: %s\n", arg1(line, strlen(line)));
                        break;
                    case C_PUSH:
                        printf("arg1: %s\n", arg1(line, strlen(line)));
                        printf("arg2: %s\n", arg2(line, strlen(line)));
                        break;
                    case C_POP:
                        printf("arg1: %s\n", arg1(line, strlen(line)));
                        printf("arg2: %s\n", arg2(line, strlen(line)));
                        break;
                }
            }
        }
    }

    //fclose(fp);
    //fclose(fw);
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
    char *arithmetics[] = {
        "add", "sub",
        "lt", "eq", "gt",
        "neg", "and", "or", "not"
    };
    char *types[] = {
        "push", "pop",
        "label", "goto", "if-goto",
        "function", "call", "return"
    };
    int i;
    for (i = 0; i < 9; i++) {
        char *temp = arithmetics[i];
        if (strncmp(line, temp, strlen(temp)) == 0)
            return C_ARITHMETIC;
    }
    for (i = 0; i < 8; i++) {
        char *temp = types[i];
        if (strncmp(line, temp, strlen(temp)) == 0)
            return i+1;
    }
    // shouldn't go here
    return -1;
}

// returns the first argument of the current command
char *arg1(char *command, int len)
{
    int i;
    for (i = 0; i < len && *command != ' '; i++, command++) {
        buffer[i] = *command;
    }
    buffer[i] = '\0';
    return buffer;
}

// returns the second argument of the current command
char *arg2(char *command, int len)
{
    char *ptr = command;
    while(*ptr++ != ' ')
        ;
    int i;
    for (i = 0; i < len && *ptr != ' '; i++, ptr++) {
        buffer[i] = *ptr;
    }
    buffer[i] = '\0';
    return buffer;
}
