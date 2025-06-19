#include <stdio.h>
#include <stdlib.h>
#include "../Headers/dynamic-tables.h"
int main() {
    int ch;
    int running = TRUE;
    symbolTable start = NULL;
    macroTable startM = NULL;

    while (running) {
        printf(
            "\n%d. Insert Symbol\n%d. Search Symbol\n%d. Display Symbols\n%d. "
            "Insert Macro\n%d. Search Macro\n%d. Display Macros\n%d Exit\n",
            INSERT, SEARCH, DISPLAY, INSERT_MAC, SEARCH_MAC, DISPLAY_MAC,
            EXIT_PROGRAM);

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

        case INSERT_MAC:
            insertMac(&startM);
            break;

        case SEARCH_MAC:
            searchMac(startM);
            break;

        case DISPLAY_MAC:
            displayMac(startM);
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
