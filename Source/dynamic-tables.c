#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dynamic-tables.h"

/**
 * Insert a node at the end of the list.
 * Traverses the list to the last node and appends the new node.
 */
void insertSymbol(symbolTable *sHeadP, char *label, int address, symbolType type, int lineNum) {
    symbol *ptr = NULL;
    symbol *newSymbol = NULL;

    /* Allocate memory for new node in the list */
    newSymbol = (symbol *)malloc(sizeof(symbol));
    /* Check if memory allocation was successful */
    if (newSymbol == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
    } else {
        /* Insert all node data in the allocated space */
        (*newSymbol).label = malloc(strlen(label) + 1);
        if ((*newSymbol).label == NULL) {
            setFatalError(lineNum, ERR_MEM_ALLOC);
        } else
            strcpy((*newSymbol).label, label);
        (*newSymbol).address = address;
        (*newSymbol).type = type;
        (*newSymbol).next = NULL;

        /* If the list was empty insert as first node */
        if (*sHeadP == NULL) {
            *sHeadP = newSymbol;
        } else { /* Else, insert as last node of the list */
            ptr = *sHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newSymbol;
        }
    }
}

/**
 * Check if a symbol is present in the list.
 * Returns the symbol if found, else NULL.
 * @param sHead Points to the pointer of the first symbolTable node.
 */
symbol *searchSymbol(symbolTable sHead, const char *label) {
    symbolTable ptr = sHead;

    while (ptr != NULL) {
        if (strcmp((*ptr).label, label) == 0) {
            return ptr;
        }
        ptr = (*ptr).next;
    }
    return NULL;
}

void freeSymbolTable(symbolTable *sHead) {
    symbol *ptr = *sHead;
    while (ptr) {
        symbol *next = (*ptr).next;
        free((*ptr).label);
        free(ptr);
        ptr = next;
    }
    *sHead = NULL;
}

void insertMacro(macroTable *mHeadP, char *label, char *body, int lineNum) {
    macro *ptr = NULL;
    macro *newMacro = NULL;

    /* Allocate memory for new node in the list */
    newMacro = (macro *)malloc(sizeof(macro));
    /* Check if memory allocation was successful */
    if (newMacro == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
    } else {
        /* Insert all node data in the allocated space */
        (*newMacro).label = malloc(strlen(label) + 1);
        if ((*newMacro).label == NULL)
            setFatalError(lineNum, ERR_MEM_ALLOC);
        else
            strcpy((*newMacro).label, label);

        (*newMacro).body = malloc(strlen(body) + 1);
        if ((*newMacro).body == NULL)
            setFatalError(lineNum, ERR_MEM_ALLOC);
        else
            strcpy((*newMacro).body, body);

        (*newMacro).next = NULL;

        /* If the list was empty insert as first node */
        if (*mHeadP == NULL) {
            *mHeadP = newMacro;
        } else { /* Else, insert as last node of the list */
            ptr = *mHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newMacro;
        }
    }
}

/**
 * Check if a macro is present in the list.
 * Returns the macro if found, else NULL.
 * @param mHead Points to the first macroTable node.
 */
macro *searchMacro(macroTable mHead, char *label) {
    macroTable ptr = mHead;

    while (ptr != NULL) {
        if (strcmp((*ptr).label, label) == 0) {
            return ptr;
        }
        ptr = (*ptr).next;
    }
    return NULL;
}

void freeMacroTable(macroTable *mHead) {
    macro *ptr = *mHead;
    while (ptr) {
        macro *next = (*ptr).next;
        free((*ptr).label);
        free((*ptr).body);
        free(ptr);
        ptr = next;
    }
    *mHead = NULL;
}

void insertBinaryWord(binaryWordList *bHeadP, int C, int L, WordType word, int kind, int lineNum) {
    binaryWordNode *ptr = *bHeadP;
    binaryWordNode *prev = NULL;
    binaryWordNode *newNode;

    /* Traverse the list to the latest node */
    while (ptr != NULL) {
        prev = ptr;
        ptr = (*ptr).next;
    }

    /* Allocate memory for new node in the list */
    newNode = malloc(sizeof(*newNode));
    /* Check if memory allocation was successful */
    if (newNode == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        return;
    }

    /* Insert all node data in the allocated space */
    (*newNode).binaryWord = word;
    (*newNode).kind = kind;
    (*newNode).C = C;
    (*newNode).L = L;
    (*newNode).next = NULL;

    /* Append to end of list */
    if (*bHeadP == NULL) {
        *bHeadP = newNode;
    } else {
        (*prev).next = newNode;
    }
}

void freeBinaryWordList(binaryWordList *bHead) {
    binaryWordNode *ptr = *bHead;
    while (ptr) {
        binaryWordNode *next = (*ptr).next;
        free(ptr);
        ptr = next;
    }
    *bHead = NULL;
}

void insertError(errorList *eHeadP, errorType type, int lineNum) {
    error *ptr = NULL;
    error *newError = NULL;

    /* Allocate memory for new node in the list */
    newError = (error *)malloc(sizeof(error));
    /* Check if memory allocation was successful */
    if (newError == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        return;
    } else {
        /* Insert all node data in the allocated space */
        (*newError).errType = type;
        (*newError).lineNum = lineNum;
        (*newError).next = NULL;

        /* If the list was empty insert as first node */
        if (*eHeadP == NULL) {
            *eHeadP = newError;
        } else { /* Else, insert as last node of the list */
            ptr = *eHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newError;
        }
    }
}

void displayErrors(errorList eHead) {
    error *ptr = eHead;
    int count = 0;

    printf("\n==================================================\n");
    printf(" Assembly failed due to the following errors:\n");
    printf("==================================================\n\n");

    while (ptr != NULL) {
        count++;
        printf("[%2d] Line %-4d | Error: %s\n", count, (*ptr).lineNum, getErrMessage((*ptr).errType));
        ptr = (*ptr).next;
    }

    printf("\n==================================================\n");
    printf(" Total error count: %d\n", count);
    printf("==================================================\n\n");
}

void freeErrorList(errorList *eHead) {
    error *ptr = *eHead;
    while (ptr) {
        error *next = (*ptr).next;
        free(ptr);
        ptr = next;
    }
    *eHead = NULL;
}