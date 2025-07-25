#ifndef MAIN_H
#define MAIN_H

#include "../Headers/global.h"
#include "../Headers/errors.h"
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"

typedef struct {
    symbolTable *symbolTable;
    macroTable *macroTable;
    const operationTable *operationTable;
    const directiveTable *directiveTable;
    const registers *registers;
} assemblerContext;

#endif