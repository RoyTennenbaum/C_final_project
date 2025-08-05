#ifndef DIR_HANDLERS_H
#define DIR_HANDLERS_H

#include "main.h"

typedef struct {
    unsigned int data_bits : 10;
} directiveWord;

void handleDirective(assemblerContext *context, const directive *dir, int *DC,
                     char *symbolName);

#endif