#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "dir-handlers.h"
#include "op-handlers.h"

#define ASSEMBLER_MAX_MEMORY 255

int isNewSymbol(assemblerContext *context, char *str);
int isKeyword(assemblerContext *context, char *str);
void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);

#endif