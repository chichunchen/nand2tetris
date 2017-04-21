#include "helper.h"
#include "constant.h"

struct nlist {
    struct nlist *next;
    char *name;
    char *type;
    int kind;
    int index;
};

struct nlist *class_hashtab[HASHSIZE];
struct nlist *subroutine_hashtab[HASHSIZE];

/* Helper Functions */

unsigned hash(char *s)
{
    unsigned hashval;

    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

struct nlist *lookup(int choice, char *s)
{
    struct nlist *np;

    if (choice == CLASS) {
        np = class_hashtab[hash(s)];
    } else {
        np = subroutine_hashtab[hash(s)];
    }
    for ( ; np != NULL; np = np->next)
        if (strcmp(s, np->name) == 0)
            return np;
    return NULL;
}

/* Public Functions */

// Defines a new identifier of a given name, type, and kind and assigns it a running index.
// STATIC and FIELD identifiers have a class scope, while ARG and VAR identifiers have a
// subroutine scope.
struct nlist *Define(int choice, char *name, char* type, int kind, int index)
{
    struct nlist *np;
    unsigned hashval;

    if ((np = lookup(choice, name)) == NULL) {
        np = (struct nlist *) malloc(sizeof(*np));
        if (np == NULL || (np->name = strdup(name)) == NULL)
            return NULL;
        np->kind = kind;
        np->index = index;
        hashval = hash(name);
        if (choice == CLASS) {
            np->next = class_hashtab[hashval];
            class_hashtab[hashval] = np;
        } else {
            np->next = subroutine_hashtab[hashval];
            subroutine_hashtab[hashval] = np;
        }
    } else
        free((void *) np->type);
    if ((np->type = strdup(type)) == NULL)
        return NULL;
    return np;
}

// Returns the number of variables of the given kind already defined in the current scope.
int varCount(int choice, int kind)
{
    int count = 0;
    struct nlist **nptr;

    if (choice == CLASS)
        nptr = class_hashtab;
    else
        nptr = subroutine_hashtab;

    int i;
    for (i = 0; i < HASHSIZE; i++) {
        if (nptr[i] && nptr[i]->kind == kind)
            count++;
    }
    return count;
}

int kindOf(int choice, char *name)
{
    struct nlist *np = lookup(choice, name);
    if (np)
        return np->kind;
    else
        return NONE;
}

// Returns the type of the named identifier in the current scope
char *typeOf(int choice, char *name)
{
    struct nlist *np = lookup(choice, name);
    if (np)
        return np->type;
    else
        return NULL;
}

int indexOf(int choice, char *name)
{
    struct nlist *np = lookup(choice, name);
    if (np)
        return np->index;
    else
        return NONE;
}

// clean subroutine table after the function/method returned
void cleanTab()
{
    int i;
    for (i = 0; i < HASHSIZE; i++) {
        struct nlist *np = subroutine_hashtab[i];
        if (np) {
            if (np->type)
                free(np->type);
            np->kind = NONE;
            np->index = NONE;
            np = NULL;
        }
    }
}

void symbolTest()
{
    printf("------ class -------\n");
    int i;
    for (i = 0; i < HASHSIZE; i++) {
        if (class_hashtab[i]) {
            char *v1 = class_hashtab[i]->name;
            printf("name: %s, type: %s, kind: %d, index: %d\n", v1, typeOf(CLASS, v1), kindOf(CLASS, v1), indexOf(CLASS, v1));
        }
    }
    printf("------ subroutine -------\n");
    for (i = 0; i < HASHSIZE; i++) {
        if (subroutine_hashtab[i]) {
            char *v1 = subroutine_hashtab[i]->name;
             printf("name: %s, type: %s, kind: %d, index: %d\n", v1, typeOf(SUBROUTINE, v1), kindOf(SUBROUTINE, v1), indexOf(SUBROUTINE, v1));
        }
    }
}
