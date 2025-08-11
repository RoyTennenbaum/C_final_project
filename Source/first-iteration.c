#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, int *pICF, int *pDCF,
                   binaryWordList *codeImage, assemblerContext *context) {
    int IC = 0, DC = 0, lineNum = 0, errorFlag = 0;
    binaryWordList dirList = NULL, opList = NULL;
    char line[LINE_SIZE], *arg, *newSymbolName, *colonPos;
    const directive *dir;
    const operation *op;

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        errorFlag = 1;
        printf("Error: file '%s' could not be opened.\n", fileName);
        return 1;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;
        printf("\nLINE #%d\n", lineNum);
        newSymbolName = NULL;

        /* Get the first word in the line */
        arg = strtok(line, " \t");
        printf("The first word is: '%s'\n", arg);

        /* Skip comment lines and empty lines */
        if (line[0] == ';' || arg == NULL) {
            continue;
        }

        if (isNewSymbol(context, arg)) {
            /* replace the colon in the symbol with '\0' */
            colonPos = strchr(arg, ':');
            *colonPos = '\0';

            newSymbolName = arg;

            /* Store the next word of the current line in arg */
            arg = strtok(NULL, " \t");
            printf("First word was a symbol. Second word is: '%s'\n", arg);

            /* Check if symbol is followed by a directive or an instruction */
            if (arg == NULL) {
                errorFlag = 1;
                printf("ERROR: symbol is not followed by a directive or "
                       "an instruction.\n");
                continue;
            }
        }

        if ((dir = searchDirective(*(*context).directiveTable, arg)) != NULL) {
            handleDirective(dir, &DC, newSymbolName, &dirList, context);
        } else if ((op = searchOperation(*(*context).operationTable, arg)) !=
                   NULL) {
            handleOperation(op, &IC, newSymbolName, &opList, context);
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