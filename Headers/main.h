#ifndef MAIN_H
#define MAIN_H

#include "../Headers/dynamic-tables.h"
#include "../Headers/global.h"
#include "../Headers/static-tables.h"

typedef struct
{
    symbolTable *symbolTable;
    macroTable *macroTable;
    operationTable *operationTable;
    directiveTable *directiveTable;
    registers *registers;
} assemblerContext;

#endif /* PRE_ASSEMBLER_H */