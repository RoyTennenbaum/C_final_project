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

        arg = strtok(line, " ");

        /* Skip comment lines and empty lines */
        if (line[0] == ';' || arg == NULL) {
            continue;
        }

        if (isNewSymbol(arg)) {
            symbolFlag = 1;
            arg = strtok(NULL, " ");
        }

        if ((tempDirective = searchDirective(arg)) != NULL) {
            if (symbolFlag) {
                insertSymbol(context.symbolTable, arg, DC, ".data");
            }
            switch (tempDirective) {
                case ".data"
                    /* Code into "word-type" memory */
                    /* Update DC accordingly */
                    continue;
                    break;
                case ".string"
                    /* Code into "word-type" memory */
                    /* Update DC accordingly */
                    continue;
                    break;
                
                case ".mat"
                    /* Code into "word-type" memory */
                    /* Update DC accordingly */
                    continue;
                    break;
                
                case ".entry"
                    continue; /* go to next line, will take care of it in second iteration */
                    break;
                
                case ".extern"
                    insertSymbol(context.symbolTable, arg, 0, ".external");
                    continue; /* go to next line, will take care of it in second iteration */
                    break;
                default
                    break;
            }
        } else {
            if (symbolFlag) {
                insertSymbol(context.symbolTable, arg, IC, ".code");
            }
            if (tempOperation = searchOperation(arg) == NULL) {
                printf("ERROR in op name (12 in algo)");
            }
            /* 13: Calculate op word count in variable L */
            /* 14: Code words into word-types */
        }
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
    if (len <= 30 || isalpha(str[0]) || str[len - 1] == ':' ||
        !isKeyword(context, str)) {
        return TRUE;
    }
    return FALSE;
}