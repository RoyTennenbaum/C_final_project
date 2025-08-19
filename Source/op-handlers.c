#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/op-handlers.h"

void handleOperation(const operation *op, int *IC, char *symbolName, binaryWordList *opList, int lineNum,
                     int *errorFlag, assemblerContext *context) {
    char *operand1, *operand2;
    operand1 = strtok(NULL, ", \t\n");
    printf("First operand is: '%s' \n", operand1);
    operand2 = strtok(NULL, ", \t\n");
    printf("Second operand is: '%s' \n", operand2);

    if (symbolName != NULL) {
        insertSymbol((*context).symbolTable, symbolName, *IC, TYPE_CODE, lineNum);
    }

    if (strtok(NULL, " \t") != NULL) {
        *errorFlag = TRUE;
        insertError((*context).errorList, ERR_TOO_MANY_OPERANDS, lineNum);
    } else {
        switch ((*op).number) {
        case MOV:
        case CMP:
        case ADD:
        case SUB:
        case LEA:
            if (operand1 == NULL || operand2 == NULL) {
                *errorFlag = TRUE;
                insertError((*context).errorList, ERR_MISSING_OPERANDS, lineNum);
            } else {
                handleTwoOperandOp(op, operand1, operand2, IC, opList, lineNum, errorFlag, context);
            }
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
            if (operand1 == NULL) {
                *errorFlag = TRUE;
                insertError((*context).errorList, ERR_MISSING_OPERANDS, lineNum);
            } else if (operand2 != NULL) {
                *errorFlag = TRUE;
                insertError((*context).errorList, ERR_TOO_MANY_OPERANDS, lineNum);
            } else {
                handleOneOperandOp(op, operand1, IC, opList, lineNum, errorFlag, context);
            }
            break;

        case RTS:
        case STP:
            if (operand1 != NULL) {
                *errorFlag = TRUE;
                insertError((*context).errorList, ERR_TOO_MANY_OPERANDS, lineNum);
            } else {
                handleNoOperandOp(op, IC, opList, lineNum, errorFlag, context);
            }
            break;

        default:
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INTERNAL, lineNum);
            break;
        }
    }
}

void handleTwoOperandOp(const operation *op, const char *operand1, const char *operand2, int *IC,
                        binaryWordList *opList, int lineNum, int *errorFlag, assemblerContext *context) {
    addressingType method1, method2;
    int L = 0;

    encodeOpFirstWord(op, operand1, operand2, &method1, &method2, &L, IC, opList, lineNum, errorFlag, context);

    if (method1 == REGISTER && method2 == REGISTER) {
        encodeTwoRegisters(operand1, operand2, &L, IC, opList, lineNum, errorFlag, context);
    } else if (method1 == REGISTER) {
        encodeTwoRegisters(operand1, NULL, &L, IC, opList, lineNum, errorFlag, context);
        encodeOperand(operand2, method2, &L, IC, opList, lineNum, errorFlag, context);
    } else if (method2 == REGISTER) {
        encodeOperand(operand1, method1, &L, IC, opList, lineNum, errorFlag, context);
        encodeTwoRegisters(NULL, operand2, &L, IC, opList, lineNum, errorFlag, context);
    } else {
        encodeOperand(operand1, method1, &L, IC, opList, lineNum, errorFlag, context);
        encodeOperand(operand2, method2, &L, IC, opList, lineNum, errorFlag, context);
    }

    /* increase IC by the number of machine code words the instruction occupies */
    (*IC) += L;
}

void handleOneOperandOp(const operation *op, const char *operand1, int *IC, binaryWordList *opList, int lineNum,
                        int *errorFlag, assemblerContext *context) {
    unsigned int dummyMethod, method;
    int L = 0;

    encodeOpFirstWord(op, operand1, NULL, &dummyMethod, &method, &L, IC, opList, lineNum, errorFlag, context);
    encodeOperand(operand1, method, &L, IC, opList, lineNum, errorFlag, context);

    /* increase IC by the number of machine code words the instruction occupies */
    (*IC) += L;
}

void handleNoOperandOp(const operation *op, int *IC, binaryWordList *opList, int lineNum, int *errorFlag,
                       assemblerContext *context) {
    unsigned int dummyMethod1, dummyMethod2;
    int L = 0;

    encodeOpFirstWord(op, NULL, NULL, &dummyMethod1, &dummyMethod2, &L, IC, opList, lineNum, errorFlag, context);

    /* increase IC by the number of machine code words the instruction occupies */
    (*IC) += L;
}

void encodeOpFirstWord(const operation *op, const char *operand1, const char *operand2, addressingType *pMethod1,
                       addressingType *pMethod2, int *L, int *IC, binaryWordList *opList, int lineNum, int *errorFlag,
                       assemblerContext *context) {
    WordType word;

    /* Initialize the 'WordType' union with zeros */
    memset(&word, 0, sizeof(word));

    word.opFirst.opcode_bits = (*op).number;

    if (operand1 && operand2) {
        *pMethod1 = getExpectedAddressMethod(context, operand1);
        printf("method1: %d\n", *pMethod1);
        *pMethod2 = getExpectedAddressMethod(context, operand2);
        printf("method2: %d\n", *pMethod2);

        if (*pMethod1 == INVALID || *pMethod2 == INVALID) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_ADDRESSING_METHOD, lineNum);
            return;
        }

        /* handle special invalid addressing method cases (table in page 35 of the course pdf) */
        if ((*pMethod1 == IMMEDIATE || *pMethod1 == REGISTER) && (*op).number == LEA) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_OPERAND, lineNum);
            return;
        }
        if (*pMethod2 == IMMEDIATE &&
            ((*op).number == MOV || (*op).number == ADD || (*op).number == SUB || (*op).number == LEA)) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_OPERAND, lineNum);
            return;
        }

        word.opFirst.src_op_bits = *pMethod1;
        word.opFirst.dest_op_bits = *pMethod2;
    } else if (operand1) {
        *pMethod1 = 0;
        *pMethod2 = getExpectedAddressMethod(context, operand1);
        printf("method2: %d\n", *pMethod2);

        if (*pMethod2 == INVALID) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_ADDRESSING_METHOD, lineNum);
        }

        /* handle special invalid addressing method cases (table in page 35 of the course pdf) */
        if (*pMethod2 == IMMEDIATE &&
            ((*op).number == CLR || (*op).number == NOT || (*op).number == INC || (*op).number == DEC ||
             (*op).number == JMP || (*op).number == BNE || (*op).number == JSR || (*op).number == RED)) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_OPERAND, lineNum);
            return;
        }

        /* If only one operand, it's the destination operand */
        word.opFirst.src_op_bits = 0;
        word.opFirst.dest_op_bits = *pMethod2;

    } else {
        *pMethod1 = 0;
        *pMethod2 = 0;
    }

    /* aer taken care of in second iteration. In first iteration we set them explicitly to zero */
    word.opFirst.aer_bits = 0;

    insertBinaryWord(opList, *IC, *L, word, OP_FIRST, lineNum);
    if (fatalError)
        return;
    else
        (*L)++;
}

void encodeTwoRegisters(const char *operand1, const char *operand2, int *L, int *IC, binaryWordList *opList,
                        int lineNum, int *errorFlag, assemblerContext *context) {
    WordType word;
    const registerInfo *reg1, *reg2;

    /* Initialize the 'WordType' union with zeros */
    memset(&word, 0, sizeof(word));

    if (operand1) {
        reg1 = searchRegister(*(*context).registers, operand1);
        if (!reg1) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_REGISTER, lineNum);
            return;
        }
        word.regPair.reg1_bits = (*reg1).number;
    }
    if (operand2) {
        reg2 = searchRegister(*(*context).registers, operand2);
        if (!reg2) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_INVALID_REGISTER, lineNum);
            return;
        }
        word.regPair.reg2_bits = (*reg2).number;
    }

    insertBinaryWord(opList, *IC, *L, word, REG_PAIR, lineNum);
    if (fatalError)
        return;
    else
        (*L)++;
}

void encodeOperand(const char *operand, addressingType method, int *L, int *IC, binaryWordList *opList, int lineNum,
                   int *errorFlag, assemblerContext *context) {
    WordType word;

    /* Initialize the 'WordType' union with zeros */
    memset(&word, 0, sizeof(word));

    if (method == IMMEDIATE) {
        /* Skip the first character '#' and convert the rest to int */
        int num = atoi(operand + 1);

        /* Check valid signed 8-bit range */
        if (num < SIGNED_8_BIT_MIN || num > SIGNED_8_BIT_MAX) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_NUMBER_OUT_OF_RANGE, lineNum);
        } else {
            word.payload.payload_bits = (unsigned int)(num);

            /* aer taken care of in second iteration. In first iteration we set them explicitly to zero */
            word.payload.aer_bits = 0;

            insertBinaryWord(opList, *IC, *L, word, PAYLOAD, lineNum);
            if (fatalError)
                return;
            else
                (*L)++;
        }
    } else if (method == DIRECT) {
        /* Symbol address is taken care of only in second iteration, so we store a zeroed payloadWord */
        word.payload.payload_bits = 0;
        word.payload.aer_bits = 0;
        insertBinaryWord(opList, *IC, *L, word, PAYLOAD, lineNum);
        if (fatalError)
            return;
        else
            (*L)++;
    } else if (method == MATRIX) {
        char reg1Str[3], reg2Str[3];
        size_t len = strlen(operand);

        /* Mat symbol address is taken care of only in second iteration, so we store a zeroed payloadWord */
        word.payload.payload_bits = 0;
        word.payload.aer_bits = 0;
        insertBinaryWord(opList, *IC, *L, word, PAYLOAD, lineNum);
        if (fatalError)
            return;
        else
            (*L)++;

        /* Mat row and col values are known and encoded in the first iteration: */

        /* FailSafe for length */
        if (len < 9) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_MAT_ADDRESS_ILLEGAL_LEN, lineNum);
            return;
        }

        /* Extract registers */
        reg1Str[0] = operand[len - 7];
        reg1Str[1] = operand[len - 6];
        reg1Str[2] = '\0';
        reg2Str[0] = operand[len - 3];
        reg2Str[1] = operand[len - 2];
        reg2Str[2] = '\0';

        if (searchRegister(*(*context).registers, reg1Str) == NULL ||
            searchRegister(*(*context).registers, reg2Str) == NULL) {
            *errorFlag = TRUE;
            insertError((*context).errorList, ERR_MAT_INVALID_REGISTERS, lineNum);
            return;
        }

        encodeTwoRegisters(reg1Str, reg2Str, L, IC, opList, lineNum, errorFlag, context);
    }
}

addressingType getExpectedAddressMethod(assemblerContext *context, const char *operand) {
    if (isImmediateAddressExpected(operand))
        return IMMEDIATE;
    else if (isRegisterAddressExpected(*(*context).registers, operand))
        return REGISTER;
    else if (isMatrixAddressExpected(*(*context).registers, operand))
        return MATRIX;
    else if (isDirectAddressExpected(*(*context).symbolTable, operand))
        return DIRECT;
    else
        return INVALID;
}

int isImmediateAddressExpected(const char *operand) {
    int i;
    size_t len;

    if (!operand)
        return FALSE;
    if (operand[0] != '#')
        return FALSE;

    len = strlen(operand);

    if (len < 2) /* must have something after '#' */
        return FALSE;

    i = 1;
    if (operand[i] == '+' || operand[i] == '-') /* allow +/- signs */
        i++;

    if (i >= len) /* no digits after '#' or "#-" */
        return FALSE;

    for (; i < len; i++) {
        if (!isdigit((unsigned char)operand[i]))
            return FALSE;
    }

    return TRUE;
}

int isDirectAddressExpected(symbolTable sHead, const char *operand) {
    size_t len = strlen(operand);
    if (len > 30 || !isalpha(operand[0]))
        return FALSE;
    return TRUE;
}

int isMatrixAddressExpected(const registers regs, const char *operand) {
    size_t len = strlen(operand);
    char reg1[3], reg2[3];
    int labelBuf = 30;
    size_t labelLen = len - 8;

    /* The format should be at least 9 chars, for it contains "M[rX][rY]" */
    if (len < 9) {
        return FALSE;
    }

    /* Check brackets at specific positions */
    if (operand[len - 8] != '[' || operand[len - 5] != ']' || operand[len - 4] != '[' || operand[len - 1] != ']') {
        return FALSE;
    }

    /* Extract registers */
    reg1[0] = operand[len - 7];
    reg1[1] = operand[len - 6];
    reg1[2] = '\0';
    reg2[0] = operand[len - 3];
    reg2[1] = operand[len - 2];
    reg2[2] = '\0';

    if (searchRegister(regs, reg1) == NULL || searchRegister(regs, reg2) == NULL) {
        return FALSE;
    }

    /* Label has non-positive length or too long */
    if (labelLen <= 0 || labelLen >= labelBuf) {
        return FALSE;
    }

    return TRUE;
}

int isRegisterAddressExpected(const registers regs, const char *operand) {
    if (searchRegister(regs, operand) == NULL)
        return FALSE;
    return TRUE;
}