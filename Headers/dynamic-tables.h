#ifndef DYNAMIC_TABLES_H
#define DYNAMIC_TABLES_H

#include <stdio.h>
#include "../Headers/global.h"

/* Symbol Table Definitions: */
/**
 * symbol is a node to be contained in the linked list 'symbolTable'.
 */
typedef struct symbol_node
{
    char *label;
    int address;
    char *type;
    struct symbol_node *next;
} symbol;

/**
 * symbolTable points to the first node in the linked list of symbols.
 */
typedef symbol *symbolTable;

/* Macro Table Definitions: */
/**
 * macro is a node to be contained in the linked list 'macros'.
 */
typedef struct macro_node
{
    char *label;
    char *body;
    struct macro_node *next;
} macro;

/**
 * macroTable points to the first node in the linked list of macros.
 */
typedef macro *macroTable;

/* testing with main.c */
typedef enum
{
    INSERT = 1,
    SEARCH,
    DISPLAY,
    INSERT_MAC,
    SEARCH_MAC,
    DISPLAY_MAC,
    EXIT_PROGRAM
} MenuChoices;

/* Function prototypes */
void insertSymbol(symbolTable *sHeadP, char *label, int address, char *type);
symbol *searchSymbol(symbolTable sHead, char *label);
void displaySymbol(symbolTable sHead); /* for debugging */
void insertMacro(macroTable *mHeadP, char *label, char *body);
macro *searchMacro(macroTable mHead, char *label);
void displayMacro(macroTable mHead); /* for debugging */

#endif /* DYNAMIC_TABLES_H */