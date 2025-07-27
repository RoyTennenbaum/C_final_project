#include <stdio.h>
#include "../Headers/main.h"

int main(int argc, char *argv[]) {
    symbolTable symbolTableHead = NULL;
    macroTable macroTableHead = NULL;
    operationTable opTable = {
        {"mov", 0},  {"cmp", 1},  {"add", 2},  {"sub", 3},
        {"not", 4},  {"clr", 5},  {"lea", 6},  {"inc", 7},
        {"dec", 8},  {"jmp", 9},  {"bne", 10}, {"red", 11},
        {"prn", 12}, {"jsr", 13}, {"rts", 14}, {"stp", 15}};
    directiveTable dirTable = {{".data", DATA},
                               {".string", STRING},
                               {".mat", MAT},
                               {".entry", ENTRY},
                               {".extern", EXTERN}};
    registers regs = {{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3},
                      {"r4", 4}, {"r5", 5}, {"r6", 6}, {"r7", 7}};

    assemblerContext context = {&symbolTableHead, &macroTableHead, &opTable,
                                &dirTable, &regs};
    int i;
    for (i = 1; i < argc; i++) {
        preAssembler(argv[i], &context);
        /*firstIteration(argv[i], &context);
        secondIteration(argv[i], &context);*/
    }

    return 0;
}
