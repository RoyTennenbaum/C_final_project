#include <string.h>
#include "../Headers/static-tables.h"

operation *searchOperation(const char *str) {
    int i;
    char *opName;
    for (i = 0; i < OP_TABLE_SIZE; i++) {
        opName = operationTable[i].name;
        if (strcmp(opName, str) == 0)
            return &operationTable[i];
    }
    return NULL;
}

directive *searchDirective(const char *str) {
    int i;
    char *dirName;
    for (i = 0; i < DIR_TABLE_SIZE; i++) {
        dirName = directiveTable[i];
        if (strcmp(dirName, str) == 0) {
            return &directiveTable[i];
        }
    }
    return NULL;
}