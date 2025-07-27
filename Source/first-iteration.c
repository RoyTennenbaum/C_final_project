#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, assemblerContext context) {
    int IC = 0, DC = 0;

    char line[LINE_SIZE], *arg;
    int lineNum = 0; /* Mark line number for future error messages */
    FILE *fp = fopen(fileName, "r");

    int symbolFlag = 0;
    directive tempDirective;
    operation tempOperation;

    if (fp == NULL) {
        printf("Error: file could not be opened.");
        return 1;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;
        /* Get the first word in the line */
        arg = strtok(line, " \t\n");

        /* Skip comment lines and empty lines */
        if (line[0] == ';' || arg == NULL) {
            continue;
        }

        if (isNewSymbol(arg)) {
            symbolFlag = 1;
            arg = strtok(NULL, " \t\n");
            /* Check if symbol is not placed in front of anything */
            if (arg == NULL) {
                printf("ERROR: symbol is the only word in the line");
                continue;
            }
        }

        if (handleDirective(context, arg, &DC, symbolFlag)) {
            /* The current line is a directive sentence, it was handled, 
            so we move to the next line */
            continue;
        }

        if (symbolFlag) {
            insertSymbol(context.symbolTable, arg, IC, ".code");
        }
        if (tempOperation = searchOperation(arg) == NULL) {
            printf("ERROR in op name (12 in algo)");
        }
        /* 13: Calculate op word count in variable L */
        /* 14: Code words into word-types */
    }

    fclose(fp);
    return 0;
}

int isKeyword(assemblerContext context, char *str) {
    if (searchSymbol(context.symbolTable, str) ||
        searchMacro(context.macroTable, str) || searchOperation(str) ||
        searchDirective(str) || searchRegister(str)) {
        return TRUE;
    }
    return FALSE;
}

int isNewSymbol(assemblerContext context, char *str) {
    size_t len = strlen(str);
    /* failsafe to ensure len-1 will always be valid */
    if (len < 1) {
        return FALSE;
    }
    if (len <= 30 && isalpha(str[0]) && str[len - 1] == ':' &&
        !isKeyword(context, str)) {
        return TRUE;
    }
    return FALSE;
}

int handleDirective(assemblerContext *context, char *str, int *DC,
                    int symbolFlag) {
    directive *dir = searchDirective(str);

    if (dir == NULL) {
        return FALSE;
    }

    switch (dir.directiveType) {
    case DATA:
        if (symbolFlag) {
            insertSymbol(context.symbolTable, str, *DC, ".data");
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        return TRUE;

    case STRING:
        if (symbolFlag) {
            insertSymbol(context.symbolTable, str, *DC, ".data");
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        return TRUE;

    case MAT:
        if (symbolFlag) {
            insertSymbol(context.symbolTable, str, *DC, ".data");
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        return TRUE;

    case ENTRY:
        /* handled in second iteration */
        return TRUE;

    case EXTERN:
        insertSymbol(context.symbolTable, str, 0, ".external");
        /* handled in second iteration */
        return TRUE;

    default:
        return FALSE;
    }
}