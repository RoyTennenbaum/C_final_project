#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "word-types.h"
/* 10-bit word that is handled only in the second iteration */
#define NO_WORD_YET "??????????"

int firstIteration(char *fileName, assemblerContext *context, int *ICF,
                   int *DCF);

int isKeyword(assemblerContext *context, char *str);
int isNewSymbol(assemblerContext *context, char *str);

void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);

int isImmediateAddressing(char *operand);
int isDirectAddressing(symbolTable sHead, char *operand);
int isMatrixAddressing(const registers regs, char *operand);
int isRegisterAddressing(const registers regs, char *operand);
unsigned int findAddressMethod(assemblerContext *context, char *operand);

void handleDirective(assemblerContext *context, const directive *dir, int *DC,
                     char *symbolName);

void handleTwoOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, char *operand2, unsigned int method1,
                        unsigned int method2, assemblerContext *context);
void handleOneOperandOp(const operation *op, cmdFirstWord firstWord,
                        char *operand1, unsigned int method1,
                        assemblerContext *context);
void handleNoOperandOp(const operation *op, cmdFirstWord firstWord,
                       assemblerContext *context);
void handleOperation(assemblerContext *context, const operation *op, int *IC,
                     char *symbolName);

#endif