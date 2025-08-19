#ifndef DYNAMIC_TABLES_H
#define DYNAMIC_TABLES_H

#include "global.h"
#include "errors.h"
#include "word-types.h"

/* Properties to differentiate between different symbol categories */
typedef enum { TYPE_EXTERNAL, TYPE_DATA, TYPE_CODE, TYPE_ENTRY } symbolType;

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

typedef struct binary_word_node {
    WordType binaryWord;
    wordKind kind;
    int C; /* IC or DC */
    int L; /* word count index in the same line as the counter's */
    struct binary_word_node *next;
} binaryWordNode;

/* error is a node of the linked list 'errors' */
typedef struct error_node {
    errorType errType;
    int lineNum;
    struct error_node *next;
} error;

/* Linked list definitions */
typedef symbol *symbolTable;
typedef macro *macroTable;
/* binaryWordList contains machine code encoded words */
typedef binaryWordNode *binaryWordList;
typedef error *errorList;

/* Function prototypes */
void insertSymbol(symbolTable *sHeadP, char *label, int address, symbolType type, int lineNum);
symbol *searchSymbol(symbolTable sHead, const char *label);
void displaySymbol(symbolTable sHead); /* for debugging */
void freeSymbolTable(symbolTable *sHead);

void insertMacro(macroTable *mHeadP, char *label, char *body);
macro *searchMacro(macroTable mHead, char *label);
void displayMacro(macroTable mHead); /* for debugging */
void freeMacroTable(macroTable *mHead);

void insertBinaryWord(binaryWordList *bHeadP, int C, int L, WordType word, int kind, int lineNum);
void freeBinaryWordList(binaryWordList *bHead);

void insertError(errorList *eHeadP, errorType type, int lineNum);
void displayErrors(errorList eHead);
void freeErrorList(errorList *eHead);

#endif