#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/op-handlers.h"

void handleOperation(assemblerContext *context, const operation *op, int *IC,
                     char *symbolName) {
    char *operand1, *operand2;
    unsigned int method1 = INVALID, method2 = INVALID;
    cmdFirstWord firstWord;

    if (symbolName != NULL) {
        insertSymbol((*context).symbolTable, symbolName, *IC, TYPE_CODE);
    }

    operand1 = strtok(NULL, ", \t");
    operand2 = strtok(NULL, " \t");
    if (strtok(NULL, " \t") != NULL) {
        /* error - too many operands */
        return;
    }

    if (operand1 != NULL)
        method1 = findAddressMethod(context, operand1);
    if (operand2 != NULL)
        method2 = findAddressMethod(context, operand2);

    if (method1 != INVALID)
        firstWord.src_op_bits = method1;
    if (method2 != INVALID)
        firstWord.dest_op_bits = method2;

    switch ((*op).number) {
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        handleTwoOperandOp(op, firstWord, operand1, operand2, method1, method2,
                           context);
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
        handleOneOperandOp(op, firstWord, operand1, method1, context);
        break;

    case RTS:
    case STP:
        handleNoOperandOp(op, firstWord, context);
        break;

    default:
        break;
    }
}

unsigned int findAddressMethod(assemblerContext *context, char *operand) {
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

void handleTwoOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, char *operand2, unsigned int method1,
                        unsigned int method2, assemblerContext *context) {
    registerAddressingWord regWord;
    const registerInfo *reg1, *reg2;
    firstWord.opcode_bits = (*op).number;
    /* insertLineData(..., firstWord); */
    if (method1 == REGISTER && method2 == REGISTER) {
        reg1 = searchRegister(*(*context).registers, operand1);
        reg2 = searchRegister(*(*context).registers, operand2);
        regWord.src_reg_bits = (*reg1).number;
        regWord.dest_reg_bits = (*reg2).number;
        /* insertLineData(..., regWord); */
    } else {
        /* continue program*/
    }
}

void handleOneOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, unsigned int method1,
                        assemblerContext *context) {
    firstWord.opcode_bits = (*op).number;
}

void handleNoOperandOp(const operation *op, cmdFirstWord firstWord,
                       assemblerContext *context) {
    firstWord.opcode_bits = (*op).number;
}

int isImmediateAddressing(char *operand) {
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

int isDirectAddressing(symbolTable sHead, char *operand) {
    symbol *op = searchSymbol(sHead, operand);
    if (op == NULL || ((*op).type != TYPE_DATA && (*op).type != TYPE_EXTERNAL))
        return FALSE;
    return TRUE;
}

int isMatrixAddressing(const registers regs, char *operand) {
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

int isRegisterAddressing(const registers regs, char *operand) {
    if (searchRegister(regs, operand) == NULL)
        return FALSE;
    return TRUE;
}