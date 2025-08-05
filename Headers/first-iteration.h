#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "dir-handlers.h"
#include "op-handlers.h"

int firstIteration(char *fileName, assemblerContext *context, int *ICF,
                   int *DCF);
void handleDirective(assemblerContext *context, const directive *dir, int *DC,
                     char *symbolName);
void handleOperation(assemblerContext *context, const operation *op, int *IC,
                     char *symbolName);
int isNewSymbol(assemblerContext *context, char *str);
int isKeyword(assemblerContext *context, char *str);
void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);

#endif