#include <stdio.h>

#define LINE_SIZE 80
enum { FALSE, TRUE };

/* Symbol Table Definitions: */

/**
 * symbol is a node to be contained in the linked list 'symbolTable'.
 */
typedef struct symbol_node {
    char *label;
    int address;
    struct symbol_node *next;
} symbol;

/**
 * symbolTable points to the first node in the linked list of symbols.
 */
typedef symbol *symbolTable;

/* Macro Table Definitions: */

/**
 * macro is a node to be contained in the linked list 'macroTable'.
 */
typedef struct macro_node {
    char *label;
    char *body;
    struct macro_node *next;
} macro;

/**
 * macroTable points to the first node in the linked list of macros.
 */
typedef macro *macroTable;

/* testing with main.c */
typedef enum {
    INSERT = 1,
    SEARCH,
    DISPLAY,
    INSERT_MAC,
    SEARCH_MAC,
    DISPLAY_MAC,
    EXIT_PROGRAM
} MenuChoices;

/* Function prototypes */
void insert(symbolTable *pstart); /* insert symbol */
void search(symbolTable start); /* search for symbol */
void display(symbolTable start); /* for debugging */
void insertMac(macroTable *pstartM); /* insert macro */
void searchMac(macroTable startM); /* search for macro */
void displayMac(macroTable startM); /* for debugging */
