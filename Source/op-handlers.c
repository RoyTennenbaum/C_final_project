#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/op-handlers.h"

void handleOperation(assemblerContext *context, const operation *op, int *IC,
                     char *symbolName) {
    char *operand1 = strtok(NULL, ", \t");
    char *operand2 = strtok(NULL, " \t");

    if (symbolName != NULL) {
        insertSymbol((*context).symbolTable, symbolName, *IC, TYPE_CODE);
    }

    if (strtok(NULL, " \t") != NULL) {
        printf("ERROR: Too many operands");
        return;
    }

    switch ((*op).number) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        if (operand1 == NULL || operand2 == NULL)
            printf("ERROR: Missing operands");
        handleTwoOperandOp(op, operand1, operand2, context);
        break;

    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case RED:
    case PRN:
    case JSR:
        if (operand1 == NULL)
            printf("ERROR: Missing operand");
        handleOneOperandOp(op, operand1, context);
        break;

    case RTS:
    case STP:
        handleNoOperandOp(op, context);
        break;

    default:
        break;
    }
}

void handleTwoOperandOp(const operation *op, const char *operand1,
                        const char *operand2, assemblerContext *context) {
    unsigned int method1, method2;

    opFirstWord word =
        encodeOpFirstWord(op, operand1, operand2, &method1, &method2, context);
    /*insertLineData(..., word);*/

    /*registerPairWord regWord;
    if (method1 == IMMEDIATE) {
    }
    registerInfo reg1, reg2;
    if (method1 == REGISTER && method2 == REGISTER) {
        reg1 = searchRegister(*(*context).registers, operand1);
        reg2 = searchRegister(*(*context).registers, operand2);
        regWord.reg1_bits = (reg1).number;
        regWord.reg2_bits = (reg2).number;
        insertLineData(..., regWord);
    }*/
}

void handleOneOperandOp(const operation *op, const char *operand1,
                        assemblerContext *context) {
    unsigned int dummyMethod, method;
    opFirstWord word =
        encodeOpFirstWord(op, operand1, NULL, &dummyMethod, &method, context);
    /*insertLineData(..., word);*/
}

void handleNoOperandOp(const operation *op, assemblerContext *context) {
    unsigned int dummyMethod1, dummyMethod2;
    opFirstWord word = encodeOpFirstWord(op, NULL, NULL, &dummyMethod1,
                                         &dummyMethod2, context);
    /*insertLineData(..., word);*/
}

opFirstWord encodeOpFirstWord(const operation *op, const char *operand1,
                              const char *operand2, unsigned int *pMethod1,
                              unsigned int *pMethod2,
                              assemblerContext *context) {
    opFirstWord word;

    word.opcode_bits = (*op).number;

    if (operand1 && operand2) {
        *pMethod1 = findAddressMethod(context, operand1);
        *pMethod2 = findAddressMethod(context, operand2);

        if (*pMethod1 == INVALID || *pMethod2 == INVALID) {
            printf("ERROR: Invalid addressing method");
        }

        word.src_op_bits = *pMethod1;
        word.dest_op_bits = *pMethod2;
    } else if (operand1) {
        *pMethod1 = 0;
        *pMethod2 = findAddressMethod(context, operand1);

        if (*pMethod2 == INVALID)
            printf("ERROR: Invalid addressing method");

        /* If only one operand, it's the destination operand */
        word.src_op_bits = 0;
        word.dest_op_bits = *pMethod2;
    } else {
        *pMethod1 = 0;
        *pMethod2 = 0;
    }

    return word;
}

unsigned int findAddressMethod(assemblerContext *context, const char *operand) {
    if (isImmediateAddressing(operand))
        return IMMEDIATE;
    else if (isDirectAddressing(*(*context).symbolTable, operand))
        return DIRECT;
    else if (isMatrixAddressing(*(*context).registers, operand))
        return MATRIX;
    else if (isRegisterAddressing(*(*context).registers, operand))
        return REGISTER;
    else
        return INVALID;
}

int isImmediateAddressing(const char *operand) {
    int i;
    size_t len = strlen(operand);

    if (operand[0] != '#')
        return FALSE;

    for (i = 1; i < len; i++) {
        if (!isdigit(operand[i]))
            return FALSE;
    }
    return TRUE;
}

int isDirectAddressing(symbolTable sHead, const char *operand) {
    symbol *op = searchSymbol(sHead, operand);
    if (op == NULL || ((*op).type != TYPE_DATA && (*op).type != TYPE_EXTERNAL))
        return FALSE;
    return TRUE;
}

int isMatrixAddressing(const registers regs, const char *operand) {
    size_t len = strlen(operand);
    char reg1[3], reg2[3];
    int labelBuf = LINE_SIZE - len;
    size_t labelLen = len - 8;

    /* The format should be at least 8 chars, for it contains "[rX][rY]" */
    if (len < 8) {
        return 0;
    }

    /* Check brackets at specific positions */
    if (operand[len - 8] != '[' || operand[len - 5] != ']' ||
        operand[len - 4] != '[' || operand[len - 1] != ']')
        return FALSE;

    /* Extract registers */
    reg1[0] = operand[len - 7];
    reg1[1] = operand[len - 6];
    reg1[2] = '\0';
    reg2[0] = operand[len - 3];
    reg2[1] = operand[len - 2];
    reg2[2] = '\0';

    if (searchRegister(regs, reg1) == NULL ||
        searchRegister(regs, reg2) == NULL)
        return FALSE;

    /* Label has non-positive length or too long */
    if (labelLen <= 0 || labelLen >= labelBuf)
        return FALSE;

    return TRUE;
}

int isRegisterAddressing(const registers regs, const char *operand) {
    if (searchRegister(regs, operand) == NULL)
        return FALSE;
    return TRUE;
}