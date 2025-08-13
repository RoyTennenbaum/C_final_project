#ifndef DIR_HANDLERS_H
#define DIR_HANDLERS_H

#include "main.h"
#include "word-types.h"

void handleDirective(const directive *dir, int *DC, char *symbolName, binaryWordList *dirList, int lineNum,
                     int *errorFlag, assemblerContext *context);

void encodeDataDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                   assemblerContext *context);
void encodeStrDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                  assemblerContext *context);
void encodeMatDir(const directive *dir, int *DC, binaryWordList *dirList, int lineNum, int *errorFlag,
                  assemblerContext *context);
#endif