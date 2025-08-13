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
    int success = FALSE;

    /* Allocate memory for new node in the list */
    newSymbol = (symbol *)malloc(sizeof(symbol));
    /* Check if memory allocation was successful */
    if (newSymbol == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
    } else {
        /* Insert all node data in the allocated space */
        (*newSymbol).label = malloc(strlen(label) + 1);
        strcpy((*newSymbol).label, label);
        (*newSymbol).address = address;
        (*newSymbol).type = type;
        (*newSymbol).next = NULL;

        /* If the list was empty insert as first node */
        if (*sHeadP == NULL) {
            *sHeadP = newSymbol;
            success = TRUE;
        } else { /* Else, insert as last node of the list */
            ptr = *sHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newSymbol;
            success = TRUE;
        }
    }

    if (success) {
        printf("Item inserted: label=%s, address=%d\n", (*newSymbol).label, (*newSymbol).address);
    }
    displaySymbol(*sHeadP);
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

void displaySymbol(symbolTable sHead) {
    symbol *ptr = sHead;
    int i = 1;

    if (ptr == NULL) {
        printf("Linked list is empty.\n");
    } else {
        printf("Sr. No.\t\tAddress\t\tInfo\t\tNext\n");
        while (ptr != NULL) {
            printf("%d.\t\t%p\t%s\t\t%p\n", i, (void *)ptr, ptr->label, (void *)ptr->next);
            ptr = ptr->next;
            i++;
        }
    }
}

void freeSymbolTable(symbolTable sHead) {
    symbol *ptr = sHead;
    while (ptr) {
        symbol *next = (*ptr).next;
        free((*ptr).label);
        free(ptr);
        ptr = next;
    }
}

void insertMacro(macroTable *mHeadP, char *label, char *body) {
    macro *ptr = NULL;
    macro *newMacro = NULL;
    int success = FALSE;

    /* Allocate memory for new node in the list */
    newMacro = (macro *)malloc(sizeof(macro));
    /* Check if memory allocation was successful */
    if (newMacro == NULL) {
        printf("Memory allocation failed!\n");
    } else {
        /* Insert all node data in the allocated space */
        (*newMacro).label = malloc(strlen(label) + 1);
        strcpy((*newMacro).label, label);
        (*newMacro).body = malloc(strlen(body) + 1);
        strcpy((*newMacro).body, body);
        (*newMacro).next = NULL;

        /* If the list was empty insert as first node */
        if (*mHeadP == NULL) {
            *mHeadP = newMacro;
            success = TRUE;
        } else { /* Else, insert as last node of the list */
            ptr = *mHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newMacro;
            success = TRUE;
        }
    }

    if (success) {
        printf("Item inserted: label=%s, body=%s\n", newMacro->label, newMacro->body);
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

void displayMacro(macroTable mHead) {
    macro *ptr = mHead;
    int i = 1;

    if (ptr == NULL) {
        printf("Linked list is empty.\n");
    } else {
        printf("Sr. No.\t\tAddress\t\tInfo\t\tNext\n");
        while (ptr != NULL) {
            printf("%d.\t\t%p\t%s\t\t%p\n", i, (void *)ptr, ptr->label, (void *)ptr->next);
            ptr = ptr->next;
            i++;
        }
    }
}

void freeMacroTable(macroTable mHead) {
    macro *ptr = mHead;
    while (ptr) {
        macro *next = (*ptr).next;
        free((*ptr).label);
        free((*ptr).body);
        free(ptr);
        ptr = next;
    }
}

void insertBinaryWord(binaryWordList *bHeadP, int C, int L, WordType word, int lineNum) {
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
    if (!newNode) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        return;
    }

    /* Insert all node data in the allocated space */
    (*newNode).binaryWord = word;
    (*newNode).C = C;
    (*newNode).L = L;
    (*newNode).next = NULL;

    /* Append to end of list */
    if (*bHeadP == NULL) {
        *bHeadP = newNode;
    } else {
        (*prev).next = newNode;
    }

    /* for debugging */
    printf("Word inserted: word=%u, C=%d, L=%d\n", *(unsigned int *)&(*newNode).binaryWord, (*newNode).C, (*newNode).L);
}

void freeBinaryWordList(binaryWordList bHead) {
    binaryWordNode *ptr = bHead;
    while (ptr) {
        binaryWordNode *next = (*ptr).next;
        free(ptr);
        ptr = next;
    }
}

void insertError(errorList *eHeadP, errorType type, int lineNum) {
    error *ptr = NULL;
    error *newError = NULL;
    int success = FALSE;

    /* Allocate memory for new node in the list */
    newError = (error *)malloc(sizeof(error));
    /* Check if memory allocation was successful */
    if (newError == NULL) {
        printf("Error: failed to allocate memory\n");
    } else {
        /* Insert all node data in the allocated space */
        (*newError).errType = type;
        (*newError).lineNum = lineNum;
        (*newError).next = NULL;

        /* If the list was empty insert as first node */
        if (*eHeadP == NULL) {
            *eHeadP = newError;
            success = TRUE;
        } else { /* Else, insert as last node of the list */
            ptr = *eHeadP;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newError;
            success = TRUE;
        }
    }

    if (success) {
        printf("Error inserted: type=%d, lineNum=%d\n", newError->errType, newError->lineNum);
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

void freeErrorList(errorList eHead) {
    error *ptr = eHead;
    while (ptr) {
        error *next = (*ptr).next;
        free(ptr);
        ptr = next;
    }
}