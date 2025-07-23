#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/global.h"
#include "../Headers/dynamic-tables.h"
#include "../Headers/static-tables.h"

#define MACRO_LABEL_BUFF 31

enum states
{
    MACRO_SPREAD,
    MACRO_DEFINE,
    NORMAL_LINE
};

/* Function declarations */
char *preAssembler(char *srcFileName, assemblerContext context);
int isValidMacroLabel(char *arg, assemblerContext context);

#endif /* PRE_ASSEMBLER_H */