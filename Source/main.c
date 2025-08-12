#include <stdio.h>
#include "../Headers/main.h"

int main(int argc, char *argv[])
{
    symbolTable symbolTableHead;
    macroTable macroTableHead;

    static const operationTable opTable = {
        {"mov", MOV}, {"cmp", CMP}, {"add", ADD}, {"sub", SUB}, {"not", NOT}, {"clr", CLR}, {"lea", LEA}, {"inc", INC}, {"dec", DEC}, {"jmp", JMP}, {"bne", BNE}, {"red", RED}, {"prn", PRN}, {"jsr", JSR}, {"rts", RTS}, {"stop", STP}};
    static const directiveTable dirTable = {{".data", DATA},
                                            {".string", STRING},
                                            {".mat", MAT},
                                            {".entry", ENTRY},
                                            {".extern", EXTERN}};
    static const registers regs = {{"r0", 0}, {"r1", 1}, {"r2", 2}, {"r3", 3}, {"r4", 4}, {"r5", 5}, {"r6", 6}, {"r7", 7}};

    assemblerContext context;
    binaryWordList codeImage;
    static int ICF, DCF;
    int i;
    int complete;

    context.symbolTable = &symbolTableHead;
    context.macroTable = &macroTableHead;
    context.operationTable = &opTable;
    context.directiveTable = &dirTable;
    context.registers = &regs;

    for (i = 1; i < argc; i++)
    {
        symbolTableHead = NULL;
        macroTableHead = NULL;
        codeImage = NULL;
        complete = FALSE;

        if (preAssembler(argv[i], &context))
        {
            if (firstIteration(argv[i], &ICF, &DCF, &codeImage, &context))
            {
                if (secondIteration(argv[i], ICF, DCF, &codeImage, &context))
                    complete = TRUE;
            }
        }

        if (complete == FALSE)
        {
            /* print errors */
        }

        freeSymbolTable(symbolTableHead);
        freeMacroTable(macroTableHead);
        freeBinaryWordList(codeImage);
    }

    return 0;
}
