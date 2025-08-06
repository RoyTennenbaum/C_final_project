#ifndef OP_HANDLERS_H
#define OP_HANDLERS_H

#include "main.h"
#include "op-word-types.h"

typedef enum {
    IMMEDIATE = 0,
    DIRECT,
    MATRIX,
    REGISTER,
    INVALID
} addressingType;

void handleOperation(assemblerContext *context, const operation *op, int *IC,
                     char *symbolName);

void handleTwoOperandOp(const operation *op, const char *operand1,
                        const char *operand2, assemblerContext *context);
void handleOneOperandOp(const operation *op, const char *operand1,
                        assemblerContext *context);
void handleNoOperandOp(const operation *op, assemblerContext *context);

opFirstWord encodeOpFirstWord(const operation *op, const char *operand1,
                              const char *operand2, unsigned int *pMethod1,
                              unsigned int *pMethod2,
                              assemblerContext *context);

unsigned int findAddressMethod(assemblerContext *context, const char *operand);
int isImmediateAddressing(const char *operand);
int isDirectAddressing(symbolTable sHead, const char *operand);
int isMatrixAddressing(const registers regs, const char *operand);
int isRegisterAddressing(const registers regs, const char *operand);

#endif