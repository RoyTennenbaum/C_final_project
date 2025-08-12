#ifndef SECOND_ITERATION_H
#define SECOND_ITERATION_H

#include "main.h"
#include "word-types.h"

#define MEMORY_ALLOCATION_ERROR -1
#define BASE4_ADDRESS_INIT "aaaa"
#define AER_EXTERNAL_ENCODING 1
#define AER_RELOCATABLE_ENCODING 2

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

int secondIteration(char *fileName, int ICF, int DCF, binaryWordList *codeImage,
                    assemblerContext *context, symbolTable *entriesTable);

int handleEntry(char **argP, assemblerContext *context, symbolTable *entriesTable,
                char **entriesContentP, size_t *entriesContentCapacityP, int lineNum);

writeToEntries(symbol *symbolP, char **entriesContentP,
               size_t *entriesContentCapacityP, int lineNum);

int handleOperation(binaryWordNode *opWordP, assemblerContext *context, symbolTable *entriesTable, const char *line, char **externalsContentP,
                    size_t *externalsContentCapacityP, int lineNum);

int handleTwoOperandOpEncoding(binaryWordNode *opFirstWordP,
                               unsigned int srcOperandAddressEncoding,
                               unsigned int destOperandAddressEncoding,
                               const assemblerContext *context, symbolTable *entriesTable, char *lineCopy,
                               char **extenalsContentP, size_t *externalsContentCapacityP,
                               int lineNum);

int encodeOpPayloadWord(const char *label, const assemblerContext *context,
                        symbolTable *entriesTable, binaryWordNode **currentWordP,
                        char **extenalsContentP, size_t *extenalsContentCapacityP,
                        int lineNum);

int handleExternal(const char *label, symbolTable *entriesTable, binaryWordNode **currentWordP,
                   char **extenalsContentP, size_t *extenalsContentCapacityP, int lineNum);

int writeToExternals(binaryWordNode currentWordP, const char *label, char **extenalsContentP,
                     size_t *extenalsContentCapacityP, int lineNum);

handleData(const char *label, binaryWordNode **currentWordP, symbol tempSymbol, int lineNum);
#endif