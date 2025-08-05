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

unsigned int findAddressMethod(assemblerContext *context, char *operand);

void handleTwoOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, char *operand2, unsigned int method1,
                        unsigned int method2, assemblerContext *context);
void handleOneOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, unsigned int method1,
                        assemblerContext *context);
void handleNoOperandOp(const operation *op, cmdFirstWord firstWord,
                       assemblerContext *context);

int isImmediateAddressing(char *operand);
int isDirectAddressing(symbolTable sHead, char *operand);
int isMatrixAddressing(const registers regs, char *operand);
int isRegisterAddressing(const registers regs, char *operand);

#endif