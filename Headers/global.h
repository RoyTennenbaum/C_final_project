#include "dynamic-tables.h"
#include "static-tables.h"

enum { FALSE, TRUE };
enum { SUCCESS, ERROR };
#define LINE_SIZE 83

typedef struct {
    symbolTable *symbolTable;
    macroTable *macroTable;
    operationTable *operationTable;
    directiveTable *directiveTable;
    registers *registers;
} assemblerContext;