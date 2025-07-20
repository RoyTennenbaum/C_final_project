#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../Headers/static-tables.h"

int firstIteration(char *fileName, context) {
    int IC = 100, DC = 0;

    char *line, *arg;
    int lineNum = 0; /* Mark line number for future error messages */
    FILE *fp = fopen(fileName, "r");

    int symbolFlag = 0;

    if (fp == NULL) {
        printf("Error: file could not be opened.");
        return 1;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;

        arg = strtok(line, " ");

        if (isNewSymbol(arg)) {
            symbolFlag = 1;
        }

        if (isDirective(arg)) {
            insert(context.symbolTable, arg, DC);
            /* if not directive, go to 8 in algo */
        }
    }

    fclose(fp);
    return 0;
}

int isNewSymbol(char *str) {
    site_t len = strlen(str);
    /* failsafe to ensure len-1 will always be valid */
    if (len < 1) {
        return 0;
    }
    if (len > 30 || !isAlpha(str[0]) || strcmp(str[len - 1], ':') != 0 ||
        isOperation(str) || isDirective(str) || isSymbol(str) || isMacro(str)) {
        return 0;
    }
    return 1;
}