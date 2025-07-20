#include <stdio.h>
#include <stdlib.h>
#include "../Headers/dynamic-tables.h"
#include "../Headers/global.h"
#include "../Headers/static-tables.h"
int main(int argc, char *argv[]) {
    symbolTable symbolTableHead = NULL;
    macroTable macroTableHead = NULL;
    operationTable opTable = {
        {"mov", 0},  {"cmp", 1},  {"add", 2},  {"sub", 3},
        {"not", 4},  {"clr", 5},  {"lea", 6},  {"inc", 7},
        {"dec", 8},  {"jmp", 9},  {"bne", 10}, {"red", 11},
        {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stp", 15}};
    directiveTable dirTable = {".data", ".string", ".mat", ".entry", ".extern"};
    registers regs = {{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3},
                      {"r4", 4}, {"r5", 5}, {"r6", 6}, {"r7", 7}};

    assemblerContext context = {&symbolTableHead, &macroTableHead, &opTable,
                                &dirTable, &regs};

    for (int i = 1; i < argc; i++) {
        /* delete tables */
        preAssembler(argv[i], context);
        firstIteration(argv[i], context);
        secondIteration(argv[i], context);
    }

    // /* For re-testing later */
    // int ch;
    //int running = TRUE;
    // while (running) {
    //     printf(
    //         "\n%d. Insert Symbol\n%d. Search Symbol\n%d. Display Symbols\n%d. "
    //         "Insert Macro\n%d. Search Macro\n%d. Display Macros\n%d. Exit\n",
    //         INSERT, SEARCH, DISPLAY, INSERT_MAC, SEARCH_MAC, DISPLAY_MAC,
    //         EXIT_PROGRAM);

    //     printf("Enter your choice: ");
    //     scanf("%d", &ch);

    //     switch (ch) {

    //     case INSERT:
    //         insert(&start, "test symbol", 100);
    //         break;

    //     case SEARCH:
    //         search(start, "test symbol");
    //         break;

    //     case DISPLAY:
    //         display(start);
    //         break;

    //     case INSERT_MAC:
    //         insertMac(&startM, "test label", "test body");
    //         break;

    //     case SEARCH_MAC:
    //         searchMac(startM, "test label");
    //         break;

    //     case DISPLAY_MAC:
    //         displayMac(startM);
    //         break;

    //     case EXIT_PROGRAM:
    //         running = FALSE;
    //         break;

    //     default:
    //         printf("\nInvalid choice. Please try again.\n");
    //         break;
    //     }
    // }
    return 0;
}
