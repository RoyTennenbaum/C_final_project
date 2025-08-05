#ifndef OP_HANDLERS_H
#define OP_HANDLERS_H

#include "main.h"

typedef enum {
    IMMEDIATE = 0,
    DIRECT,
    MATRIX,
    REGISTER,
    INVALID
} addressingType;

/* 10-bit word that is handled only in the second iteration */
#define NO_WORD_YET "??????????"

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int dest_op_bits : 2;
    unsigned int src_op_bits : 2;
    unsigned int opcode_bits : 4;
} cmdFirstWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int value_bits : 8;
} immediateAddressingWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int value_bits : 8;
} directAddressingWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int label_address_bits : 8;
} matrixAddressingFirstWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int col_reg_bits : 3;
    unsigned int unused_padding_1 : 1;
    unsigned int row_reg_bits : 3;
    unsigned int unused_padding_2 : 1;
} matrixAddressingSecondWord;

typedef struct {
    unsigned int aer_bits : 2;
    unsigned int dest_reg_bits : 3;
    unsigned int unused_padding_1 : 1;
    unsigned int src_reg_bits : 3;
    unsigned int unused_padding_2 : 1;
} registerAddressingWord;

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