#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "dir-handlers.h"
#include "op-handlers.h"

#define ASSEMBLER_MAX_MEMORY 255

int isNewSymbol(char *str, int lineNum, int *errorFlag, assemblerContext *context);
int isKeyword(char *str, int lineNum, int *errorFlag, assemblerContext *context);
void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);

#endif