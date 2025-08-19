#ifndef MAIN_H
#define MAIN_H

#include "global.h"
#include "errors.h"
#include "dynamic-tables.h"
#include "static-tables.h"

/* enum to conclude iterations over files (pre, first and second) */
enum { ERROR_FOUND = 0, NO_ERROR_FOUND };

#define SYMBOL_MAX_LENGTH 30
#define LINE_SIZE 82

#define SIGNED_8_BIT_MIN -128
#define SIGNED_8_BIT_MAX 127
#define SIGNED_10_BIT_MIN -512
#define SIGNED_10_BIT_MAX 511

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