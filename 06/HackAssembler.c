#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "dbg.h"

#define DEFAULT_START 16
#define BUFFERLEN 100
#define ENTRYNUM  24577
#define BITS_NUM    16
#define TOK_LEN     16
#define DESTTAB_LEN 8
#define COMPTAB_LEN 18
#define JUMPTAB_LEN 8

char buffer[BUFFERLEN];

enum command { A_COMMAND, C_COMMAND, L_COMMAND };
struct symbol {      // table entry
    char label[100];
    int address;
};

int registerptr;
int symboltabptr;
struct symbol symboltab[ENTRYNUM];

// parser module
void initializer();
int commandType(char *line, int lim);
void addEntry(char *label, int address);
bool contains(char *label);
int getAddress(char *symbol);

// code module
char * convertA(unsigned int in, char *out);
void printTable();
char * convertDest(char *src);
char * convertComp(char *src, int len, int * a);
char * convertJMP(char *src);
char * convertC(char *instruction);

// initialize the symbol table with all the predefined symbols and
// their pre-allocated RAM addresses
void initializer()
{
    symboltabptr = 0;       // ptr for symbol table
    char labels[7][10] = {
        "SP", "LCL", "ARG", "THIS", "THAT", "SCREEN", "KBD"
    };
    int addresses[7] = { 0, 1, 2, 3, 4, 16384, 24576 };
    int i;
    for (i = 0; i < 7; i++) {
        addEntry(labels[i], addresses[i]);
        symboltabptr++;
    }
    // set default registers
    for (i = 0; i < 16; i++) {
        char *virtual_label = malloc(sizeof(char) * 4);
        virtual_label[0] = 'R';
        if (i < 10) {
            virtual_label[1] = '0' + i;
            virtual_label[2] = '\0';
        } else {
            virtual_label[1] = '0' + i/10;
            virtual_label[2] = '0' + i%10;
            virtual_label[3] = '\0';
        }
        addEntry(virtual_label, i);
    }
    registerptr = DEFAULT_START;
}

int commandType(char *line, int lim)
{
    int i;
    char *ptr = line;
    int l_brace = 0;

    for (i = 0; i < lim-1 && *ptr; i++) {
        if (*ptr == '@')
            return A_COMMAND;
        else if (*ptr == '(')
            l_brace = 1;
        else if (*ptr == ')' && l_brace)
            return L_COMMAND;
        ptr++;
    }
    return C_COMMAND;
}

bool contains(char *label)
{
    int i;
    for (i = 0; i < ENTRYNUM; i++) {
        //printf("compare %s\n", symboltab[i].label);
        if (strncmp(label, symboltab[i].label, strlen(label)-1) == 0) { // ignore the null terminator, it might be '\n' or EOF
            //printf("Has set up.\n");
            return true;
        }
    }
    return false;
}

void addEntry(char *label, int address)
{
    strcpy(symboltab[symboltabptr].label, label);
    symboltab[symboltabptr].address = address;
    symboltabptr++;
}

// Returns the address associated with the symbol
// if cannot find one, add the symbol into symboltab, and retrun the address
int getAddress(char *symbol)
{
    int i;
    for (i = 0; i < symboltabptr; i++) {
        if (strcmp(symboltab[i].label, symbol) == 0)
            return symboltab[i].address;
    }
    addEntry(symbol, registerptr);
    return registerptr++;
}

void printTable()
{
    int j;
    for (j = 0; j < ENTRYNUM; j++) {
        if (isalnum(symboltab[j].label[0])) {
            printf("T: %d, %s\n", symboltab[j].address, symboltab[j].label);
        }
    }
}

// 16 -> 0000000000010000
char * convertA(unsigned int in, char *out)
{
    unsigned int mask = 1U << (BITS_NUM-1);
    int i;
    for (i = 0; i < BITS_NUM; i++) {
        out[i] = (in & mask) ? '1' : '0';
        in <<= 1;
    }
    return out;
}

char * convertDest(char *src)
{
    char * desttab[DESTTAB_LEN] = {
        "null", "M", "D", "MD", "A", "AM", "AD", "AMD"
    };
    char * destBintab[DESTTAB_LEN] = {
        "000", "001", "010", "011", "100", "101", "110", "111"
    };
    int i;
    for (i = 0; i < DESTTAB_LEN; i++) {
        if (strcmp(src, desttab[i]) == 0)
            return destBintab[i];
    }
    return "000";
}

// if a equal 1, then the c-converter should know that a bit set to one
char * convertComp(char *src, int len, int * a)
{
    char * comptab[COMPTAB_LEN] = {
        "0", "1", "-1", "D",
        "A", "!D", "!A", "-D",
        "-A", "D+1", "A+1", "D-1",
        "A-1", "D+A", "D-A", "A-D",
        "D&A", "D|A"
    };
    char * compBintab[COMPTAB_LEN] = {
        "101010", "111111", "111010", "001100",
        "110000", "001101", "110001", "001111",
        "110011", "011111", "110111", "001110",
        "110010", "000010", "010011", "000111",
        "000000", "010101"
    };
    // check a bit, if M existed, then set it to true
    *a = 0;
    int i;
    char *ptr = src;
    for(i = 0; i < len; i++) {
        if (*ptr == 'M') {
            *a = 1;
            *ptr = 'A';
        }
        ptr++;
    }
    debug("a: %d, src: %s\n", *a, src);

    // after check the a bit, no matter A/M used in src, turn A/M in src to A
    for (i = 0; i < COMPTAB_LEN; i++) {
        if (strcmp(src, comptab[i]) == 0)
            return compBintab[i];
    }

    // should not come here
    return "error";
}

char * convertJMP(char *src)
{
    char * jumptab[8] = {
        "null", "JGT", "JEQ", "JGE", "JLT", "JNE", "JLE", "JMP"
    };
    char * jumbBintab[8] = {
        "000", "001", "010", "011", "100", "101", "110", "111"
    };
    int i;
    for (i = 0; i < JUMPTAB_LEN; i++) {
        if (strcmp(src, jumptab[i]) == 0)
            return jumbBintab[i];
    }
    return "000";
}

// the start of c-instruction must be 111
// ex: 111accccccdddjjj
// a is decided inside the comp bits,
// if a==1, then ALU use M
// if a==0, then ALU use A-register
char * convertC(char *instruction)
{
    int has_tok_1 = 0,
        has_tok_3 = 0;
    int i = 0, j = 0;
    char c;
    char *token_1 = malloc(sizeof(char) * TOK_LEN),
         *token_2 = malloc(sizeof(char) * TOK_LEN),
         *token_3 = malloc(sizeof(char) * TOK_LEN);
    int check_a;
    //printf("instruction: %s\n", instruction);
    // remember the index of the equal symbol
    // if the index is zero, then we know that token_1 or token_3
    // should be skipped
    while(((c = instruction[i++]) && c != '=') && i < BITS_NUM)
        ;
    if (c == '=')
        has_tok_1 = i-1;
    i = 0;
    while(((c = instruction[i++]) && c != ';') && i < BITS_NUM)
        ;
    if (c == ';')
        has_tok_3 = i-1;
    //printf("= : %d\n", has_tok_1);
    //printf("; : %d\n", has_tok_3);

    i = 0;
    if (has_tok_1) {
        while(has_tok_1 > i) {
            token_1[i] = instruction[i];
            i++;
        }
        token_1[i] = '\0';
    } else {
        token_1[0] = '\0';
    }
    //printf("token_1 %s\n", token_1);

    // computer the start position of token_2
    i = has_tok_1;
    if (has_tok_1)
        i++;
    j = 0;
    //printf("i %d\n", i);
    if (has_tok_3) {
        while(i < has_tok_3 && i < BITS_NUM)
            token_2[j++] = instruction[i++];
        if (!has_tok_3)
            token_2[0] = '\0';
        else
            token_2[j] = '\0';
    } else {
        while(i < BITS_NUM)
            token_2[j++] = instruction[i++];
        token_2[j] = '\0';
    }

    // token part 3
    i++;
    j = 0;
    if (has_tok_3) {
        while(instruction[i] && i < BITS_NUM) {
            token_3[j++] = instruction[i++];
        }
        token_3[j] = '\0';
    } else {
        token_3[0] = '\0';
    }
//     printf("token_2 %s, len %lu\n", token_2, strlen(token_2));
//     printf("token_3 %s\n", token_3);

    // turn to binary
    char *dest = convertDest(token_1);
    char *comp = convertComp(token_2, strlen(token_2), &check_a);
    char *jump = convertJMP(token_3);
    //printf("a-bit: %d\n", check_a);
    //printf("comp: %s\n", comp);
    //printf("dest: %s\n", dest);
    //printf("jump: %s\n", jump);

    // concat all
    char *result = malloc(sizeof(char) * (BITS_NUM+1));
    strcpy(result, "111");
    if (check_a)
        strcat(result, "1");
    else
        strcat(result, "0");
    strcat(result, comp);
    strcat(result, dest);
    strcat(result, jump);
    //printf("result: %s\n", result);

    return result;
}

void test_convertA()
{
    //char digit[50];
    char out[50];
    printf("result of %d = %s\n", 23857, convertA(23857, out));
}

void test_convertC()
{
    char *test[] = {"D=D|M", "AMD=D;JMP", "M=1", "0;JMP", "D=D-A"};
    int i;
    for (i = 0; i < 5; i++)
        printf("result of %s = %s\n", test[i], convertC(test[i]));
}

int main(int argc, const char *argv[])
{
    initializer();

    if (argc != 2)
        printf("usage: ./a.out <filename>\n");
    else {
        FILE *fp = fopen(argv[1], "r");
        strcpy(buffer, argv[1]);
        char *writeptr = buffer;
        while(*writeptr++ != '.') ;
        strcpy(writeptr, "hack");
        FILE *fw = fopen(buffer, "w");

        char *lineptr = buffer;
        size_t linecap = BUFFERLEN;
        int linenum = 0;
        int i;
        // parse each linenum label, i.e, the text inside the braces
        // into the symboltab
        while((i = getline(&lineptr, &linecap, fp)) > 0) {      // Pass 1
            char *ptr = lineptr;
            char *label = malloc(sizeof(char) * BUFFERLEN);
            if (*ptr != '(') {
                if (*lineptr != '\n' && *(lineptr+1) != '\n' && *lineptr != '/') {
                    linenum++;
                }
                continue;
            }
            else {
                int i = 0;
                ptr++; // skip '('
                while(*ptr && *ptr != '\n' && *ptr != ' ')
                    label[i++] = *ptr++;
                if (*(ptr-2) == ')') {    // 2 = (null terminator) + ')'
                    label[i-2] = '\0';
                    addEntry(label, linenum);
                }
                else {
                    fprintf(stderr, ") is missed\n");
                    return EXIT_FAILURE;
                }
            }
        }

        fseek(fp, 0, SEEK_SET);

        while((i = getline(&lineptr, &linecap, fp)) > 0) {      // Pass 2
            char command[linecap];
            char *ptr = buffer;
            while(*ptr == ' ')  // skip whitespace in front of the any text
                ptr++;
            if (*ptr == '/')
                continue;
            int j = 0;
            for(; j < i && *ptr != ' ' && *ptr != '\n' && isprint(*ptr); j++, ptr++) {
                command[j] = *ptr;
            }
            command[j] = '\0';
            debug("command: %s\n", command);
            if (strlen(command) > 1) {      // above did not filter the newline, and if there is, just skip it
                if (command[0] == '@') {
                    char *label = &command[1];
                    if (!isdigit(*label)) {
                        if (!contains(label)) {
                            addEntry(label, registerptr++);
                        }
                        char out[50];
                        fputs(convertA((unsigned) getAddress(label), out), fw);
                        fputs("\n", fw);
                    } else {
                        char out[17];
                        debug("AAA %s %s\n", label, convertA((unsigned) atoi(label), out));
                        fputs(convertA((unsigned) atoi(label), out), fw);
                        fputs("\n", fw);
                    }
                } else if (command[0] != '(') {        // handle instruction c
                    debug("%s\n", convertC(command));
                    fputs(convertC(command), fw);
                    fputs("\n", fw);
                }
            }
        }
        fclose(fp);
        fclose(fw);
    }
#ifndef NDEBUG
    printTable();
#endif
    return 0;
}
