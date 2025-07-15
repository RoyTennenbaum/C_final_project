#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../Headers/static-tables.h"

int firstIteration(char *fileName) {
    int IC = 0, DC = 0;

    char *line, *arg;
    int lineNum = 0; /* Mark line number for future error messages */
    FILE *fp = fopen(fileName, "r");

    if (fp == NULL) {
        printf("Error: file could not be opened.");
        return 1;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;

        arg = strtok(line, " ");

        if (isValidSymbol(arg)) {
            symbolFlag = 1;
        }
    }

    fclose(fp);
    return 0;
}

int isValidSymbol(char *str) {
    site_t len = strlen(str);
    if (len > 30 || !isAlpha(str[0]) || strcmp(str[len - 1], ':') != 0 ||
        isOperation(str) || isDirective(str) || isSymbol(str) || isMacro(str)) {
        return 0;
    }
    return 1;
}