#include <stdio.h>

#define ROW_SIZE 80
enum { FALSE, TRUE };

typedef struct symbol {
    char *label;
    int address;
    struct symbol *next;
} symbol;

/**
 * LinkedList is a pointer to the first node in the list.
 */
typedef symbol *symbolTable;

/* testing with main.c */
typedef enum { INSERT = 1, SEARCH, DISPLAY, EXIT_PROGRAM } MenuChoices;

/* Function prototypes */
void insert(symbolTable *pstart); /* insert symbol */
void search(symbolTable start); /* search for symbol */
void display(symbolTable start); /* for debugging */
