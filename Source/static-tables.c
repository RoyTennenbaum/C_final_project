#include <string.h>
#include "../Headers/static-tables.h"

operation *searchOperation(operationTable opTable, const char *str) {
    int i;
    char *opName;
    for (i = 0; i < OP_TABLE_SIZE; i++) {
        opName = opTable[i].name;
        if (strcmp(opName, str) == 0)
            return &opTable[i];
    }
    return NULL;
}

directive *searchDirective(directiveTable dirTable, const char *str) {
    int i;
    char *dirName;
    for (i = 0; i < DIR_TABLE_SIZE; i++) {
        dirName = dirTable[i].name;
        if (strcmp(dirName, str) == 0) {
            return &dirTable[i];
        }
    }
    return NULL;
}