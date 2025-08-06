#ifndef DYNAMIC_TABLES_H
#define DYNAMIC_TABLES_H

#include "global.h"

/* Properties to differentiate between different symbol categories */
typedef enum { TYPE_EXTERNAL, TYPE_DATA, TYPE_CODE } symbolType;

/* symbol is a node of the linked list 'symbolTable' */
typedef struct symbol_node {
    char *label;
    int address;
    symbolType type;
    struct symbol_node *next;
} symbol;

/* macro is a node of the linked list 'macroTable' */
typedef struct macro_node {
    char *label;
    char *body;
    struct macro_node *next;
} macro;

typedef struct line_data_node {
    int IC;
    int L;
    char *binarySentence;
    struct line_data_node *next;
} lineData;

/* Linked list definitions */
typedef symbol *symbolTable;
typedef macro *macroTable;
/* codeImage contains the binary representation of a file after the first iteration */
typedef lineData *codeImage;

/* Function prototypes */
void insertSymbol(symbolTable *sHeadP, char *label, int address,
                  symbolType type);
symbol *searchSymbol(symbolTable sHead, const char *label);
void displaySymbol(symbolTable sHead); /* for debugging */
void insertMacro(macroTable *mHeadP, char *label, char *body);
macro *searchMacro(macroTable mHead, char *label);
void displayMacro(macroTable mHead); /* for debugging */
void insertLineData(codeImage *lHeadP, int IC, char *binaryWord);

#endif