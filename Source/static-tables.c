#include "../Headers/static-tables.h"

int isInTable(const instruction *table, int tableSize, const char *str) {
    for (int i = 0; i < tableSize; i++) {
        if (strcmp(table[i].name, str) == 0) {
            return 1;
        }
    }
    return 0;
}