#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "word-types.h"
/* Word that is handled only in the second iteration */
#define NO_WORD_YET "??????????"

int firstIteration(char *fileName, assemblerContext *context, int *ICF,
                   int *DCF);
int isKeyword(assemblerContext *context, char *str);
int isNewSymbol(assemblerContext *context, char *str);
void adjustDataSymbolAddresses(symbolTable sHead, int *pICF);
int handleDirective(assemblerContext *context, char *str, int *DC,
                    int symbolFlag);
int handleOperation(assemblerContext *context, char *str, int *IC,
                    int symbolFlag);

#endif