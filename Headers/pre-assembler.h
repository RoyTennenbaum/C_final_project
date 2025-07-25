#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "../Headers/main.h"

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

#endif