#ifndef SECOND_ITERATION_H
#define SECOND_ITERATION_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "word-types.h"

#define BASE4_ADDRESS_PADDING 4
#define BASE4_CODE_PADDING 5
#define AER_EXTERNAL_ENCODING 1
#define AER_RELOCATABLE_ENCODING 2
#define DIRECT_ADDRESS_ENCODING 1
#define MATRIX_ADDRESS_ENCODING 2
#define REGISTER_ADDRESS_ENCODING 3
#define OBJECT_FILE_LINE_SIZE 12
#define INITIAL_ADDRESS 100

enum {
  UNKNOWN_LINE_TYPE,
  EMPTY_LINE,
  COMMENT_LINE,
  NON_ENTRY_DIRECTIVE_LINE,
  ENTRY_LINE,
  OPERATION_LINE
};

enum { ADDRESS_REP, CODE_REP, COUNTER_REP } reps;

/* Entry handling functions */
void handleEntryLine(char *line, char **entriesContentP, size_t *entriesContentCapacityP,
                     symbolTable *entriesTable, const int lineNum, int *errorFlagP,
                     assemblerContext *context);

void writeToEntries(const symbol symbol, char **entriesContentP,
                    size_t *entriesContentCapacityP, const int lineNum, int *errorFlagP,
                    assemblerContext *context);

/* Operation handling functions */
void handleOperationLine(char *line, binaryWordNode **opWordP, char **externalsContentP,
                         size_t *externalsContentCapacityP, const int lineNum, int *errorFlagP,
                         assemblerContext *context, const symbolTable *entriesTable);

void handleTwoOperandOpCode(char *line, binaryWordNode **opWordP, char **externalsContentP,
                            size_t *externalsContentCapacityP, const unsigned srcAddressMethod,
                            const unsigned destAddressMethod, const int lineNum,
                            int *errorFlagP, assemblerContext *context,
                            const symbolTable *entriesTable);

void handleOneOperandOpCode(char *line, binaryWordNode **opWordP, char **externalsContentP,
                            size_t *externalsContentCapacityP,
                            const unsigned destAddressMethod, const int lineNum,
                            int *errorFlagP, assemblerContext *context,
                            const symbolTable *entriesTable);

/* Payload and symbol encoding functions */
void encodeOpPayloadWord(const char *label, binaryWordNode *currentWordP,
                         char **externalsContentP, size_t *externalsContentCapacityP,
                         const unsigned addressMethod, const int lineNum, int *errorFlagP,
                         assemblerContext *context, const symbolTable *entriesTable);

void handleExternal(const char *label, binaryWordNode *currentWordP, char **externalsContentP,
                    size_t *externalsContentCapacityP, const int lineNum, int *errorFlagP,
                    assemblerContext *context, const symbolTable *entriesTable);

void writeToExternals(const char *label, const binaryWordNode *currentWordP,
                      char **externalsContentP, size_t *externalsContentCapacityP,
                      const int lineNum, int *errorFlagP, assemblerContext *context);

void handleRelocatable(symbol symbol, binaryWordNode *currentWordP);

/* Output file creation functions */
void createObjectOutputFile(const char *fileName, const binaryWordList *codeImage,
                            const int ICF, const int DCF);

void createEntriesOutputFile(const char *fileName, const char *entriesContent);

void createExternalsOutputFile(const char *fileName, const char *externalsContent);

/* Utility functions */
char *intToBase4(unsigned integer, int rep, int lineNum);

unsigned int BinaryWordToInt(binaryWordNode word);
#endif