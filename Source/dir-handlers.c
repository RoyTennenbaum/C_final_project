#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/dir-handlers.h"

void handleDirective(const directive *dir, int *DC, char *symbolName,
                     binaryWordList *dirList, assemblerContext *context) {
    switch ((*dir).type) {
    case DATA:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        encodeDataDir(dir, DC, dirList, context);
        break;

    case STRING:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        break;

    case MAT:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        break;

    case ENTRY:
        /* handled in second iteration */
        break;

    case EXTERN:
        insertSymbol((*context).symbolTable, symbolName, 0, TYPE_EXTERNAL);
        break;

    default:
        /* ERROR */
        break;
    }
}

void encodeDataDir(const directive *dir, int *DC, binaryWordList *dirList,
                   assemblerContext *context) {
    char *arg;
    WordType word;
    int L = 0;
    while ((arg = strtok(NULL, ", \t")) != NULL) {
        int i = 0, num;
        size_t len = strlen(arg);

        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));

        if (arg[i] == '+' || arg[i] == '-') /* allow +/- signs */
            i++;

        for (; i < len; i++) {
            if (!isdigit((unsigned char)arg[i])) {
                printf(
                    "ERROR: one or more of the params is not a legal integer");
                return;
            }
        }
        num = atoi(arg);

        /* Check valid signed 10-bit range */
        if (num < SIGNED_10_BIT_MIN || num > SIGNED_10_BIT_MAX) {
            printf("ERROR: number out of range\n");
            return;
        } else {
            word.dir.data_bits = (unsigned int)num;

            insertBinaryWord(dirList, *DC, L, word);
            L++;
        }
    }
    (*DC) += L;
}