#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dynamic-tables.h"

/**
 * Insert a node at the end of the list.
 * Traverses the list to the last node and appends the new node.
 */
void insert(symbolTable *pstart) {
    char label[LINE_SIZE + 1];
    int address;
    symbol *ptr = NULL;
    symbol *newSymbol = NULL;
    int success = FALSE;

    /* Testing */
    printf("Enter label: ");
    scanf("%s", label);
    printf("Enter address: ");
    scanf("%d", &address);

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
        (*newSymbol).next = NULL;

        /* If the list was empty insert as first node */
        if (*pstart == NULL) {
            *pstart = newSymbol;
            success = TRUE;
        } else { /* Else, insert as last node of the list */
            ptr = *pstart;

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
 * Check if an item is present in the list.
 * Prints the location if found.
 * @param pstart Pointer to the pointer of the start node.
 */
void search(symbolTable start) {
    char label[LINE_SIZE + 1];
    symbolTable ptr = start;
    int count = 1;
    int found = FALSE;

    /* testing */
    printf("Enter symbol label to be searched: ");
    scanf("%s", label);

    while (ptr != NULL && !found) {
        if (strcmp((*ptr).label, label) != 0) {
            ptr = (*ptr).next;
            count++;
        } else {
            printf("label %s is present in symbol number %d\n", label, count);
            found = TRUE;
        }
    }

    if (!found) {
        printf("symbol is not present in the list.\n");
    }
}

void display(symbolTable start) {
    symbol *ptr = start;
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

// void insertMac(macroTable *pstartM) {
void insertMac(macroTable *pstartM, char *label, char *body) {
    // char label[LINE_SIZE + 1]; /* upto 30 chars */
    // char body[1000]; /* temp only for testing, should be an arg in the func */
    macro *ptr = NULL;
    macro *newMacro = NULL;
    int success = FALSE;

    // /* testing */
    // printf("Enter macro label: ");
    // scanf("%s", label);
    // printf("Enter macro body: ");
    // scanf("%s", body);

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
        if (*pstartM == NULL) {
            *pstartM = newMacro;
            success = TRUE;
        } else { /* Else, insert as last node of the list */
            ptr = *pstartM;

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

macro *searchMac(macroTable startM, char *label) {
    // char label[LINE_SIZE + 1];
    macroTable ptr = startM;
    int count = 1;
    int found = FALSE;

    // /* testing */
    // printf("Enter macro label to be searched: ");
    // scanf("%s", label);

    while (ptr != NULL && !found) {
        if (strcmp((*ptr).label, label) != 0) {
            ptr = (*ptr).next;
            count++;
        } else {
            printf("label %s is present in macro number %d\n", label, count);
            found = TRUE;
            return ptr;
        }
    }

    if (!found) {
        printf("macro is not present in the list.\n");
    }
    return NULL;
}

void displayMac(macroTable startM) {
    macro *ptr = startM;
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