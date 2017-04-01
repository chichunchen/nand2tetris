#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define DEFAULT_START 16
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

void initializer();
void addEntry(char *label, int address);
bool contains(char *label);

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

void addEntry(char *label, int address)
{
    strcpy(symboltab[symboltabptr].label, label);
    symboltab[symboltabptr].address = address;
//     printf("T: %d ", symboltab[symboltabptr].address);
//     printf(", %s\n", symboltab[symboltabptr].label);
    symboltabptr++;
    //registerptr++;
}

int getAddress(char *symbol)
{
    int i;
    for (i = 0; i < symboltabptr; i++) {
        if (strcmp(symboltab[i].label, symbol) == 0)
            return symboltab[i].address;
    }
    fprintf(stderr, "[getAddress]: cannot found address\n");
    return -1;
}


int main(int argc, const char *argv[])
{
    initializer();

    if (argc != 2)
        printf("usage: ./a.out <filename>\n");
    else {
        FILE *fp = fopen(argv[1], "r");
        char *lineptr = buffer;
        size_t linecap = BUFFERLEN;
        int linenum = 0;
        int i;
        // parse each linenum label, i.e, the text inside the braces,
        // into the symboltab
        while((i = getline(&lineptr, &linecap, fp)) > 0) {      // Pass 1
            char *ptr = lineptr;
            char *label = malloc(sizeof(char) * BUFFERLEN);
            if (*ptr != '(') {
                if (*lineptr != '\n' && *(lineptr+1) != '\n' && *lineptr != '/') {
//                     printf("%s ", lineptr);
//                     printf("linenum %d++\n", linenum);
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
                    //printf("%s, linenum: %d\n", label, linenum);
                }
                else {
                    fprintf(stderr, ") is missed\n");
                    return EXIT_FAILURE;
                }
            }
        }

        // parse symbolic a-instruction
//         while((i = getline(&lineptr, &linecap, fp)) > 0) {
//             char command[linecap];
//             char *ptr = buffer;
//             while(*ptr == ' ')  // skip whitespace in front of the any text
//                 ptr++;
//             if (*ptr == '/')
//                 continue;
//             int j = 0;
//             while(j < i) {
//                 command[j++] = *ptr++;
//                 if (*ptr == ' ' || *ptr == '\n')
//                     break;
//             }
//             command[j] = '\0';
//             if (strlen(command) > 1) {      // above did not filter the newline, and if there is, just skip it
//                 if (command[0] == '@') {
//                     if (!isdigit(command[1])) {
//                         char *label = &command[1];
//                         if (!contains(label)) {
//                             addEntry(label);
//                         }
//                     }
//                 }
//                 linenum++;
//                 printf("%s\n", command);
//             }
//         }
    }
    int j;
    for (j = 0; j < ENTRYNUM; j++) {
        if (isalnum(symboltab[j].label[0])) {
            printf("T: %d, %s\n", symboltab[j].address, symboltab[j].label);
        }
    }
    return 0;
}
