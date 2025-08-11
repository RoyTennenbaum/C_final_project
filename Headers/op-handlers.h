#ifndef OP_HANDLERS_H
#define OP_HANDLERS_H

#include "main.h"
#include "word-types.h"

typedef enum {
    IMMEDIATE = 0,
    DIRECT,
    MATRIX,
    REGISTER,
    INVALID
} addressingType;

void handleOperation(const operation *op, int *IC, char *symbolName,
                     binaryWordList *opList, assemblerContext *context);

void handleTwoOperandOp(const operation *op, const char *operand1,
                        const char *operand2, int *IC, binaryWordList *opList,
                        assemblerContext *context);
void handleOneOperandOp(const operation *op, const char *operand1, int *IC,
                        binaryWordList *opList, assemblerContext *context);
void handleNoOperandOp(const operation *op, int *IC, binaryWordList *opList,
                       assemblerContext *context);

void encodeOpFirstWord(const operation *op, const char *operand1,
                       const char *operand2, addressingType *pMethod1,
                       addressingType *pMethod2, int *L, int *IC,
                       binaryWordList *opList, assemblerContext *context);
void encodeTwoRegisters(const char *operand1, const char *operand2, int *L,
                        int *IC, binaryWordList *opList,
                        assemblerContext *context);
void encodeOperand(const char *operand, addressingType method, int *L, int *IC,
                   binaryWordList *opList, assemblerContext *context);

addressingType getExpectedAddressMethod(assemblerContext *context,
                                        const char *operand);
int isImmediateAddressExpected(const char *operand);
int isDirectAddressExpected(symbolTable sHead, const char *operand);
int isMatrixAddressExpected(const registers regs, const char *operand);
int isRegisterAddressExpected(const registers regs, const char *operand);

#endif