#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#include "main.h"

#define MACRO_LABEL_BUFF 31
#define MEMORY_ALLOCATION_ERROR -1

enum states { EMPTY_LINE, MACRO_SPREAD, MACRO_DEFINE, NORMAL_LINE };

/* Function declarations */
int preAssembler(char *fileName, assemblerContext *context);
int handleEmptyLine(char **amContent, size_t *contentCapacity, int lineNum, assemblerContext *context);
int handleMacroSpread(macro *tempMacro, char **argP, char **amContent, size_t *contentCapacity, int lineNum,
                      assemblerContext *context);
int handleMacroDefinition(char **argP, assemblerContext *context, FILE *srcFile, int *lineNumP);
int addToContent(const char *text, char **amContentP, size_t *contentCapacityP, int lineNum, assemblerContext *context);
int createOutputFile(const char *fileName, const char *amContent, int lineNum, assemblerContext *context);
int isValidMacroLabel(char *arg, assemblerContext *context);

#endif