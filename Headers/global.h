#include "dynamic-tables.h"
#include "static-tables.h"

enum {FALSE, TRUE};

enum {SUCCESS, ERROR};

typedef struct {
    symbolTable *symbolTable;
    macroTable *macroTable;
    operationTable *operationTable;
    directiveTable *directiveTable;
    registers *registers;
} assemblerContext;