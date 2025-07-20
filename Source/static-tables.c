#include "../Headers/static-tables.h"

operation searchOperation(const char *str) {
    int i;
    for (i = 0; i < OP_TABLE_SIZE; i++) {
        if (strcmp(operationTable[i].name, str) == 0)
            return operationTable[i];
    }
    return NULL;
}

directive searchDirective(const char *str) {
    int i;
    for (i = 0; i < DIR_TABLE_SIZE; i++) {
        if (strcmp(directiveTable[i], str) == 0) {
            return directiveTable[i];
        }
    }
    return NULL;
}

int isOperation(const char *str) {
    return (searchOperation(str) != NULL) ? 1 : 0;
}

int isDirective(const char *str) {
    return (searchDirective(str) != NULL) ? 1 : 0;
}