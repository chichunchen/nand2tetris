#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define DEFAULT_START 10
#define BUFFERLEN 100
#define ENTRYNUM  1000
char buffer[BUFFERLEN];

struct symbol {      // table entry
    char label[100];
    int address;
};

int registerptr;
int symboltabptr;
struct symbol symboltab[ENTRYNUM];

bool contains(char *label)
{
    int i;
    for (i = 0; i < ENTRYNUM; i++) {
        //printf("compare %s\n", symboltab[j].label);
        if (strcmp(label, symboltab[i].label) == 0) {
            printf("Has set up.\n");
            return true;
        }
    }
    return false;
}

void addEntry(char *label)
{
    strcpy(symboltab[symboltabptr].label, label);
    symboltab[symboltabptr].address = registerptr;
    printf("T: %d ", symboltab[symboltabptr].address);
    printf(", %s\n", symboltab[symboltabptr].label);
    symboltabptr++;
    registerptr++;
}


int main(int argc, const char *argv[])
{
    symboltabptr = 0;       // ptr for symbol table
    registerptr = DEFAULT_START;

    if (argc != 2)
        printf("usage: ./a.out <filename>\n");
    else {
        FILE *fp = fopen(argv[1], "r");
        char *lineptr = buffer;
        size_t linecap = BUFFERLEN;
        int linenum = 0;
        int i;
        while((i = getline(&lineptr, &linecap, fp)) > 0) {
            char command[linecap];
            char *ptr = buffer;
            while(*ptr == ' ')  // skip whitespace in front of the any text
                ptr++;
            if (*ptr == '/')
                continue;
            int j = 0;
            while(j < i) {
                command[j++] = *ptr++;
                if (*ptr == ' ' || *ptr == '\n')
                    break;
            }
            command[j] = '\0';
            if (strlen(command) > 1) {      // above did not filter the newline, and if there is, just skip it
                if (command[0] == '@') {
                    if (!isdigit(command[1])) {
                        char *label = &command[1];
                        if (!contains(label)) {
                            addEntry(label);
                        }
                    }
                }
                linenum++;
                printf("%s\n", command);
            }
        }
    }
    int j;
    for (j = 0; j < ENTRYNUM; j++) {
        if (isalnum(symboltab[j].label[0])) {
            printf("T: %d ", symboltab[j].address);
            printf(", %s\n", symboltab[j].label);
        }
    }
    return 0;
}
