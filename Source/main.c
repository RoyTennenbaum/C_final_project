#include <stdio.h>
#include <stdlib.h>
#include "../Headers/symbol-linked-list.h"
int main() {
    int ch;
    int running = TRUE;
    symbolTable start = NULL;

    while (running) {
        printf("\n%d. Insert\n%d. Search\n%d. Display\n%d. Exit\n", INSERT,
               SEARCH, DISPLAY, EXIT_PROGRAM);

        printf("Enter your choice: ");
        scanf("%d", &ch);

        switch (ch) {

        case INSERT:
            insert(&start);
            break;

        case SEARCH:
            search(start);
            break;

        case DISPLAY:
            display(start);
            break;

        case EXIT_PROGRAM:
            running = FALSE;
            break;

        default:
            printf("\nInvalid choice. Please try again.\n");
            break;
        }
    }
    return 0;
}
