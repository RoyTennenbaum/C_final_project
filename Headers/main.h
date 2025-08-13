#ifndef MAIN_H
#define MAIN_H

#include "global.h"
#include "errors.h"
#include "dynamic-tables.h"
#include "static-tables.h"

typedef struct {
    symbolTable *symbolTable;
    macroTable *macroTable;
    const operationTable *operationTable;
    const directiveTable *directiveTable;
    const registers *registers;
    errorList *errorList;
} assemblerContext;

int preAssembler(char *srcFileName, assemblerContext *context);
int firstIteration(char *fileName, int *pICF, int *pDCF, binaryWordList *codeImage, assemblerContext *context);
int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage, assemblerContext *context,
                    symbolTable *entriesTable);

#endif