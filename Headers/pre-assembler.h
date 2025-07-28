#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "main.h"

#define MACRO_LABEL_BUFF 31

#define MEMORY_ALLOCATION_ERROR -1

enum states
{
    EMPTY_LINE,
    MACRO_SPREAD,
    MACRO_DEFINE,
    NORMAL_LINE
};

/* Function declarations */
int preAssembler(char *srcFileName, assemblerContext *context);
int handleMacroSpread(macro *tempMacro, char *firstArg, char **amContent, size_t *contentCapacity);
int handleMacroDefinition(char *firstArg, assemblerContext *context, FILE *srcFile, int *lineNum);
int addToContent(char **amContent, size_t *contentCapacity, const char *text);
int createOutputFile(const char *srcFileName, const char *amContent);
int isValidMacroLabel(char *arg, assemblerContext *context);
#endif