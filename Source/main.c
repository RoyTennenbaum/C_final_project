#include <stdio.h>
#include "../Headers/main.h"

int main(int argc, char *argv[]) {
    symbolTable symbolTableHead = NULL;
    macroTable macroTableHead = NULL;

    static const operationTable opTable = {{"mov", MOV}, {"cmp", CMP}, {"add", ADD}, {"sub", SUB},
                                           {"not", NOT}, {"clr", CLR}, {"lea", LEA}, {"inc", INC},
                                           {"dec", DEC}, {"jmp", JMP}, {"bne", BNE}, {"red", RED},
                                           {"prn", PRN}, {"jsr", JSR}, {"rts", RTS}, {"stop", STP}};
    static const directiveTable dirTable = {
        {".data", DATA}, {".string", STRING}, {".mat", MAT}, {".entry", ENTRY}, {".extern", EXTERN}};
    static const registers regs = {{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3},
                                   {"r4", 4}, {"r5", 5}, {"r6", 6}, {"r7", 7}};
    errorList errors = NULL;

    assemblerContext context;
    binaryWordList codeImage = NULL;
    symbolTable entriesTable = NULL;
    static int ICF, DCF;
    int i;
    int fileSuccess, mainSuccess = 0;

    context.symbolTable = &symbolTableHead;
    context.macroTable = &macroTableHead;
    context.operationTable = &opTable;
    context.directiveTable = &dirTable;
    context.registers = &regs;
    context.errorList = &errors;

    for (i = 1; i < argc; i++) {
        fileSuccess = FALSE;

        if (preAssembler(argv[i], &context)) {
            if (firstIteration(argv[i], &ICF, &DCF, &codeImage, &context)) {
                if (secondIteration(argv[i], ICF, DCF, &codeImage, &context, &entriesTable))
                    fileSuccess = TRUE;
            }
        }

        if (fileSuccess == FALSE) {
            displayErrors(errors);
            mainSuccess = 1;
        }

        freeSymbolTable(&symbolTableHead);
        freeMacroTable(&macroTableHead);
        freeBinaryWordList(&codeImage);
        freeErrorList(&errors);
    }
    freeSymbolTable(&entriesTable);

    /* Conventionally, main returns 0 when successful :) */
    return (mainSuccess || fatalError) ? 1 : 0;
}
