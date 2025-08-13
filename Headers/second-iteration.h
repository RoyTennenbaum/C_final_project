#ifndef SECOND_ITERATION_H
#define SECOND_ITERATION_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "main.h"
#include "word-types.h"

#define MEMORY_ALLOCATION_ERROR -1
#define BASE4_ADDRESS_INIT "aaaa"
#define AER_EXTERNAL_ENCODING 1
#define AER_RELOCATABLE_ENCODING 2
#define OBJECT_FILE_LINE_SIZE 12
#define INITIAL_ADDRESS 100

enum
{
    UNKNOWN_LINE_TYPE,
    EMPTY_LINE,
    COMMENT_LINE,
    NON_ENTRY_DIRECTIVE_LINE,
    ENTRY_LINE,
    OPERATION_LINE
} lineType;

enum
{
    UNKNOWN_ENCODING,
    NON_LABEL_ENCODING,
    DIRECT_ENCODING,
    MATRIX_ENCODING
} opEncodings;

int handleEntryLine(char **argP, int lineNum, assemblerContext *context,
                    symbolTable *entriesTable, char **entriesContentP,
                    size_t *entriesContentCapacityP);

int writeToEntries(symbol *symbolP, char **entriesContentP,
                   size_t *entriesContentCapacityP, int lineNum, assemblerContext *context);

int handleOperationLine(binaryWordNode **opWordP, char *lineCopy, int lineNum,
                        assemblerContext *context, symbolTable *entriesTable,
                        char **externalsContentP, size_t *externalsContentCapacityP);

int handleTwoOperandOpEncoding(binaryWordNode *opFirstWordP, char *lineCopy,
                               unsigned int srcOperandAddressEncoding,
                               unsigned int destOperandAddressEncoding,
                               int lineNum, assemblerContext *context,
                               symbolTable *entriesTable, char **externalsContentP,
                               size_t *externalsContentCapacityP);

int handleOneOperandOpEncoding(binaryWordNode *opFirstWordP, char *lineCopy,
                               unsigned int destOperandAddressEncoding, int lineNum,
                               assemblerContext *context, symbolTable *entriesTable,
                               char **externalsContentP, size_t *externalsContentCapacityP);

int encodeOpPayloadWord(char *label, int lineNum, assemblerContext *context,
                        symbolTable *entriesTable, binaryWordNode *currentWordP,
                        char **externalsContentP, size_t *externalsContentCapacityP);

int handleExternal(char *label, int lineNum, symbolTable *entriesTable,
                   binaryWordNode *currentWordP, char **externalsContentP,
                   size_t *externalsContentCapacityP, assemblerContext *context);

int handleData(char *label, symbol tempSymbol, int lineNum,
               binaryWordNode *currentWordP, assemblerContext *context);

int writeToExternals(binaryWordNode currentWordP, char *label, int lineNum,
                     char **externalsContentP, size_t *externalsContentCapacityP,
                     assemblerContext *context);

int createEntriesOutputFile(char *fileName, char *entriesContent, assemblerContext *context);

int createExternalsOutputFile(char *fileName, char *externalsContent, assemblerContext *context);

int createObjectOutputFile(char *fileName, binaryWordList codeImage, int ICF, int DCF,
                           assemblerContext *context);

char *intToBase4(int integer);

int BinarywordToInt(binaryWordNode word);

#endif