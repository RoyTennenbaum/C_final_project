#include <stdio.h>
#include <stdlib.h>
#include "../Headers/list-data-structure.h"
/**
 * Insert a node at the beginning of the list.
 * Allocates memory and sets the new node as start.
 */
void insert_first(LinkedList *pstart) {
    int item;
    node *newNode = (node *)malloc(sizeof(node));

    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
    } else {
        printf("Enter item: ");
        scanf("%d", &item);

        newNode->info = item;
        newNode->next = *pstart;
        *pstart = newNode;

        printf("Item inserted: %d\n", item);
    }
}

/**
 * Insert a node at the end of the list.
 * Traverses the list to the last node and appends the new node.
 */
void insert_last(LinkedList *pstart) {
    int item;
    node *ptr = NULL;
    node *newNode = NULL;
    int success = FALSE;

    printf("Enter item: ");
    scanf("%d", &item);

    newNode = (node *)malloc(sizeof(node));
    if (newNode == NULL) {
        printf("Memory allocation failed!\n");
    } else {
        newNode->info = item;
        newNode->next = NULL;

        if (*pstart == NULL) {
            *pstart = newNode;
            success = TRUE;
        } else {
            ptr = *pstart;

            while (ptr->next != NULL) {
                ptr = ptr->next;
            }

            ptr->next = newNode;
            success = TRUE;
        }
    }

    if (success) {
        printf("Item inserted: %d\n", item);
    }
}

/**
 * Insert a node after a specific value in the list.
 * Reads both the reference value and new item from user input.
 * @param pstart Pointer to the pointer of the start node.
 */
void insert_specific(LinkedList *pstart) {
    int after, item;
    node *ptr = *pstart;
    node *newNode = NULL;
    int found = FALSE;
    int success = FALSE;

    if (*pstart == NULL) {
        printf(
            "Linked list is empty. Cannot insert after a specific element.\n");
    } else {
        printf("Enter INFO after which new node is to be inserted: ");
        scanf("%d", &after);

        printf("Enter ITEM to insert: ");
        scanf("%d", &item);

        while (ptr != NULL && !found) {
            if (ptr->info == after) {
                found = TRUE;
                newNode = (node *)malloc(sizeof(node));

                if (newNode == NULL) {
                    printf("Memory allocation failed!\n");
                } else {
                    newNode->info = item;
                    newNode->next = ptr->next;
                    ptr->next = newNode;
                    success = TRUE;
                }
            }

            if (!found) {
                ptr = ptr->next;
            }
        }

        if (!found) {
            printf("Node with INFO %d not found in the list.\n", after);
        } else if (success) {
            printf("Item inserted: %d after %d\n", item, after);
        }
    }
}

/**
 * Delete the first node of the list.
 * Updates the start pointer and frees the old first node.
 * @param pstart Pointer to the pointer of the start node.
 */
void delete_first(LinkedList *pstart) {
    int item;
    node *ptr = NULL;

    if (*pstart == NULL) {
        printf("Linked list is empty. Nothing to delete.\n");
    } else {
        ptr = *pstart;
        item = ptr->info;
        *pstart = ptr->next;
        free(ptr);
        printf("Item deleted: %d\n", item);
    }
}

/**
 * Delete the last node of the list.
 * Traverses the list to find the last node and removes it.
 * @param pstart Pointer to the pointer of the start node.
 */
void delete_last(LinkedList *pstart) {
    int item;
    node *ptr = NULL;
    node *prev = NULL;
    int success = FALSE;

    if (*pstart == NULL) {
        printf("Linked list is empty. Nothing to delete.\n");
    } else if ((*pstart)->next == NULL) {
        item = (*pstart)->info;
        free(*pstart);
        *pstart = NULL;
        success = TRUE;
    } else {
        ptr = *pstart;
        while (ptr->next != NULL) {
            prev = ptr;
            ptr = ptr->next;
        }
        if (prev != NULL) {
            prev->next = NULL;
        }
        item = ptr->info;
        free(ptr);
        success = TRUE;
    }

    if (success) {
        printf("Item deleted: %d\n", item);
    }
}

/**
 * Delete a specific node by item value.
 * Searches the list and removes the node if found.
 * @param pstart Pointer to the pointer of the start node.
 */
void delete_specific(LinkedList *pstart) {
    int item;
    node *ptr = NULL;
    node *prev = NULL;
    int found = FALSE;

    printf("Enter ITEM which is to be deleted: ");
    scanf("%d", &item);

    if (*pstart == NULL) {
        printf("Linked list is empty. Nothing to delete.\n");
    } else if ((*pstart)->info == item) {
        ptr = *pstart;
        *pstart = ptr->next;
        free(ptr);
        found = TRUE;
    } else {
        ptr = *pstart;
        while (ptr != NULL && !found) {
            if (ptr->info == item) {
                found = TRUE;
                if (prev != NULL) {
                    prev->next = ptr->next;
                }
                free(ptr);
            } else {
                prev = ptr;
                ptr = ptr->next;
            }
        }
    }

    if (found) {
        printf("Item deleted: %d\n", item);
    } else {
        printf("Item %d not found in the list.\n", item);
    }
}

/**
 * Check if an item is present in the list.
 * Prints the location if found.
 * @param pstart Pointer to the pointer of the start node.
 */
void member(LinkedList start) {
    int item;
    LinkedList ptr = start;
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

/**
 * Reverse the linked list.
 * Reverses the links between nodes and updates the start pointer.
 * @param pstart Pointer to the pointer of the start node.
 */
void reverse(LinkedList *pstart) {
    node *ptr = *pstart;
    node *prev = NULL;
    node *next = NULL;

    while (ptr != NULL) {
        next = ptr->next;
        ptr->next = prev;
        prev = ptr;
        ptr = next;
    }

    *pstart = prev;

    printf("Reversed Linked List:\n");
    display(*pstart);
}

/**
 * Display the linked list nodes.
 * Prints address, info, and next pointer.
 */
void display(LinkedList start) {
    node *ptr = start;
    int i = 1;

    if (ptr == NULL) {
        printf("Linked list is empty.\n");
    } else {
        printf("Sr. No.\t\tAddress\t\tInfo\t\tNext\n");
        while (ptr != NULL) {
            printf("%d.\t\t%p\t%d\t\t%p\n", i, (void *)ptr, ptr->info,
                   (void *)ptr->next);
            ptr = ptr->next;
            i++;
        }
    }
}

/**
 * Return the length (number of nodes) in the list.
 */
int get_length(LinkedList start) {
    node *ptr = start;
    int count = 0;
    while (ptr != NULL) {
        count++;
        ptr = ptr->next;
    }

    return count;
}