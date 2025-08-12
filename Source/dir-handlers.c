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
        encodeStrDir(dir, DC, dirList, context);
        break;

    case MAT:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA);
        }
        encodeMatDir(dir, DC, dirList, context);
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
    WordType word;
    int L = 0;
    char *arg;
    while ((arg = strtok(NULL, ", \t\n")) != NULL) {
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
    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}

void encodeStrDir(const directive *dir, int *DC, binaryWordList *dirList,
                  assemblerContext *context) {
    WordType word;
    int L = 0, i;
    char *param = strtok(NULL, " \t\n");
    size_t len;
    printf("The string to encode is: %s\n", param);

    if (!param) {
        printf("ERROR: no parameter for '.string' directive\n");
        return;
    }

    len = strlen(param);

    if (strtok(NULL, ", \t\n") != NULL) {
        printf(
            "ERROR: only one parameter is allowed for '.string' directive\n");
        return;
    }

    /* The minimal format for .string param is "a". including '\0' in the end, len is at least 3 */
    if (len < 3) {
        printf("ERROR: parameter of '.string' directive is too short\n");
        return;
    }

    if (param[0] != '"' || param[len - 1] != '"') {
        printf("ERROR: no \" in the beginning or end of '.string' parameter\n");
        return;
    }

    /* Skip the starting quote, stop before ending quote */
    for (i = 1; i < (len - 1); i++) {
        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));

        word.dir.data_bits = (unsigned int)((unsigned char)param[i]);

        insertBinaryWord(dirList, *DC, L, word);
        L++;
    }

    /* Add '\0' to the code image to mark the end of the string */
    memset(&word, 0, sizeof(word));
    word.dir.data_bits = 0;
    insertBinaryWord(dirList, *DC, L, word);
    L++;

    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}

void encodeMatDir(const directive *dir, int *DC, binaryWordList *dirList,
                  assemblerContext *context) {
    WordType word;
    int L = 0, num1, num2, matDim, value;
    char *dimentions = strtok(NULL, " \t\n"), *valStr;

    if (!dimentions) {
        printf("ERROR: no parameters for '.mat' directive\n");
        return;
    }

    if (sscanf(dimentions, "[%d][%d]", &num1, &num2) != 2 || num1 <= 0 ||
        num2 <= 0) {
        printf("ERROR: Invalid '.mat' dimentions format\n");
        return;
    }
    matDim = num1 * num2;

    while ((valStr = strtok(NULL, ", \t\n")) != NULL) {
        int i = 0;

        /* Cannot have more values than the dimention of the matrix */
        if (L >= matDim) {
            printf("ERROR: too many values for matrix\n");
            return;
        }

        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));

        if (valStr[i] == '+' || valStr[i] == '-') /* allow +/- signs */
            i++;

        for (; valStr[i] != '\0'; i++) {
            if (!isdigit((unsigned char)valStr[i])) {
                printf("ERROR: one or more of the values is not a legal "
                       "integer\n");
                return;
            }
        }

        value = atoi(valStr);

        /* Check valid signed 10-bit range */
        if (value < SIGNED_10_BIT_MIN || value > SIGNED_10_BIT_MAX) {
            printf("ERROR: number out of range\n");
            return;
        }

        word.dir.data_bits = (unsigned int)value;

        insertBinaryWord(dirList, *DC, L, word);
        L++;
    }

    /* If too few values, fill the rest of the matrix with zeroes */
    while (L < matDim) {
        memset(&word, 0, sizeof(word));
        word.dir.data_bits = 0;
        insertBinaryWord(dirList, *DC, L, word);
        L++;
    }

    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}