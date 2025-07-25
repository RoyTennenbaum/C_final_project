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
} assemblerContext;

#endif