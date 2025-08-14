#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, int *pICF, int *pDCF, binaryWordList *codeImage, assemblerContext *context) {
    /* File handler */
    FILE *fp;
    /* Counters */
    int IC = 100, DC = 0;
    /* Error handling vars */
    int lineNum = 0, errorFlag = FALSE;
    /* Parsing vars */
    char line[LINE_SIZE];
    char *arg, *newSymbolName, *colonPos;
    const directive *dir;
    const operation *op;
    /* List vars */
    binaryWordNode *dirPtr, *opPtr;
    binaryWordList dirList = NULL, opList = NULL;

    /* Allocate memory for source filename with .am extension */
    char *srcFileName = malloc(strlen(fileName) + 4);
    if (srcFileName == NULL) {
        insertError((*context).errorList, ERR_MEM_ALLOC, lineNum);
        return FALSE;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    fp = fopen(srcFileName, "r");
    if (fp == NULL) {
        insertError((*context).errorList, ERR_FILE_OPEN, lineNum);
        return FALSE;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        lineNum++;
        printf("\nLINE #%d\n", lineNum);
        newSymbolName = NULL;

        /* Get the first word in the line */
        arg = strtok(line, " \t\n");
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
            arg = strtok(NULL, " \t\n");
            printf("First word was a symbol. Second word is: '%s'\n", arg);

            /* Check if symbol is followed by a directive or an instruction */
            if (arg == NULL) {
                errorFlag = TRUE;
                insertError((*context).errorList, ERR_SYMBOL_NOT_FOLLOWED, lineNum);
            }
        }

        if ((dir = searchDirective(*(*context).directiveTable, arg)) != NULL) {
            handleDirective(dir, &DC, newSymbolName, &dirList, lineNum, &errorFlag, context);
        } else if ((op = searchOperation(*(*context).operationTable, arg)) != NULL) {
            handleOperation(op, &IC, newSymbolName, &opList, lineNum, &errorFlag, context);
        } else {
            errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_COMMAND, lineNum);
        }
    }

    /* If program had errors, stop running now */
    if (errorFlag)
        return FALSE;

    *pICF = IC;
    *pDCF = DC;

    /* Increase every directive word address by ICF, to separate data from instructions */
    dirPtr = dirList;
    while (dirPtr != NULL) {
        (*dirPtr).C += (*pICF);
        dirPtr = (*dirPtr).next;
    }

    /* append dirList to opList to get the full code image */
    if (opList == NULL) {
        /* If opList empty, codeImage contains only dirList */
        *codeImage = dirList;
    } else {
        /* If opList not empty, point codeImage to opList */
        *codeImage = opList;

        /* Append dirList to the end of opList */
        opPtr = opList;
        while ((*opPtr).next != NULL) {
            opPtr = (*opPtr).next;
        }
        (*opPtr).next = dirList;
    }

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
    if (len <= 30 && isalpha(str[0]) && str[len - 1] == ':' && !isKeyword(context, str)) {
        return TRUE;
    }
    return FALSE;
}

int isKeyword(assemblerContext *context, char *str) {
    if (searchSymbol(*(*context).symbolTable, str) || searchMacro(*(*context).macroTable, str) ||
        searchOperation(*(*context).operationTable, str) || searchDirective(*(*context).directiveTable, str) ||
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