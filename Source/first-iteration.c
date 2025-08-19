#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, int *pICF, int *pDCF, binaryWordList *codeImage, assemblerContext *context) {
    FILE *fp;
    /* Counters */
    int IC = 100, DC = 0;
    /* Error handling vars */
    int lineNum = 0, errorFlag = FALSE;
    /* Parsing helpers */
    char line[LINE_SIZE];
    char *arg, *newSymbolName, *colonPos;
    const directive *dir;
    const operation *op;
    /* List handling helpers */
    binaryWordNode *dirPtr, *opPtr;
    binaryWordList dirList = NULL, opList = NULL;

    /* Allocate memory for source file name with .am extension */
    char *srcFileName = malloc(strlen(fileName) + 4);
    if (srcFileName == NULL) {
        setFatalError(lineNum, ERR_MEM_ALLOC);
        return ERROR_FOUND;
    }

    strcpy(srcFileName, fileName);
    strcat(srcFileName, ".am");

    fp = fopen(srcFileName, "r");
    if (fp == NULL) {
        insertError((*context).errorList, ERR_FILE_OPEN, lineNum);
        /* Not a fatal error, but must move to the next test file... */
        return ERROR_FOUND;
    }

    while (fgets(line, LINE_SIZE, fp) != NULL) {
        if (fatalError) {
            fclose(fp);
            free(srcFileName);
            return ERROR_FOUND;
        }
        lineNum++;
        printf("\nLINE #%d\n", lineNum);
        newSymbolName = NULL;

        if (IC + DC > ASSEMBLER_MAX_MEMORY) {
            setFatalError(lineNum, ERR_MEM_ALLOC);
            fclose(fp);
            free(srcFileName);
            return ERROR_FOUND;
        }

        if (strlen(line) >= LINE_SIZE - 1) {
            insertError((*context).errorList, ERR_LINE_TOO_LONG, lineNum);
            errorFlag = TRUE;
            continue;
        }

        /* Get the first word in the line */
        arg = strtok(line, " \t\n");
        printf("The first word is: '%s'\n", arg);

        /* Skip empty lines and comment lines */
        if (arg == NULL || line[0] == ';') {
            continue;
        }

        if (isNewSymbol(arg, lineNum, &errorFlag, context)) {
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
    if (errorFlag) {
        fclose(fp);
        free(srcFileName);
        return ERROR_FOUND;
    }

    /* Save the final instruction and data counters to be used in second iteration */
    *pICF = IC;
    *pDCF = DC;

    /* Increase every directive word address by ICF, to separate data from instructions */
    dirPtr = dirList;
    while (dirPtr != NULL) {
        (*dirPtr).C += (*pICF);
        dirPtr = (*dirPtr).next;
    }

    /* Increase every directive symbol by ICF, so the symbol table matches the code image addresses */
    adjustDataSymbolAddresses(*(*context).symbolTable, pICF);

    /* append dirList to opList to put together the full code image */
    if (opList == NULL && dirList == NULL) {
        *codeImage = NULL;
    } else if (opList == NULL) {
        *codeImage = dirList;
    } else {
        *codeImage = opList;
        /* Appending dirList to the end of opList */
        opPtr = opList;
        while ((*opPtr).next != NULL) {
            opPtr = (*opPtr).next;
        }
        (*opPtr).next = dirList;
    }

    fclose(fp);
    free(srcFileName);
    return NO_ERROR_FOUND;
}

int isNewSymbol(char *str, int lineNum, int *errorFlag, assemblerContext *context) {
    size_t len = strlen(str);
    /* failsafe to ensure len-1 will always be valid */
    if (len < 1) {
        return FALSE;
    }
    if (len < SYMBOL_MAX_LENGTH && isalpha(str[0]) && str[len - 1] == ':' &&
        !isKeyword(str, lineNum, errorFlag, context)) {
        return TRUE;
    }
    return FALSE;
}

int isKeyword(char *str, int lineNum, int *errorFlag, assemblerContext *context) {
    if (searchSymbol(*(*context).symbolTable, str)) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_DUPLICATE_SYMBOL, lineNum);
        return TRUE;
    } else if (searchMacro(*(*context).macroTable, str) || searchOperation(*(*context).operationTable, str) ||
               searchDirective(*(*context).directiveTable, str) || searchRegister(*(*context).registers, str)) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_RESERVED_NAME, lineNum);
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