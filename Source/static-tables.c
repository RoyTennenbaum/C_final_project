#include "../Headers/static-tables.h"

int isOperation(const char *str) {
    int i;
    for (i = 0; i < OP_TABLE_SIZE; i++) {
        if (strcmp(operationTable[i].name, str) == 0)
            return 1;
    }
    return 0;
}

int isDirective(const char *str) {
    int i;
    for (i = 0; i < DIR_TABLE_SIZE; i++) {
        if (strcmp(directiveTable[i].name, str) == 0)
            return 1;
    }
    return 0;
}