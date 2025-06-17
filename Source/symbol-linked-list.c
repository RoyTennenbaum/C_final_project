#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/symbol-linked-list.h"

/**
 * Insert a node at the end of the list.
 * Traverses the list to the last node and appends the new node.
 */
void insert(symbolTable *pstart) {
    char label[ROW_SIZE];
    int address;
    symbol *ptr = NULL;
    symbol *newSymbol = NULL;
    int success = FALSE;

    /* Testing */
    printf("Enter label: ");
    scanf("%s", label);
    printf("Enter address: ");
    scanf("%d", &address);

    newSymbol = (symbol *)malloc(sizeof(symbol));
    if (newSymbol == NULL) {
        printf("Memory allocation failed!\n");
    } else {
        (*newSymbol).label = malloc(strlen(label) + 1);
        strcpy((*newSymbol).label, label);
        (*newSymbol).address = address;
        (*newSymbol).next = NULL;

        if (*pstart == NULL) {
            *pstart = newSymbol;
            success = TRUE;
        } else {
            ptr = *pstart;

            while ((*ptr).next != NULL) {
                ptr = (*ptr).next;
            }

            (*ptr).next = newSymbol;
            success = TRUE;
        }
    }

    if (success) {
        printf("Item inserted: label=%s, address=%d\n", newSymbol->label,
               newSymbol->address);
    }
}

/**
 * Check if an item is present in the list.
 * Prints the location if found.
 * @param pstart Pointer to the pointer of the start node.
 */
void search(symbolTable start) {
    int item;
    symbolTable ptr = start;
    int loc = 1;
    int found = FALSE;

    printf("Enter ITEM to be searched: ");
    scanf("%d", &item);

    while (ptr != NULL && !found) {
        if (ptr->info == item) {
            printf("Item %d is present at location %d\n", item, loc);
            found = TRUE;
        } else {
            ptr = ptr->next;
            loc++;
        }
    }

    if (!found) {
        printf("Item is not present in the list\n");
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