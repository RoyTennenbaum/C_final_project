#ifndef DIR_HANDLERS_H
#define DIR_HANDLERS_H

#include "main.h"
#include "word-types.h"

void handleDirective(assemblerContext *context, const directive *dir, int *DC,
                     char *symbolName);

#endif