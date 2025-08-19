#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../Headers/dir-handlers.h"

void handleDirective(const directive *dir, int *DC, char *symbolName, binaryWordList *dirList, int lineNum,
                     int *errorFlag, assemblerContext *context) {
    switch ((*dir).type) {
    case DATA:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA, lineNum);
        }
        encodeDataDir(dir, DC, dirList, lineNum, errorFlag, context);
        break;

    case STRING:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA, lineNum);
        }
        encodeStrDir(dir, DC, dirList, lineNum, errorFlag, context);
        break;

    case MAT:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, *DC, TYPE_DATA, lineNum);
        }
        encodeMatDir(dir, DC, dirList, lineNum, errorFlag, context);
        break;

    case ENTRY:
        /* Handled in second iteration */
        break;

    case EXTERN:
        if (symbolName != NULL) {
            insertSymbol((*context).symbolTable, symbolName, 0, TYPE_EXTERNAL, lineNum);
        }
        /* Other than adding symbol to table, it is handled in second iteration */
        break;

    default:
        setFatalError(lineNum, ERR_INTERNAL);
        break;
    }
}

void encodeDataDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                   assemblerContext *context) {
    WordType word;
    int values[DATA_PARAMS_BUF];
    int count = 0, i = 0;
    int L = 0;

    /* Parse helpers */
    char *restOfLine, *arg, *pEnd;
    long longNum;
    int num;

    /* Get pointer to rest of the line (for pre-checks for comma errors) */
    restOfLine = strtok(NULL, "\n");
    if (!restOfLine) {
        *errorFlag = TRUE;
        insertError(context->errorList, ERR_DIR_NO_PARAM, lineNum);
        return;
    }

    /* Validate numbers and comma locations */
    if (!areDataArgsValid(restOfLine)) {
        *errorFlag = TRUE;
        insertError(context->errorList, ERR_DATA_INVALID_FORMAT, lineNum);
        return;
    }

    /* Reset strtok to parse normally on commas and spaces */
    arg = strtok(restOfLine, ", \t\n");

    while (arg != NULL) {
        printf("arg is: %s\n", arg);
        errno = 0;
        /* Store strtol result */
        longNum = strtol(arg, &pEnd, DECIMAL);

        /* Check for any invalid characters */
        if (*pEnd != '\0') {
            *errorFlag = TRUE;
            insertError(context->errorList, ERR_INVALID_INTEGER, lineNum);
            return;
        }

        if (errno == ERANGE || longNum < SIGNED_10_BIT_MIN || longNum > SIGNED_10_BIT_MAX) {
            *errorFlag = TRUE;
            insertError(context->errorList, ERR_NUMBER_OUT_OF_RANGE, lineNum);
            return;
        }

        /* Cast from long to int and store num */
        num = (int)longNum;
        values[count++] = num;

        arg = strtok(NULL, ", \t\n");
    }

    for (; i < count; i++) {
        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));
        word.dir.data_bits = (unsigned int)values[i];

        insertBinaryWord(dirList, *DC, i, word, DIR, lineNum);
        if (fatalError)
            return;

        L++;
    }

    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}

void encodeStrDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                  assemblerContext *context) {
    WordType word;
    int L = 0, i;
    char *param = strtok(NULL, " \t\n");
    size_t len;
    printf("The string to encode is: %s\n", param);

    if (!param) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_DIR_NO_PARAM, lineNum);
        return;
    }

    len = strlen(param);

    if (strtok(NULL, ", \t\n") != NULL) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_STR_TOO_MANY_PARAMS, lineNum);
        return;
    }

    /* The minimal format for .string param is "a". including '\0' in the end, len is at least 3 */
    if (len < 3) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_STRING_TOO_SHORT, lineNum);
        return;
    }

    if (param[0] != '"' || param[len - 1] != '"') {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_STRING_MISSING_QUOTES, lineNum);
        return;
    }

    /* Skip the starting quote, stop before ending quote */
    for (i = 1; i < (len - 1); i++) {
        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));

        word.dir.data_bits = (unsigned int)((unsigned char)param[i]);

        insertBinaryWord(dirList, *DC, L, word, DIR, lineNum);
        if (fatalError)
            return;
        else
            L++;
    }

    /* Add '\0' to the code image to mark the end of the string */
    memset(&word, 0, sizeof(word));
    word.dir.data_bits = 0;
    insertBinaryWord(dirList, *DC, L, word, DIR, lineNum);
    if (fatalError)
        return;
    else
        L++;

    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}

void encodeMatDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                  assemblerContext *context) {
    WordType word;
    int L = 0, num1, num2, matDim, value;
    char *dimentions = strtok(NULL, " \t\n"), *valStr;

    if (!dimentions) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_DIR_NO_PARAM, lineNum);
        return;
    }

    if (sscanf(dimentions, "[%d][%d]", &num1, &num2) != 2 || num1 <= 0 || num2 <= 0) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_MAT_INVALID_DIM, lineNum);
        return;
    }

    matDim = num1 * num2;

    while ((valStr = strtok(NULL, ", \t\n")) != NULL) {
        int i = 0;

        /* Cannot have more values than the dimention of the matrix */
        if (L >= matDim) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_MAT_TOO_MANY_VALUES, lineNum);
            return;
        }

        /* Initialize the 'WordType' union with zeros */
        memset(&word, 0, sizeof(word));

        if (valStr[i] == '+' || valStr[i] == '-') /* allow +/- signs */
            i++;

        for (; valStr[i] != '\0'; i++) {
            if (!isdigit((unsigned char)valStr[i])) {
                *errorFlag = TRUE;
                insertError((*context).errorList, ERR_INVALID_INTEGER, lineNum);
                return;
            }
        }

        value = atoi(valStr);

        /* Check valid signed 10-bit range */
        if (value < SIGNED_10_BIT_MIN || value > SIGNED_10_BIT_MAX) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_NUMBER_OUT_OF_RANGE, lineNum);
            return;
        }

        word.dir.data_bits = (unsigned int)value;

        insertBinaryWord(dirList, *DC, L, word, DIR, lineNum);
        if (fatalError)
            return;
        else
            L++;
    }

    /* If too few values, fill the rest of the matrix with zeroes */
    while (L < matDim) {
        memset(&word, 0, sizeof(word));
        word.dir.data_bits = 0;
        insertBinaryWord(dirList, *DC, L, word, DIR, lineNum);
        if (fatalError)
            return;
        else
            L++;
    }

    /* increase DC by the number of data words the instruction occupies */
    (*DC) += L;
}

int areDataArgsValid(const char *line) {
    const char *c = line;

    printf("Validating line: '%s' (length: %d)\n", line, (int)strlen(line));

    /* Skip leading spaces */
    while (isspace((unsigned char)*c))
        c++;

    /* Must start with a digit or sign */
    if (!isdigit((unsigned char)*c) && *c != '+' && *c != '-') {
        printf("ERR 1");
        return FALSE;
    }

    while (*c) {
        /* Skip sign if there is */
        if (*c == '+' || *c == '-')
            c++;

        /* Must have digits */
        if (!isdigit((unsigned char)*c)) {
            printf("ERR 2");
            return FALSE;
        }

        while (isdigit((unsigned char)*c))
            c++;

        /* Skip spaces after number */
        while (isspace((unsigned char)*c))
            c++;

        /* Finished running successfully */
        if (*c == '\0')
            return TRUE;

        if (*c != ',') {
            printf("ERR 3");
            return FALSE;
        }

        /* Skip first comma between numbers */
        c++;

        /* Skip spaces after comma */
        while (isspace((unsigned char)*c))
            c++;

        /* After a comma must come a sign or digit */
        if (!isdigit((unsigned char)*c) && *c != '+' && *c != '-') {
            printf("ERR 4");
            return FALSE;
        }
    }

    return TRUE;
}