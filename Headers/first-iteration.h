#ifndef FIRST_ITERATION_H
#define FIRST_ITERATION_H

#include "main.h"
#include "word-types.h"

int firstIteration(char *fileName, assemblerContext *context, int *ICF,
                   int *DCF);
int isKeyword(assemblerContext *context, char *str);
int isNewSymbol(assemblerContext *context, char *str);
int handleDirective(assemblerContext *context, char *str, int *DC,
                    int symbolFlag);
int handleOperation(assemblerContext *context, char *str, int *IC,
                    int symbolFlag);

#endif