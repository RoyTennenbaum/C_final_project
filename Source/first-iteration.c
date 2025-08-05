#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, assemblerContext *context, int *pICF,
                   int *pDCF) {
    int IC = 0, DC = 0, lineNum = 0, errorFlag = 0;
    char line[LINE_SIZE], *arg, *newSymbolName;
    const directive *dir;
    const operation *op;

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        errorFlag = 1;
        printf("Error: file could not be opened.");
        return 1;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;
        newSymbolName = NULL;
        /* Get the first word in the line */
        arg = strtok(line, " \t");

        /* Skip comment lines and empty lines */
        if (line[0] == ';' || arg == NULL) {
            continue;
        }

        if (isNewSymbol(context, arg)) {
            newSymbolName = arg;
            /* Store the next word of the current line in arg */
            arg = strtok(NULL, " \t");
            /* Check if symbol is followed by a directive or an instruction */
            if (arg == NULL) {
                errorFlag = 1;
                printf("ERROR: symbol is not followed by a directive or "
                       "an instruction.");
                continue;
            }
        }

        if ((dir = searchDirective(*(*context).directiveTable, arg)) != NULL) {
            handleDirective(context, dir, &DC, newSymbolName);
        } else if ((op = searchOperation(*(*context).operationTable, arg)) !=
                   NULL) {
            handleOperation(context, op, &IC, newSymbolName);
        } else {
            /* ERROR */
        }
    }

    if (errorFlag) {
        /* stop program */
    }

    *pICF = IC;
    *pDCF = DC;

    adjustDataSymbolAddresses(*(*context).symbolTable, pICF);

    fclose(fp);
    return TRUE;
}

int isNewSymbol(assemblerContext *context, char *str) {
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

int isKeyword(assemblerContext *context, char *str) {
    if (searchSymbol(*(*context).symbolTable, str) ||
        searchMacro(*(*context).macroTable, str) ||
        searchOperation(*(*context).operationTable, str) ||
        searchDirective(*(*context).directiveTable, str) ||
        searchRegister(*(*context).registers, str)) {
        return TRUE;
    }
    return FALSE;
}

void adjustDataSymbolAddresses(symbolTable sHead, int *pICF) {
    symbolTable pCurrentSymbol = sHead;

    while (pCurrentSymbol != NULL) {
        if ((*pCurrentSymbol).type == TYPE_DATA) {
            (*pCurrentSymbol).address += (*pICF);
        }
        pCurrentSymbol = (*pCurrentSymbol).next;
    }
}