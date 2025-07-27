#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dynamic-tables.h"

/**
 * Insert a node at the end of the list.
 * Traverses the list to the last node and appends the new node.
 */
void insertSymbol(symbolTable *sHeadP, char *label, int address, char *type) {
    symbol *ptr = NULL;
    symbol *newSymbol = NULL;
    int success = FALSE;

    /* Allocate memory for new node in the list */
    newSymbol = (symbol *)malloc(sizeof(symbol));
    /* Check if memory allocation was successful */
    if (newSymbol == NULL) {
        printf("Memory allocation failed!\n");
    } else {
        /* Insert all node data in the allocated space */
        (*newSymbol).label = malloc(strlen(label) + 1);
        strcpy((*newSymbol).label, label);
        (*newSymbol).address = address;
        (*newSymbol).type = malloc(strlen(type) + 1);
        strcpy((*newSymbol).type, type);
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
        printf("Item inserted: label=%s, address=%d\n", (*newSymbol).label,
               (*newSymbol).address);
    }
}

/**
 * Check if a symbol is present in the list.
 * Returns the symbol if found, else NULL.
 * @param sHead Points to the pointer of the first symbolTable node.
 */
symbol *searchSymbol(symbolTable sHead, char *label) {
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
            printf("%d.\t\t%p\t%s\t\t%p\n", i, (void *)ptr, ptr->label,
                   (void *)ptr->next);
            ptr = ptr->next;
            i++;
        }
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
        printf("Item inserted: label=%s, body=%s\n", newMacro->label,
               newMacro->body);
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
            printf("%d.\t\t%p\t%s\t\t%p\n", i, (void *)ptr, ptr->label,
                   (void *)ptr->next);
            ptr = ptr->next;
            i++;
        }
    }
}