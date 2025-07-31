#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/first-iteration.h"

int firstIteration(char *fileName, assemblerContext *context, int *pICF,
                   int *pDCF) {
    char line[LINE_SIZE], *arg;
    int lineNum = 0; /* Mark line number for future error messages */

    int IC = 0, DC = 0, symbolFlag = 0, errorFlag = 0;

    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) {
        errorFlag = 1;
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

        if (isNewSymbol(context, arg)) {
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

        /* If we reached this point, the line must be an operation sentence */
        if (symbolFlag) {
            insertSymbol((*context).symbolTable, arg, IC, TYPE_CODE);
        }
        if (handleOperation(context, arg, &IC, symbolFlag)) {
            continue;
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

void adjustDataSymbolAddresses(symbolTable sHead, int *pICF) {
    symbolTable pCurrentSymbol = sHead;

    while (pCurrentSymbol != NULL) {
        if ((*pCurrentSymbol).type == TYPE_DATA) {
            (*pCurrentSymbol).address += (*pICF);
        }
        pCurrentSymbol = (*pCurrentSymbol).next;
    }
}

int handleDirective(assemblerContext *context, char *str, int *DC,
                    int symbolFlag) {
    const directive *dir = searchDirective(*(*context).directiveTable, str);

    if (dir == NULL) {
        /* not a directive */
        return FALSE;
    }

    switch ((*dir).type) {
    case DATA:
        if (symbolFlag) {
            insertSymbol((*context).symbolTable, str, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* encodeDataDir()*/
        /* Update DC accordingly */
        return TRUE;

    case STRING:
        if (symbolFlag) {
            insertSymbol((*context).symbolTable, str, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        return TRUE;

    case MAT:
        if (symbolFlag) {
            insertSymbol((*context).symbolTable, str, *DC, TYPE_DATA);
        }
        /* Code into "word-type" memory */
        /* Update DC accordingly */
        return TRUE;

    case ENTRY:
        /* handled in second iteration */
        return TRUE;

    case EXTERN:
        insertSymbol((*context).symbolTable, str, 0, TYPE_EXTERNAL);
        /* handled in second iteration */
        return TRUE;

    default:
        return FALSE;
    }
    return FALSE;
}

int handleOperation(assemblerContext *context, char *str, int *IC,
                    int symbolFlag) {
    const operation *op = searchOperation(*(*context).operationTable, str);
    /*int L = 0;
    cmdFirstWord cmdWord;*/

    if (op == NULL) {
        /* error */
        return FALSE;
    }

    switch ((*op).number) {
    case MOV:
        /* Handle MOV operation */
        /* Update IC accordingly */
        /*cmdWord.opcode_bits = 0U;
        str = strtok(NULL, " \t\n");
        cmdWord.src_op_bits = 2U;
        L += 4;
        return TRUE;*/
        return TRUE;

    case CMP:
        /* Handle CMP operation */
        /* Update IC accordingly */
        return TRUE;

    case ADD:
        /* Handle ADD operation */
        /* Generate opcode for addition */
        return TRUE;

    case SUB:
        /* Handle SUB operation */
        /* Update instruction format */
        return TRUE;

    case NOT:
        /* Handle NOT operation */
        /* Single operand */
        return TRUE;

    case CLR:
        /* Handle CLR operation */
        /* Use destination operand only */
        return TRUE;

    case LEA:
        /* Handle LEA operation */
        /* Load effective address */
        return TRUE;

    case INC:
        /* Handle INC operation */
        /* Unary operation */
        return TRUE;

    case DEC:
        /* Handle DEC operation */
        /* Decrement register/memory */
        return TRUE;

    case JMP:
        /* Handle JMP operation */
        /* Control transfer */
        return TRUE;

    case BNE:
        /* Handle BNE operation */
        /* Conditional jump */
        return TRUE;

    case RED:
        /* Handle RED operation */
        /* Read input to destination */
        return TRUE;

    case PRN:
        /* Handle PRN operation */
        /* Print numeric value */
        return TRUE;

    case JSR:
        /* Handle JSR operation */
        /* Jump to subroutine */
        return TRUE;

    case RTS:
        /* Handle RTS operation */
        /* Return from subroutine */
        return TRUE;

    case STP:
        /* Handle STOP operation */
        /* Halts program */
        return TRUE;

    default:
        return FALSE;
    }
    return FALSE;
}