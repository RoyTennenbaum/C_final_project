#ifndef GLOBAL_H
#define GLOBAL_H

#include "dynamic-tables.h"
#include "static-tables.h"

enum
{
    FALSE,
    TRUE
};
enum
{
    SUCCESS,
    ERROR
};
#define LINE_SIZE 83

typedef struct
{
    symbolTable *symbolTable;
    macroTable *macroTable;
    operationTable *operationTable;
    directiveTable *directiveTable;
    registers *registers;
} assemblerContext;

/* Function prototypes */
int isKeyword(assemblerContext context);
int isNewSymbol(char *str, assemblerContext context);

#endif /* GLOBAL_H */