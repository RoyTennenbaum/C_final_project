#include <stdio.h>
#include <stdlib.h>
#include "../list-data-structure.h"
int main() {
    int ch;
    int running = TRUE;
    LinkedList start = NULL;

    while (running) {
        printf("\n\n%d. Insert First\n%d. Insert Last\n%d. Insert After "
               "Specific\n%d. Delete First\n%d. Delete Last\n%d. Delete "
               "Specific\n%d. Check Member\n%d. Reverse\n%d. Display\n%d. Get "
               "Length\n%d. Exit\n",
               INSERT_FIRST, INSERT_LAST, INSERT_SPECIFIC, DELETE_FIRST,
               DELETE_LAST, DELETE_SPECIFIC, MEMBER_CHECK, REVERSE, DISPLAY,
               GET_LENGTH, EXIT_PROGRAM);

        printf("Enter your choice: ");
        scanf("%d", &ch);

        switch (ch) {
        case INSERT_FIRST:
            insert_first(&start);
            break;

        case INSERT_LAST:
            insert_last(&start);
            break;

        case INSERT_SPECIFIC:
            insert_specific(&start);
            break;

        case DELETE_FIRST:
            delete_first(&start);
            break;

        case DELETE_LAST:
            delete_last(&start);
            break;

        case DELETE_SPECIFIC:
            delete_specific(&start);
            break;

        case MEMBER_CHECK:
            member(start);
            break;

        case REVERSE:
            reverse(&start);
            break;

        case DISPLAY:
            display(start);
            break;

        case GET_LENGTH:
            printf("\nThe length of the list is: %d\n", get_length(start));
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
