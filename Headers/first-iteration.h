#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "dir-handlers.h"
#include "op-handlers.h"

int isNewSymbol(assemblerContext *context, char *str);
int isKeyword(assemblerContext *context, char *str);
void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);

#endif